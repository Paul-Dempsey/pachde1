#include "components.hpp"

void ThemeModule::onReset() {
    dirty = true;
}

json_t *ThemeModule::dataToJson()
{
    // derived classes call base (ThemeModule::)dataToJson to get json root
    json_t *rootJ = json_object();

    std::string value = ToString(theme);
    json_object_set_new(rootJ, "theme", json_stringn(value.c_str(), value.size()));

    return rootJ;
}

void ThemeModule::dataFromJson(json_t *rootJ)
{
    // derived classes call base
    theme = ThemeFromJson(rootJ);
    dirty = true;
}

void ThemeModule::addThemeMenu(Menu *menu, IChangeTheme* change) {
    menu->addChild(new MenuSeparator);
    menu->addChild(createSubmenuItem("Theme", "",
        [=](Menu *menu)
        {
            menu->addChild(createCheckMenuItem(
                "Light", "",
                [=]() { return getTheme() == Theme::Light; },
                [=]() {
                    setTheme(Theme::Light);
                    if (change) { change->onChangeTheme(); }
                }));
            menu->addChild(createCheckMenuItem(
                "Dark", "",
                [=]() { return getTheme() == Theme::Dark; },
                [=]() {
                    setTheme(Theme::Dark);
                    if (change) { change->onChangeTheme(); }
                }));
        }));
}

void ThemePanel::draw(const DrawArgs &args)
{
    nvgBeginPath(args.vg);
    nvgRect(args.vg, 0.0, 0.0, box.size.x, box.size.y);
    nvgFillColor(args.vg, PanelBackground(theme));
    nvgFill(args.vg);

    Widget::draw(args);
}
