#include "../myplugin.hpp"
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
    const int len = 512;
    std::string s(len, '\0');
    va_list args;
    va_start(args, fmt);
    auto r = std::vsnprintf(&(*s.begin()), len, fmt, args);
    va_end(args);
    if (r < 0) return "??";
    s.resize(std::min(r, len));
    return s;
}

size_t format_buffer(char * buffer, size_t length, const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    auto r = std::vsnprintf(buffer, length, fmt, args);
    va_end(args);
    return r;
}

std::string rgba_string(PackedColor color) {
    uint8_t r = color & 0xff; color >>= 8;
    uint8_t g = color & 0xff; color >>= 8;
    uint8_t b = color & 0xff; color >>= 8;
    uint8_t a = color & 0xff;
    if (a) {
        return format_string("rgba(%d,%d,%d,%d)", r, g, b, a);
    } else {
        return format_string("rgb(%d,%d,%d)", r, g, b);
    }
}

std::string hsla_string(float hue, float saturation, float lightness, float alpha)
{
    char buffer[25];
     std::string result{"hsl"};
     int ihue = static_cast<int>(hue * 360.f);
     if (alpha < 1.0f) {
        result.append("a(");
        auto len = format_buffer(buffer, sizeof(buffer), "%d,", ihue);
        result.append(buffer, len);
        if (saturation > .01f) {
            len = format_buffer(buffer, sizeof(buffer), "%.2f%%,", saturation * 100.f);
            result.append(buffer, len);
        } else {
            result.append("0%");
        }
        if (lightness > .01f) {
            len = format_buffer(buffer, sizeof(buffer), "%.2f%%,", lightness * 100.f);
            result.append(buffer, len);
        } else {
            result.append("0%,");
        }
        if (alpha > .01f) {
            len = format_buffer(buffer, sizeof(buffer), "%.2f%%)", alpha * 100.f);
            result.append(buffer, len);
        } else {
            result.append("0%)");
        }
     } else {
        result.push_back('(');
        auto len = format_buffer(buffer, sizeof(buffer), "%d,", ihue);
        result.append(buffer, len);
        if (saturation > .01f) {
            len = format_buffer(buffer, sizeof(buffer), "%.2f%%,", saturation * 100.f);
            result.append(buffer, len);
        } else {
            result.append("0%,");
        }
        if (lightness > .01f) {
            len = format_buffer(buffer, sizeof(buffer), "%.2f%%)", lightness * 100.f);
            result.append(buffer, len);
        } else {
            result.append("0%)");
        }
     }
     return result;
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