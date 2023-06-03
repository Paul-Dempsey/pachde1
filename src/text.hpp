#pragma once
#include "plugin.hpp"

namespace pachde {

std::string format_string(const char *fmt, ...);

inline std::shared_ptr<window::Font> GetPluginFont(const char * path = NULL)
{
    return APP->window->loadFont(asset::plugin(pluginInstance, path ? path : "res/fonts/HankenGrotesk-SemiBold.ttf"));
}

inline bool FontOk(std::shared_ptr<window::Font> font) {
    return font && font->handle >= 0;
}

void SetTextStyle(NVGcontext *vg, std::shared_ptr<window::Font> font, NVGcolor color = GRAY20, float height = 16);

// Center text vertically and horizontally on the given point
// Text style must have been previously set
void CenterText(NVGcontext *vg, float x, float y, const char * text, const char * end);

enum class BaselineCorrection {
    None,
    Baseline
};

// The y coordinate is the baseline (BaselineCorrection::none) 
// or the bottom of text box (BaselineCorrection::Baseline).
// Text style must have been previously set.
void RightAlignText(NVGcontext *vg, float x, float y, const char * text, const char * end, BaselineCorrection correction = BaselineCorrection::None);

} // namespace pachde