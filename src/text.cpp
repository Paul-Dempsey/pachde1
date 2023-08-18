#include "plugin.hpp"
#include "text.hpp"

namespace pachde {

std::shared_ptr<window::Font> GetPluginFontSemiBold(const char * path)
{
    return APP->window->loadFont(asset::plugin(pluginInstance, path ? path : "res/fonts/HankenGrotesk-SemiBold.ttf"));
}
std::shared_ptr<window::Font> GetPluginFontRegular(const char * path)
{
    return APP->window->loadFont(asset::plugin(pluginInstance, path ? path : "res/fonts/HankenGrotesk-Regular.ttf"));
}

std::string format_string(const char *fmt, ...)
{
    const int len = 256;
    std::string s(len, '\0');
    va_list args;
    va_start(args, fmt);
    auto r = std::vsnprintf(&(*s.begin()), len + 1, fmt, args);
    va_end(args);
    return r < 0 ? "??" : s;
}

void SetTextStyle(NVGcontext *vg, std::shared_ptr<window::Font> font, NVGcolor color, float height)
{
    assert(FontOk(font));
    nvgFillColor(vg, color);
    nvgFontFaceId(vg, font->handle);
    nvgTextLetterSpacing(vg, 0.f);
    nvgFontSize(vg, height);
}

void CenterText(NVGcontext *vg, float x, float y, const char * text, const char * end)
{
    // nvg offers a variety of text alignment options
    nvgTextAlign(vg, NVG_ALIGN_CENTER);
    nvgText(vg, x, y, text, end);
}

void RightAlignText(NVGcontext *vg, float x, float y, const char * text, const char * end, BaselineCorrection correction)
{
    float bounds[4] = { 0, 0, 0, 0 };
    nvgTextBounds(vg, 0, 0, text, end, bounds);
    auto descent = correction == BaselineCorrection::Baseline ? bounds[3] : 0.;
    nvgText(vg, x - bounds[2], y - descent, text, end);
}

}