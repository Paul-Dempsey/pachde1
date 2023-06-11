#include "components.hpp"

namespace pachde {

void ThemeModule::onReset() {
    dirty = true;
}

// derived classes call ThemeModule::dataToJson to get json root
json_t* ThemeModule::dataToJson()
{
    json_t* root = json_object();

    std::string value = ToString(theme);
    json_object_set_new(root, "theme", json_stringn(value.c_str(), value.size()));

    if (isColorVisible(panel_color)) {
        auto color_string = rack::color::toHexString(panel_color);
        json_object_set_new(root, "panel-color", json_stringn(color_string.c_str(), color_string.size()));
    }

    json_object_set_new(root, "screws", json_boolean(show_screws));

    return root;
}

// derived classes must call ThemeModule::dataFromJson
void ThemeModule::dataFromJson(json_t *root)
{
    theme = ThemeFromJson(root);

    auto j = json_object_get(root, "panel-color");
    if (j) {
        auto color_string = json_string_value(j);
        panel_color = rack::color::fromHexString(color_string);
    }

    j = json_object_get(root, "screws");
    show_screws = j ? json_is_true(j) : true;

    dirty = true;
}

struct ThemeColorField : ui::TextField {
    IChangeTheme* change = nullptr;

    ThemeColorField(IChangeTheme* ict) {
        change = ict;
    }

    void onChange(const ChangeEvent& e) override {
        ui::TextField::onChange(e);
        if (change) { 
            auto color = COLOR_NONE;
            auto text = getText();
            if (!text.empty() && text[0] == '#') {
                color = rack::color::fromHexString(text);
            }
            change->setColor(color);
        }
    }
};

void ThemeModule::addThemeMenu(Menu *menu, IChangeTheme* change, bool isChangeColor, bool isChangeScrews) {
    menu->addChild(new MenuSeparator);
    menu->addChild(createSubmenuItem("Theme", "",
        [=](Menu *menu)
        {
            menu->addChild(createCheckMenuItem(
                "Light", "",
                [=]() { return getTheme() == Theme::Light; },
                [=]() {
                    setTheme(Theme::Light);
                    if (change) { change->setTheme(Theme::Light); }
                }));
            menu->addChild(createCheckMenuItem(
                "Dark", "",
                [=]() { return getTheme() == Theme::Dark; },
                [=]() {
                    setTheme(Theme::Dark);
                    if (change) { change->setTheme(Theme::Dark); }
                }));
            menu->addChild(createCheckMenuItem(
                "High contrast", "",
                [=]() { return getTheme() == Theme::HighContrast; },
                [=]() {
                    setTheme(Theme::HighContrast);
                    if (change) { change->setTheme(Theme::HighContrast); }
                }));
        }));
    if (isChangeColor) {
        menu->addChild(createSubmenuItem("Panel Color", "",
            [=](Menu *menu)
            {
                auto tw = new ThemeColorField(change);
                tw->setText(rack::color::toHexString(getColor()));
                tw->box.size.x = 100;
                menu->addChild(tw);
            }));
    }
    if (isChangeScrews) {
        menu->addChild(createCheckMenuItem("Screws", "",
            [=]() { return hasScrews(); }, 
            [=]() { if (change) change->setScrews(!hasScrews()); }
            ));
    }
}

}