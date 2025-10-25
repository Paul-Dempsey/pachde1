#include "Info.hpp"
#include "info_symbol.hpp"
#include "widgets/create-theme-widget.hpp"
#include "widgets/draw-logo.hpp"
#include "widgets/hamburger.hpp"
#include "widgets/logo-widget.hpp"
#include "widgets/screws.hpp"

using namespace widgetry;

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
        info_theme = new InfoTheme();
    }
    info_theme->setNotify(this);
    setModule(module);
    applyThemeSetting(info_theme->getThemeSetting());
}

void InfoModuleWidget::addResizeHandles()
{
    if (!my_module) return;

    auto handle = new ModuleResizeHandle(my_module);
    addChild(handle);

    handle = new ModuleResizeHandle(my_module);
    handle->right = true;
    addChild(handle);
}

void InfoModuleWidget::onChangeTheme(ChangedItem item) // override
{
    switch (item) {
    case ChangedItem::Theme:
    case ChangedItem::MainColor:
        sendChildrenThemeColor(this, info_theme->getTheme(), info_theme->getMainColor());
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

    AddScrewCaps(this, info_theme->getTheme(), info_theme->getMainColor(), ScrewAlign::SCREWS_OUTSIDE, WhichScrew::ALL_SCREWS);
}

void InfoModuleWidget::applyThemeSetting(ThemeSetting setting)
{
    info_theme->setThemeSetting(setting);
    if (children.empty()) {
        panel = new InfoPanel(my_module, info_theme, box.size);
        setPanel(panel);
        addResizeHandles();
        if (info_theme->hasScrews()) {
            addScrews();
        }

        title = createThemeWidgetCentered<InfoSymbol>(info_theme->getTheme(), Vec(box.size.x*.5f, 7.5f));
        addChild(title);

        logo = new LogoWidget(info_theme->getTheme(), .18f);
        logo->box.pos = Vec(box.size.x*.5f, RACK_GRID_HEIGHT - RACK_GRID_WIDTH + 7.5f);
        addChild(widgetry::Center(logo));

    } else {
        sendChildrenThemeColor(this, info_theme->getTheme(), info_theme->getMainColor());
    }
}

void InfoModuleWidget::step()
{
    bool changed = info_theme->pollRackThemeChanged();

    if (my_module)
    {
        box.size.x = my_module->width * RACK_GRID_WIDTH;
        // sync with module for change from presets
        if (!changed && my_module->isDirty()) {
            applyThemeSetting(info_theme->getThemeSetting());
            applyScrews(info_theme->hasScrews());
        }
        my_module->setClean();
    }

    panel->box.size = box.size;
    title->box.pos.x = box.size.x*.5f - title->box.size.x*.5f;
    logo->box.pos.x = box.size.x*.5f - logo->box.size.x*.5f;
    ModuleWidget::step();
}

// ----  Menu  --------------------------------------------------------------

struct FontSizeQuantity : Quantity
{
    InfoTheme* info_theme;
    explicit FontSizeQuantity(InfoTheme* it) {
        info_theme = it;
    }
    void setValue(float value) override { info_theme->setFontSize(value); }
    float getValue() override { return info_theme->getFontSize(); }
    float getMinValue() override { return info_constant::MIN_FONT_SIZE; }
    float getMaxValue() override { return info_constant::MAX_FONT_SIZE; }
    float getDefaultValue() override { return info_constant::DEFAULT_FONT_SIZE; }
    int getDisplayPrecision() override { return 3; }
    std::string getLabel() override { return "Font size"; }
    std::string getUnit() override { return "px"; }
};

struct FontSizeSlider : ui::Slider
{
    explicit FontSizeSlider(InfoTheme* info_theme) {
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

    menu->addChild(createMenuLabel<HamburgerTitle>("#d Info"));
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
        [=](Menu *menu) {
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
        [=](Menu *menu) {
            MenuTextField *editField = new MenuTextField();
            editField->box.size.x = 100;
            if (packed_color::visible(info_theme->user_text_color)) {
                char hex[10];
                hexFormat(info_theme->user_text_color, sizeof(hex), hex);
                editField->setText(hex);
            } else {
                editField->setText(HEXPLACEHOLDER);
            }
            editField->changeHandler = [=](std::string text) {
                auto color{colors::Black};
                if (!text.empty()) {
                    parseColor(color, colors::Black, text.c_str());
                }
                info_theme->setUserTextColor(color);
            };
            menu->addChild(editField);
        }));

    menu->addChild(createSubmenuItem("Copper", "",
        [=](Menu *menu) {
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
                "Text", "",
                [=]() { return my_module->getCopperTarget() == CopperTarget::Text; },
                [=]() { my_module->setCopperTarget(CopperTarget::Text); }
                ));
        }));

}

}