#pragma once
#include <rack.hpp>
#include "services/svg-query.hpp"
#include "services/svg-theme-2.hpp"

extern ::rack::Plugin* pluginInstance;

extern ::rack::Model* modelBlank;
extern ::rack::Model* modelInfo;
extern ::rack::Model* modelCopper;
extern ::rack::Model* modelCopperMini;
extern ::rack::Model* modelImagine;
extern ::rack::Model* modelSkiff;
extern ::rack::Model* modelRui;
extern ::rack::Model* modelGuide;

void initThemeCache();
void reloadThemeCache();
::svg_theme_2::ThemeCache& getThemeCache();
::svg_theme_2::RackSvgCache* getRackSvgs();
::svg_theme_2::SvgNoCache* getSvgNoCache();