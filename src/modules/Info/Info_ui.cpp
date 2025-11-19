#include "Info.hpp"
#include "info_symbol.hpp"
#include "widgets/create-theme-widget.hpp"
#include "widgets/draw-logo.hpp"
#include "widgets/color-picker.hpp"
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
        settings->setTheme(theme_holder->getTheme());
        sendChildrenThemeColor(this, theme_holder->getTheme(), theme_holder->getMainColor());
        break;
    case ChangedItem::MainColor:
        settings->setUserPanelColor(theme_holder->getMainColor());
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

        title = Center(createThemeWidget<InfoSymbol>(theme_holder->getTheme(), Vec(box.size.x*.5f, 7.5f)));
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
    void setValue(float value) override { settings->setFontSize(::rack::math::clamp(value, getMinValue(), getMaxValue())); }
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

struct MarginQuantity : Quantity
{
    float* data{nullptr};
    std::string label;
    explicit MarginQuantity(float* value, const std::string& name) : data(value), label(name) {}
    void setValue(float value) override { *data = ::rack::math::clamp(value, getMinValue(), getMaxValue()); }
    float getValue() override { return *data; }
    float getMinValue() override { return 0.f; }
    float getMaxValue() override { return 30.f; }
    float getDefaultValue() override { return 0.f; }
    int getDisplayPrecision() override { return 3; }
    std::string getLabel() override { return label; }
    std::string getUnit() override { return "px"; }
};
struct MarginSlider : ui::Slider
{
    explicit MarginSlider(float* value, const std::string& name) {
        quantity = new MarginQuantity(value, name);
    }
    ~MarginSlider() {
        delete quantity;
    }
};
void InfoModuleWidget::appendContextMenu(Menu *menu)
{
    if (!my_module)
        return;

    menu->addChild(createMenuLabel<HamburgerTitle>("#d Info"));

    AddThemeMenu(menu, this, theme_holder, true, true);

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
    slider->box.size.x = 250.f;
    menu->addChild(slider);

    MarginSlider* mslider = new MarginSlider(&settings->left_margin, "Left margin");
    mslider->box.size.x = 250.f;
    menu->addChild(mslider);

    mslider = new MarginSlider(&settings->right_margin, "Right margin");
    mslider->box.size.x = 250.f;
    menu->addChild(mslider);

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
            menu->addChild(new MenuSeparator);
            menu->addChild(createCheckMenuItem(
                "Top", "",
                [=]() { return settings->getVerticalAlignment() == VAlign::Top; },
                [=]() { settings->setVerticalAlignment(VAlign::Top); }
                ));
            menu->addChild(createCheckMenuItem(
                "Middle", "",
                [=]() { return settings->getVerticalAlignment() == VAlign::Middle; },
                [=]() { settings->setVerticalAlignment(VAlign::Middle); }
                ));
            menu->addChild(createCheckMenuItem(
                "Bottom", "",
                [=]() { return settings->getVerticalAlignment() == VAlign::Bottom; },
                [=]() { settings->setVerticalAlignment(VAlign::Bottom); }
                ));

        }));

    // menu->addChild(createSubmenuItem("Background", "", [=](Menu* menu) {
    //     auto picker = new ColorPickerMenu();
    //     picker->set_color(my_module->getMainColor());
    //     picker->set_on_new_color([=](PackedColor color) {
    //         my_module->setMainColor(color);
    //     });
    //     menu->addChild(picker);
    // }));

    menu->addChild(createSubmenuItem("Text color", "", [=](Menu* menu) {
        auto picker = new ColorPickerMenu();
        picker->set_color(settings->getUserTextColor());
        picker->set_on_new_color([=](PackedColor color) {
            settings->setUserTextColor(color);
        });
        menu->addChild(picker);
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