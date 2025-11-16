#pragma once
#include <rack.hpp>
#include "services/theme.hpp"
#include "services/svg-theme.hpp"

namespace pachde {

extern const std::set<std::string> known_rails;

inline bool known_rail(const char * name) {
    return known_rails.find(name) != known_rails.cend();
}

enum RailThemeSetting { // ThemeSetting + None
    Light,
    Dark,
    HighContrast,
    FollowRackUi,
    FollowRackPreferDark,
    None
};

std::shared_ptr<svg_theme::SvgTheme> get_rail_theme(const std::string& name);
std::string rail_theme_name(RailThemeSetting choice);
void load_rail_themes();

RailThemeSetting RailThemeSetting_from_json(json_t* root);
void RailThemeSetting_to_json(json_t* root, RailThemeSetting choice);

}