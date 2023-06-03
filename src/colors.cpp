#include <rack.hpp>
#include "colors.hpp"

namespace pachde {

Theme ParseTheme(std::string text) {
    if (text.empty()) return Theme::Unset;
    switch (text[0]) {
        case 'l': case 'L': return Theme::Light;
        case 'd': case 'D': return Theme::Dark;
        case 'h': case 'H': return Theme::HighContrast;
    }
    return Theme::Unset;
}

std::string ToString(Theme t) {
    switch (t) {
        case Theme::Light: return "light";
        case Theme::Dark: return "dark";
        case Theme::HighContrast: return "highcontrast";
        default: return "";
    }
}

Theme ThemeFromJson(json_t * root) {
    json_t* themeJ = json_object_get(root, "theme");
    return themeJ ? ParseTheme(json_string_value(themeJ)) : Theme::Unset;
}

void FillRect(NVGcontext *vg, float x, float y, float width, float height, NVGcolor color)
{
    nvgBeginPath(vg);
    nvgRect(vg, x, y, width, height);
    nvgFillColor(vg, color);
    nvgFill(vg);
}

void RoundRect(NVGcontext *vg, float x, float y, float width, float height, NVGcolor color, float radius)
{
    nvgBeginPath(vg);
    nvgRoundedRect(vg, x, y, width, height, radius);
    nvgFillColor(vg, color);
    nvgFill(vg);
}

void BoxRect(NVGcontext *vg, float x, float y, float width, float height, NVGcolor color, float strokeWidth)
{
    nvgBeginPath(vg);
    nvgRect(vg, x, y, width, height);
    nvgStrokeColor(vg, color);
    nvgStrokeWidth(vg, strokeWidth);
    nvgStroke(vg);
}

void RoundBoxRect(NVGcontext *vg, float x, float y, float width, float height, NVGcolor color, float radius, float strokeWidth)
{
    nvgBeginPath(vg);
    nvgRoundedRect(vg, x, y, width, height, radius);
    nvgStrokeColor(vg, color);
    nvgStrokeWidth(vg, strokeWidth);
    nvgStroke(vg);
}

void Line(NVGcontext * vg, float x1, float y1, float x2, float y2, NVGcolor color, float strokeWidth)
{
    nvgBeginPath(vg);
    nvgMoveTo(vg, x1, y1);
    nvgLineTo(vg, x2, y2);
    nvgStrokeColor(vg, color);
    nvgStrokeWidth(vg, strokeWidth);
    nvgStroke(vg);
}

}