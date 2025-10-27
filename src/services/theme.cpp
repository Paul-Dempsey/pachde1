#include "../myplugin.hpp"
#include "theme.hpp"
#include "json-help.hpp"

namespace pachde {

void sendChildrenTheme(Widget *widget, Theme theme) {
    for (Widget* child : widget->children) {
        sendChildrenTheme(child, theme);
    }
    auto themed = dynamic_cast<ISetTheme*>(widget);
    if (themed) themed->setTheme(theme);
}

void sendChildrenThemeColor(Widget *widget, Theme theme, PackedColor main)
{
    for (Widget* child : widget->children) {
        sendChildrenThemeColor(child, theme, main);
    }
    auto themed = dynamic_cast<ISetTheme*>(widget);
    if (themed) themed->setTheme(theme);

    auto colored = dynamic_cast<ISetColor*>(widget);
    if (colored) colored->setMainColor(main);
}

Theme ParseThemeName(const std::string &name)
{
    if (name.empty()) return Theme::Unset;
    switch (*name.cbegin()) {
    case 'L': if ((name.size() == 5) && name == "Light") return Theme::Light; break;
    case 'D': if ((name.size() == 4) && name == "Dark") return Theme::Dark; break;
    case 'H': if ((name.size() == 13) && (name == "High Contrast")) return Theme::HighContrast; break;
    }
    return Theme::Unset;
}

const char *ThemeName(Theme choice)
{
    switch (choice) {
    case Theme::Light: return "Light";
    case Theme::Dark: return "Dark";
    case Theme::HighContrast: return "High Contrast";
    default: return "Dark";
    }
}

Theme getActualTheme(ThemeSetting choice) {
    switch (choice) {
    case ThemeSetting::Light:        return Theme::Light;
    case ThemeSetting::Dark:         return Theme::Dark;
    case ThemeSetting::HighContrast: return Theme::HighContrast;

    case ThemeSetting::FollowRackUi:
        if (::rack::settings::uiTheme == "light")  return Theme::Light;
        if (::rack::settings::uiTheme == "dark")   return Theme::Dark;
        if (::rack::settings::uiTheme == "hcdark") return Theme::HighContrast;
        return Theme::Dark;

    case ThemeSetting::FollowRackPreferDark:
        return ::rack::settings::preferDarkPanels
            ? ((::rack::settings::uiTheme == "hcdark") ? Theme::HighContrast : Theme::Dark)
            : Theme::Light;
    }
    return Theme::Dark;
}

ThemeSetting ParseThemeSettingShorthand(std::string text) {
    if (text.empty()) return DefaultThemeSetting;
    switch (text[0]) {
    case 'l': case 'L': return ThemeSetting::Light;
    case 'd': case 'D': return ThemeSetting::Dark;
    case 'h': case 'H': return ThemeSetting::HighContrast;
    case 'r': case 'R': return ThemeSetting::FollowRackUi;
    case 'f': case 'F': return ThemeSetting::FollowRackPreferDark;
    }
    return DefaultThemeSetting;
}

ThemeSetting ThemeSettingFromJson(json_t* root) {
    return ParseThemeSettingShorthand(get_json_string(root, "theme-choice", "f"));
}

void broadcastThemeSetting(::rack::app::ModuleWidget*source, ThemeSetting setting)
{
    ::rack::app::RackWidget* rack = APP->scene->rack;
    std::vector<::rack::app::ModuleWidget*> module_widgets{rack->getModules()};
    if (module_widgets.size() <= 1) return;
    for (auto module_widget: module_widgets) {
        if (module_widget == source) continue; // skip broadcaster
        if (module_widget->model->plugin == pluginInstance) {
            IBasicTheme* theme = dynamic_cast<IBasicTheme*>(module_widget->module);
            if (theme) {
                theme->setThemeSetting(setting);
            }
        }
    }
}

void ThemeSettingToJson(json_t* root, ThemeSetting choice) {
    const char * value{"f"};
    switch (choice) {
    case ThemeSetting::Light: value = "light"; break;
    case ThemeSetting::Dark: value = "dark"; break;
    case ThemeSetting::HighContrast: value = "hc"; break;
    case ThemeSetting::FollowRackUi: value = "rack-ui"; break;
    case ThemeSetting::FollowRackPreferDark: value = "follow"; break;
    }
    set_json(root, "theme-choice", value);
}

void ThemeBase::reset() {
    setMainColor(colors::NoColor);
    setThemeSetting(DefaultThemeSetting);
    setScrews(true);
}

void ThemeBase::randomize() { RandomizeTheme(this, true); }

json_t* ThemeBase::save(json_t* root) {
    ThemeSettingToJson(root,theme_setting);
    set_json(root, "screws", screws);
    char hex_color[10];
    packed_color::hexFormat(main_color, sizeof(hex_color), hex_color);
    set_json(root, "main-color", hex_color);
    return root;
}

void ThemeBase::load(json_t* root)
{
    theme_setting = ThemeSettingFromJson(root);
    actual_theme = getActualTheme(theme_setting);
    screws = get_json_bool(root, "screws", screws);
    auto color_string = get_json_string(root, "main-color");
    if (color_string.empty()) {
        main_color = colors::NoColor;
    } else {
        parseHexColor(main_color, colors::NoColor, color_string.c_str(), nullptr);
    }
};

void RandomizeTheme(ITheme* itheme, bool opaque)
{
    itheme->setMainColor(opaque ? packed_color::opaque(random::u32()) : random::u32());
    itheme->setScrews(random::get<bool>());
    itheme->setThemeSetting(RandomThemeSetting());
}

}