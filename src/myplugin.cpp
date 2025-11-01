#include "myplugin.hpp"
using namespace ::rack;
#include "services/svg-theme-2-load.hpp"
using namespace ::svg_theme_2;

Plugin* pluginInstance;

void init(Plugin* p) {
	pluginInstance = p;

	// Add modules here
	p->addModel(modelImagine);
	p->addModel(modelCopper);
	p->addModel(modelCopperMini);
	p->addModel(modelInfo);
	p->addModel(modelBlank);
	p->addModel(modelSkiff);
	p->addModel(modelRui);

	// Any other plugin initialization may go here.
	// As an alternative, consider lazy-loading assets and lookup tables when your module is created to reduce startup times of Rack.
}

RackSvgCache rack_svg_cache;
RackSvgCache * getRackSvgs() { return &rack_svg_cache;}

SvgNoCache no_svg_cache;
SvgNoCache * getSvgNoCache() { return &no_svg_cache; }

ThemeCache theme_cache;

ThemeCache& getThemeCache() {
    initThemeCache();
    return theme_cache;
}

void reloadThemeCache() {
    theme_cache.clear();
    initThemeCache();
}

void initThemeCache() {
    if (!theme_cache.themes.empty()) return;
#ifdef DEV_BUILD
    ErrorContext err;
    ErrorContext* error_context = &err;
#else
    ErrorContext* error_context = nullptr;
#endif
    auto theme = loadSvgThemeFile(asset::plugin(pluginInstance, "res/themes/light.vgt"), error_context);
#ifdef DEV_BUILD
    if (!theme) {
        auto report = error_context->makeErrorReport();
        WARN("%s", report.c_str());
    }
#endif
    if (theme) theme_cache.addTheme(theme);

    theme = loadSvgThemeFile(asset::plugin(pluginInstance, "res/themes/dark.vgt"), error_context);
#ifdef DEV_BUILD
    if (!theme) {
        auto report = error_context->makeErrorReport();
        WARN("%s", report.c_str());
    }
#endif
    if (theme) theme_cache.addTheme(theme);

    theme = loadSvgThemeFile(asset::plugin(pluginInstance, "res/themes/high.vgt"), error_context);
#ifdef DEV_BUILD
    if (!theme) {
        auto report = error_context->makeErrorReport();
        WARN("%s", report.c_str());
    }
#endif
    if (theme) theme_cache.addTheme(theme);

    theme_cache.sort();
}
