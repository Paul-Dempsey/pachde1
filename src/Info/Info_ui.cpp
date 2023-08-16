#include "info.hpp"
#include "info_symbol.hpp"

namespace pachde {

const char * HEXPLACEHOLDER = "#<hexcolor>";

InfoModuleWidget::InfoModuleWidget(InfoModule* module)
{
    my_module = module;
    if (module) {
        box.size = Vec(module->width * RACK_GRID_WIDTH, RACK_GRID_HEIGHT);
        info_theme = module->getInfoTheme();
    } else {
        box.size = Vec(RACK_GRID_WIDTH * 8, RACK_GRID_HEIGHT);
        info_theme = new InfoTheme(nullptr);
    }
    info_theme->setNotify(this);
    setModule(module);
    applyTheme(info_theme->getTheme());
}

void InfoModuleWidget::addResizeHandles()
{
    if (!my_module) return;

    auto handle = new ModuleResizeHandle;
    handle->module = my_module;
    addChild(handle);

    handle = new ModuleResizeHandle;
    handle->right = true;
    handle->module = my_module;
    addChild(handle);
}

void InfoModuleWidget::onChangeTheme(ChangedItem item) // override
{
    switch (item) {
    case ChangedItem::Theme:
        applyTheme(GetPreferredTheme(info_theme));
        break;
    case ChangedItem::FollowDark:
    case ChangedItem::DarkTheme:
        if (info_theme->getFollowRack()) {
            applyTheme(GetPreferredTheme(info_theme));
        }
        break;
    case ChangedItem::MainColor:
        SetChildrenThemeColor(this, info_theme->getMainColor());
        break;
    case ChangedItem::Screws:
        applyScrews(info_theme->hasScrews());
        break;
    }
}

void InfoModuleWidget::applyScrews(bool screws)
{
    //info_theme->setScrews(screws);
    if (screws) {
        addScrews();
    } else {
        RemoveScrewCaps(this);
    }
}

void InfoModuleWidget::addScrews()
{
    if (HaveScrewChildren(this)) return;

    auto theme = GetPreferredTheme(info_theme);
    auto main_color = info_theme->getMainColor();
    AddScrewCaps(this, theme, main_color, ScrewAlign::SCREWS_OUTSIDE, WhichScrew::ALL_SCREWS);
}

void InfoModuleWidget::applyTheme(Theme theme)
{
    if (children.empty()) {
        panel = new InfoPanel(my_module, info_theme, box.size);
        setPanel(panel);
        addResizeHandles();
        if (info_theme->hasScrews()) {
            addScrews();
        }

        title = createThemeWidgetCentered<InfoSymbol>(theme, Vec(box.size.x / 2, 7.5f));
        addChild(title);

        logo = createThemeWidgetCentered<LogoOverlayWidget>(theme, Vec(box.size.x / 2, RACK_GRID_HEIGHT - RACK_GRID_WIDTH + 7.5f));
        addChild(logo);

    } else {
        info_theme->applyTheme(theme);
        SetChildrenTheme(this, theme);
        SetChildrenThemeColor(this, info_theme->getMainColor());
    }
}

void InfoModuleWidget::step()
{
    bool changed = info_theme->pollRackDarkChanged();

    if (my_module)
    {
        box.size.x = my_module->width * RACK_GRID_WIDTH;
        // sync with module for change from presets
        if (!changed && my_module->isDirty()) {
            applyTheme(GetPreferredTheme(info_theme));
            applyScrews(info_theme->hasScrews());
        }
        my_module->setClean();
    }

    panel->box.size = box.size;
    title->box.pos.x = box.size.x / 2 - title->box.size.x / 2;
    logo->box.pos.x = box.size.x / 2 - logo->box.size.x / 2;
    ModuleWidget::step();
}

// ----  Menu  --------------------------------------------------------------

struct FontSizeQuantity : Quantity {
    InfoTheme* info_theme;
    FontSizeQuantity(InfoTheme* it) {
        info_theme = it;
    }
    void setValue(float value) override { info_theme->setFontSize(value); }
    float getValue() override { return info_theme->getFontSize(); }
    float getMinValue() override { return MIN_FONT_SIZE; }
    float getMaxValue() override { return MAX_FONT_SIZE; }
    float getDefaultValue() override { return DEFAULT_FONT_SIZE; }
    int getDisplayPrecision() override { return 3; }
    std::string getLabel() override { return "Font size"; }
    std::string getUnit() override { return "px"; }
};

struct FontSizeSlider : ui::Slider {
    FontSizeSlider(InfoTheme* info_theme) {
        quantity = new FontSizeQuantity(info_theme);
    }
    ~FontSizeSlider() {
        delete quantity;
    }
};


void InfoModuleWidget::appendContextMenu(Menu *menu)
{
    if (!my_module)
        return;
    AddThemeMenu(menu, info_theme, true, true);

    menu->addChild(createCheckMenuItem("Bright text in a dark room", "",
        [=]() { return info_theme->getBrilliant(); },
        [=]() { info_theme->toggleBrilliant(); }));

    menu->addChild(new MenuSeparator);

    menu->addChild(createSubmenuItem("Edit Info", "",
        [=](Menu *menu)
        {
            MenuTextField *editField = new MenuTextField();
            editField->box.size.x = 200.f;
            editField->box.size.y = 100.f;
            editField->setText(my_module->text);
            editField->commitHandler = [=](std::string text) {
                my_module->text = text;
            };
            menu->addChild(editField);
            }));
    menu->addChild(createMenuItem("Copy info", "", [=]() {
        if (!my_module->text.empty())
        {
            glfwSetClipboardString(APP->window->win, my_module->text.c_str());
        }
    }, my_module->text.empty()));
    menu->addChild(createMenuItem("Paste info", "", [=]() {
        auto text = glfwGetClipboardString(APP->window->win);
        if (text) my_module->text = text;
    }));

    menu->addChild(new MenuSeparator);

    auto name = system::getStem(info_theme->font_file);
    menu->addChild(construct<MenuLabel>(&MenuLabel::text, name));
    menu->addChild(createMenuItem("Font...", "", [=]() {
        info_theme->fontDialog();
    }));

    FontSizeSlider* slider = new FontSizeSlider(info_theme);
    slider->box.size.x = 250.0;
    menu->addChild(slider);

    menu->addChild(createSubmenuItem("Text alignment", "",
        [=](Menu *menu)
        {
            menu->addChild(createCheckMenuItem(
                "Left", "",
                [=]() { return info_theme->getHorizontalAlignment() == HAlign::Left; },
                [=]() { info_theme->setHorizontalAlignment(HAlign::Left); }
                ));
            menu->addChild(createCheckMenuItem(
                "Center", "",
                [=]() { return info_theme->getHorizontalAlignment() == HAlign::Center; },
                [=]() { info_theme->setHorizontalAlignment(HAlign::Center); }
                ));
            menu->addChild(createCheckMenuItem(
                "Right", "",
                [=]() { return info_theme->getHorizontalAlignment() == HAlign::Right; },
                [=]() { info_theme->setHorizontalAlignment(HAlign::Right); }
                ));
        }));

    menu->addChild(createSubmenuItem("Text color", "",
        [=](Menu *menu)
        {
            MenuTextField *editField = new MenuTextField();
            editField->box.size.x = 100;
            if (isColorVisible(info_theme->user_text_color)) {
                editField->setText(rack::color::toHexString(info_theme->user_text_color));
            } else {
                editField->setText(HEXPLACEHOLDER);
            }
            editField->changeHandler = [=](std::string text) {
                auto color = COLOR_NONE;
                if (!text.empty() && text[0] == '#') {
                    color = rack::color::fromHexString(text);
                }
                info_theme->setUserTextColor(color);
            };
            menu->addChild(editField);
        }));

    menu->addChild(createSubmenuItem("Text background color", "",
        [=](Menu *menu)
        {
            MenuTextField *editField = new MenuTextField();
            editField->box.size.x = 100;
            if (isColorVisible(info_theme->user_text_background)) {
                editField->setText(rack::color::toHexString(info_theme->user_text_background));
            } else {
                editField->setText(HEXPLACEHOLDER);
            }
            editField->changeHandler = [=](std::string text) {
                auto color = COLOR_NONE;
                if (!text.empty() && text[0] == '#') {
                    color = rack::color::fromHexString(text);
                }
                info_theme->setUserTextBackground(color);
            };
            menu->addChild(editField);
        }));

    menu->addChild(createSubmenuItem("Copper", "",
        [=](Menu *menu)
        {
            menu->addChild(createCheckMenuItem(
                "None", "",
                [=]() { return my_module->getCopperTarget() == CopperTarget::None; },
                [=]() { my_module->setCopperTarget(CopperTarget::None); }
                ));
            menu->addChild(createCheckMenuItem(
                "Panel", "",
                [=]() { return my_module->getCopperTarget() == CopperTarget::Panel; },
                [=]() { my_module->setCopperTarget(CopperTarget::Panel); }
                ));
            menu->addChild(createCheckMenuItem(
                "Interior (L/R B/T)", "",
                [=]() { return my_module->getCopperTarget() == CopperTarget::Interior; },
                [=]() { my_module->setCopperTarget(CopperTarget::Interior); }
                ));
        }));

}

}