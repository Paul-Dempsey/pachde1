#include "rails.hpp"
#include "services/json-help.hpp"
#include "services/svg-theme-load.hpp"

extern ::rack::Plugin* pluginInstance;

namespace pachde {

svg_theme::ThemeCache rail_theme_cache;
const std::set<std::string> known_rails{"Rack", "Blank", "Plain", "Simple", "No-hole", "Pinstripe", "Gradient", "Sine"};

struct RailThemeData {
    static std::string light() { return asset::plugin(pluginInstance, "res/rails/-light.vgt"); }
    static std::string dark() { return asset::plugin(pluginInstance, "res/rails/-light.vgt"); }
    static std::string high_contrast() { return asset::plugin(pluginInstance, "res/rails/-light.vgt"); }
};

std::string rail_theme_name(RailThemeSetting choice) {
    switch (choice) {
        default:
        case RailThemeSetting::None: return "";
        case RailThemeSetting::Light: return "Light";
        case RailThemeSetting::Dark: return "Dark";
        case RailThemeSetting::HighContrast: return "High Contrast" ;
        case RailThemeSetting::FollowRackUi: return ThemeName(getActualTheme(ThemeSetting::FollowRackUi));
        case RailThemeSetting::FollowRackPreferDark: return ThemeName(getActualTheme(ThemeSetting::FollowRackPreferDark));
    }
}

std::shared_ptr<svg_theme::SvgTheme> get_rail_theme(const std::string& name) {
    return rail_theme_cache.getTheme(name);
}

void load_rail_themes() {
    using namespace svg_theme;
#ifdef DEV_BUILD
    ErrorContext err;
    ErrorContext* error_context = &err;
#else
    ErrorContext* error_context = nullptr;
#endif

    auto theme = loadSvgThemeFile(RailThemeData::light(), error_context);
#ifdef DEV_BUILD
    if (!theme) {
        auto report = error_context->makeErrorReport();
        WARN("%s", report.c_str());
    }
#endif
    if (theme) rail_theme_cache.addTheme(theme);

    theme = loadSvgThemeFile(RailThemeData::dark(), error_context);
#ifdef DEV_BUILD
    if (!theme) {
        auto report = error_context->makeErrorReport();
        WARN("%s", report.c_str());
    }
#endif
    if (theme) rail_theme_cache.addTheme(theme);

    theme = loadSvgThemeFile(RailThemeData::high_contrast(), error_context);
#ifdef DEV_BUILD
    if (!theme) {
        auto report = error_context->makeErrorReport();
        WARN("%s", report.c_str());
    }
#endif
    if (theme) rail_theme_cache.addTheme(theme);
}

RailThemeSetting parse_setting_shorthand(const std::string& text) {
    if (text.empty()) return RailThemeSetting::None;
    switch (text[0]) {
        case 'l': case 'L': return RailThemeSetting::Light;
        case 'd': case 'D': return RailThemeSetting::Dark;
        case 'h': case 'H': return RailThemeSetting::HighContrast;
        case 'r': case 'R': return RailThemeSetting::FollowRackUi;
        case 'f': case 'F': return RailThemeSetting::FollowRackPreferDark;
        case 'n': case 'N': return RailThemeSetting::None;
    }
    return RailThemeSetting::None;
}

RailThemeSetting RailThemeSetting_from_json(json_t* root) {
    return parse_setting_shorthand(get_json_string(root, "rail-theme", "n"));
}

void RailThemeSetting_to_json(json_t* root, RailThemeSetting choice) {
    const char * value{"f"};
    switch (choice) {
        case RailThemeSetting::Light: value = "light"; break;
        case RailThemeSetting::Dark: value = "dark"; break;
        case RailThemeSetting::HighContrast: value = "hc"; break;
        case RailThemeSetting::FollowRackUi: value = "rack-ui"; break;
        case RailThemeSetting::FollowRackPreferDark: value = "follow-dark"; break;
        case RailThemeSetting::None: value = "none"; break;
    }
    set_json(root, "rail-theme", value);
}

}