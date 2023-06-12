#include "theme.hpp"

namespace pachde {

json_t* ThemeBase::save(json_t* root) {
    std::string value = ToString(theme);
    json_object_set_new(root, "theme", json_stringn(value.c_str(), value.size()));

    if (isColorOverride()) {
        auto color_string = rack::color::toHexString(panel_color);
        json_object_set_new(root, "panel-color", json_stringn(color_string.c_str(), color_string.size()));
    }

    json_object_set_new(root, "screws", json_boolean(screws));
    return root;
}

void ThemeBase::load(json_t* root)
{
    theme = ThemeFromJson(root);

    auto j = json_object_get(root, "panel-color");
    if (j) {
        auto color_string = json_string_value(j);
        panel_color = rack::color::fromHexString(color_string);
    }

    j = json_object_get(root, "screws");
    screws = j ? json_is_true(j) : true;
};

}