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
        settings = module->getSettings();
        theme_holder = my_module;
    } else {
        box.size = Vec(RACK_GRID_WIDTH * 8, RACK_GRID_HEIGHT);
        settings = new InfoSettings();
        theme_holder = new ThemeBase();
    }
    setModule(module);
    applyThemeSetting(theme_holder->getThemeSetting());
    theme_holder->setNotify(this);
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
        settings->setTheme(theme_holder->getTheme());
        sendChildrenThemeColor(this, theme_holder->getTheme(), theme_holder->getMainColor());
        break;
    case ChangedItem::Screws:
        applyScrews(theme_holder->hasScrews());
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
    AddScrewCaps(this, theme_holder->getTheme(), theme_holder->getMainColor(), ScrewAlign::SCREWS_OUTSIDE, WhichScrew::ALL_SCREWS);
}

void InfoModuleWidget::applyThemeSetting(ThemeSetting setting)
{
    theme_holder->setThemeSetting(setting);
    settings->setTheme(theme_holder->getTheme());
    if (children.empty()) {
        panel = new InfoPanel(my_module, settings, theme_holder, box.size);
        setPanel(panel);
        addResizeHandles();
        if (theme_holder->hasScrews()) {
            addScrews();
        }

        title = createThemeWidgetCentered<InfoSymbol>(theme_holder->getTheme(), Vec(box.size.x*.5f, 7.5f));
        addChild(title);

        logo = new LogoWidget(theme_holder->getTheme(), .18f);
        logo->box.pos = Vec(box.size.x*.5f, RACK_GRID_HEIGHT - RACK_GRID_WIDTH + 7.5f);
        addChild(widgetry::Center(logo));
    } else {
        sendChildrenThemeColor(this, theme_holder->getTheme(), theme_holder->getMainColor());
    }
}

void InfoModuleWidget::step()
{
    bool changed = theme_holder->pollRackThemeChanged();

    if (my_module)
    {
        box.size.x = my_module->width * RACK_GRID_WIDTH;
        // sync with module for change from presets
        if (!changed && my_module->isDirty()) {
            applyThemeSetting(theme_holder->getThemeSetting());
            applyScrews(theme_holder->hasScrews());
        }
        my_module->setClean();
    }
    panel->box.size = box.size;
    title->box.pos.x = box.size.x*.5f - title->box.size.x*.5f;
    title->setVisible(settings->getBranding());
    logo->box.pos.x = box.size.x*.5f - logo->box.size.x*.5f;
    logo->setVisible(settings->getBranding());
    ModuleWidget::step();
}

// ----  Menu  --------------------------------------------------------------

struct FontSizeQuantity : Quantity
{
    InfoSettings* settings{nullptr};
    explicit FontSizeQuantity(InfoSettings* settings) : settings(settings) {}
    void setValue(float value) override { settings->setFontSize(value); }
    float getValue() override { return settings->getFontSize(); }
    float getMinValue() override { return info_constant::MIN_FONT_SIZE; }
    float getMaxValue() override { return info_constant::MAX_FONT_SIZE; }
    float getDefaultValue() override { return info_constant::DEFAULT_FONT_SIZE; }
    int getDisplayPrecision() override { return 3; }
    std::string getLabel() override { return "Font size"; }
    std::string getUnit() override { return "px"; }
};

struct FontSizeSlider : ui::Slider
{
    explicit FontSizeSlider(InfoSettings* settings) {
        quantity = new FontSizeQuantity(settings);
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
    AddThemeMenu(menu, theme_holder, true, true);

    menu->addChild(createCheckMenuItem("Bright text in a dark room", "",
        [=]() { return settings->getBrilliant(); },
        [=]() { settings->setBrilliant(!settings->getBrilliant()); }));

    menu->addChild(createCheckMenuItem(
        "Show branding", "",
        [=]() { return settings->getBranding(); },
        [=]() { settings->setBranding(!settings->getBranding()); }));

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

    auto name = system::getStem(settings->font_file);
    menu->addChild(construct<MenuLabel>(&MenuLabel::text, name));
    menu->addChild(createMenuItem("Font...", "", [=]() {
        settings->fontDialog();
    }));

    FontSizeSlider* slider = new FontSizeSlider(settings);
    slider->box.size.x = 250.0;
    menu->addChild(slider);

    menu->addChild(createSubmenuItem("Orientation", "",
        [=](Menu* menu) {
            NVGcolor co_dot{nvgHSL(200.f/360.f, .5, .5)};

            ColorDotMenuItem* option;
            option = createMenuItem<ColorDotMenuItem>(OrientationName(Orientation::Normal), "",
                [=](){ settings->setOrientation(Orientation::Normal); }, false);
            option->color = settings->getOrientation() == Orientation::Normal ? co_dot : RampGray(G_45);
            menu->addChild(option);

            option = createMenuItem<ColorDotMenuItem>(OrientationName(Orientation::Down), "",
                [=](){ settings->setOrientation(Orientation::Down); }, false);
            option->color = settings->getOrientation() == Orientation::Down ? co_dot : RampGray(G_45);
            menu->addChild(option);

            option = createMenuItem<ColorDotMenuItem>(OrientationName(Orientation::Up), "",
                [=](){ settings->setOrientation(Orientation::Up); }, false);
            option->color = settings->getOrientation() == Orientation::Up ? co_dot : RampGray(G_45);
            menu->addChild(option);

            option = createMenuItem<ColorDotMenuItem>(OrientationName(Orientation::Inverted), "",
                [=](){ settings->setOrientation(Orientation::Inverted); }, false);
            option->color = settings->getOrientation() == Orientation::Inverted ? co_dot : RampGray(G_45);
            menu->addChild(option);
        }));
    menu->addChild(createSubmenuItem("Text alignment", "",
        [=](Menu *menu) {
            menu->addChild(createCheckMenuItem(
                "Left", "",
                [=]() { return settings->getHorizontalAlignment() == HAlign::Left; },
                [=]() { settings->setHorizontalAlignment(HAlign::Left); }
                ));
            menu->addChild(createCheckMenuItem(
                "Center", "",
                [=]() { return settings->getHorizontalAlignment() == HAlign::Center; },
                [=]() { settings->setHorizontalAlignment(HAlign::Center); }
                ));
            menu->addChild(createCheckMenuItem(
                "Right", "",
                [=]() { return settings->getHorizontalAlignment() == HAlign::Right; },
                [=]() { settings->setHorizontalAlignment(HAlign::Right); }
                ));
        }));

    menu->addChild(createSubmenuItem("Text color", "",
        [=](Menu *menu) {
            MenuTextField *editField = new MenuTextField();
            editField->box.size.x = 100;
            if (packed_color::visible(settings->user_text_color)) {
                char hex[10];
                hexFormat(settings->user_text_color, sizeof(hex), hex);
                editField->setText(hex);
            } else {
                editField->setText(HEXPLACEHOLDER);
            }
            editField->changeHandler = [=](std::string text) {
                auto color{colors::Black};
                if (!text.empty()) {
                    parseColor(color, colors::Black, text.c_str());
                }
                settings->setUserTextColor(color);
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