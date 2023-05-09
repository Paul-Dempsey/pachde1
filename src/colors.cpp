#include <rack.hpp>
#include "colors.hpp"

Theme ParseTheme(std::string text) {
    if (text.empty()) return Theme::Unset;
    switch (text[0]) {
        case 'l': case 'L': return Theme::Light;
        case 'd': case 'D': return Theme::Dark;
        case 'h': case 'H': return Theme::HighContrast;
    }
    return Theme::Unset;
}

std::string ToString(Theme t) {
    switch (t) {
        case Theme::Light: return "light";
        case Theme::Dark: return "dark";
        case Theme::HighContrast: return "highcontrast";
        default: return "";
    }
}

Theme ThemeFromJson(json_t * root) {
    json_t* themeJ = json_object_get(root, "theme");
    return themeJ ? ParseTheme(json_string_value(themeJ)) : Theme::Unset;
}