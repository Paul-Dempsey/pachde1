#pragma once
#include <rack.hpp>
#include "services/svg-query.hpp"
#include "services/svg-theme.hpp"

extern ::rack::Plugin* pluginInstance;

extern ::rack::Model* modelBlank;
extern ::rack::Model* modelInfo;
extern ::rack::Model* modelCopper;
extern ::rack::Model* modelCopperMini;
extern ::rack::Model* modelImagine;
extern ::rack::Model* modelSkiff;
extern ::rack::Model* modelFancyBox;
extern ::rack::Model* modelRui;
extern ::rack::Model* modelGuide;
extern ::rack::Model* modelPanelTone;

void initThemeCache();
void reloadThemeCache();
::svg_theme::ThemeCache& getThemeCache();
::svg_theme::RackSvgCache* getRackSvgs();
::svg_theme::SvgNoCache* getSvgNoCache();

inline std::string user_plugin_asset(const std::string& asset) {
    return system::join(asset::user(pluginInstance->slug.c_str()), asset);
}
// Make a fake path if under plugin or user folder, so that presets are portable
std::string make_portable_path(std::string path);
// Decode fake path as needed to make a real path
std::string path_from_portable_path(std::string path);
