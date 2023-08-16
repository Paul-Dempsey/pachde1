#include "plugin.hpp"
#include "theme.hpp"

namespace pachde {

void DarkToJson(ITheme * itheme, json_t* root)
{
    std::string value = ToString(itheme->getDarkTheme());
    json_object_set_new(root, "dark-theme", json_stringn(value.c_str(), value.size()));
    json_object_set_new(root, "rack-dark", json_boolean(itheme->getFollowRack()));
}

void DarkFromJson(ITheme * itheme, json_t* root)
{
    itheme->setDarkTheme(DarkThemeFromJson(root));
    itheme->setFollowRack(GetBool(root, "rack-dark", itheme->getFollowRack()));
}

json_t* ThemeBase::save(json_t* root) {
    std::string value = ToString(theme);
    json_object_set_new(root, "theme", json_stringn(value.c_str(), value.size()));

    if (isColorOverride()) {
        auto color_string = rack::color::toHexString(main_color);
        json_object_set_new(root, "main-color", json_stringn(color_string.c_str(), color_string.size()));
    }

    json_object_set_new(root, "screws", json_boolean(screws));
    DarkToJson(this, root);
    return root;
}

void ThemeBase::load(json_t* root)
{
    theme = ThemeFromJson(root);

    auto j = json_object_get(root, "main-color");
    if (j) {
        auto color_string = json_string_value(j);
        main_color = rack::color::fromHexString(color_string);
    }

    screws = GetBool(root, "screws", screws);
    DarkFromJson(this, root);
};

}