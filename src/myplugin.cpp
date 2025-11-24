#include "myplugin.hpp"
using namespace ::rack;
#include "services/svg-theme-load.hpp"
using namespace ::svg_theme;

Plugin* pluginInstance;

void init(Plugin* p) {
	pluginInstance = p;

	p->addModel(modelImagine);
	p->addModel(modelCopper);
	p->addModel(modelCopperMini);
	p->addModel(modelInfo);
	p->addModel(modelBlank);
	p->addModel(modelSkiff);
	p->addModel(modelFancyBox);
	p->addModel(modelRui);
	p->addModel(modelGuide);
    p->addModel(modelPanelTone);
}


std::string make_portable_path(std::string path) {
    path = system::getCanonical(path);

    auto prefix = pluginInstance->path;
    auto size = prefix.size();
    if (0 == prefix.compare(0, size, path.c_str(), 0, size)) {
        return path.replace(0, size, "{plug}");
    }
    prefix = asset::userDir;
    size = prefix.size();
    if (0 == prefix.compare(0, size, path.c_str(), 0, size)) {
        return path.replace(0, size, "{user}");
    }
    return path;
}

std::string path_from_portable_path(std::string path) {
    std::string prefix{"{plug}"};
    auto size = prefix.size();
    if (0 == prefix.compare(0, size, path, 0, size)) {
        path = path.replace(0, size, pluginInstance->path);
    } else {
        prefix = "{user}";
        if (0 == prefix.compare(0, size, path, 0, size)) {
            path = path.replace(0, size, asset::userDir);
        }
    }
    return system::getCanonical(path);
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

    //theme_cache.sort();
}
