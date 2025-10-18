#include "../plugin.hpp"
#include "theme.hpp"
#include "json-help.hpp"

namespace pachde {

void DarkToJson(ITheme * itheme, json_t* root)
{
    std::string value = ToString(itheme->getDarkTheme());
    set_json(root, "dark-theme", value);
    set_json(root, "rack-dark", itheme->getFollowRack());
}

void DarkFromJson(ITheme * itheme, json_t* root)
{
    itheme->setDarkTheme(DarkThemeFromJson(root));
    itheme->setFollowRack(get_json_bool(root, "rack-dark", itheme->getFollowRack()));
}

Theme ParseTheme(std::string text) {
    if (text.empty()) return DefaultTheme;
    switch (text[0]) {
        case 'l': case 'L': return Theme::Light;
        case 'd': case 'D': return Theme::Dark;
        case 'h': case 'H': return Theme::HighContrast;
    }
    return DefaultTheme;
}

std::string ToString(Theme t) {
    switch (t) {
        default:
        case Theme::Light: return "light";
        case Theme::Dark: return "dark";
        case Theme::HighContrast: return "highcontrast";
    }
}

Theme ThemeFromJson(json_t * root) {
    json_t* j = json_object_get(root, "theme");
    return j ? ParseTheme(json_string_value(j)) : DefaultTheme;
}

Theme DarkThemeFromJson(json_t * root) {
    json_t* j = json_object_get(root, "dark-theme");
    return j ? ParseTheme(json_string_value(j)) : Theme::Dark;
}


void ThemeBase::reset()
{
    setMainColor(COLOR_NONE);
    setTheme(DefaultTheme);
    setDarkTheme(Theme::Dark);
    setFollowRack(true);
    setScrews(true);
}

json_t* ThemeBase::save(json_t* root) {
    std::string value = ToString(my_theme);
    set_json(root, "theme", value);

    if (isColorOverride()) {
        auto color_string = rack::color::toHexString(main_color);
        set_json(root, "main-color", color_string);
    }

    set_json(root, "screws", screws);
    DarkToJson(this, root);
    return root;
}

void ThemeBase::load(json_t* root)
{
    my_theme = ThemeFromJson(root);

    auto j = json_object_get(root, "main-color");
    if (j) {
        auto color_string = json_string_value(j);
        main_color = rack::color::fromHexString(color_string);
    }

    screws = get_json_bool(root, "screws", screws);
    DarkFromJson(this, root);
};

void RandomizeTheme(ITheme* itheme, bool opaque)
{
    itheme->setMainColor(opaque ? RandomOpaqueColor() : RandomColor());
    itheme->setScrews(random::get<bool>());
    itheme->setTheme(RandomTheme());
    itheme->setDarkTheme(RandomTheme());
    itheme->setFollowRack(random::get<bool>());
}

}