#include <rack.hpp>
#include "colors.hpp"

namespace pachde {

float Hue1(const NVGcolor& color)
{
    auto r = color.r, g = color.g, b = color.b;

    auto M = std::max(std::max(r, g), b);
    auto m = std::min(std::min(r, g), b);
    auto C = M - m;

    float result;
    if (0.f == C) {
        result = 0.f;
    } else if (M == g) {
        result = (b - r)/C + 2.0f;
    } else if (M == b) {
        result = ((r - g)/C) + 4.0f;
    } else {
        result = fmodf((g - b)/C, 6.0f);
    }
    result = (result * 60.f)/360.f;
    //assert(result >= 0.f && result <= 1.0f);
    return result;
}


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
    json_t* j = json_object_get(root, "theme");
    return j ? ParseTheme(json_string_value(j)) : Theme::Unset;
}

NVGcolor PanelBackground(Theme theme)
{
    switch (theme) {
        default:
        case Theme::Unset:
        case Theme::Light: return GrayRamp[G_90];
        case Theme::Dark: return GrayRamp[G_20];
        case Theme::HighContrast: return GrayRamp[G_05];
    }
}

NVGcolor ThemeTextColor(Theme theme)
{
    switch (theme)
    {
    default:
    case Theme::Unset:
    case Theme::Light:
        return GrayRamp[G_20];
        break;

    case Theme::Dark:
        return GrayRamp[G_65];

    case Theme::HighContrast:
        return GrayRamp[G_90];
        break;
    };
}

NVGcolor OutputBackground(Theme theme)
{
    switch (theme) {
        default:
        case Theme::Unset:
        case Theme::Light:
            return GrayRamp[G_40];
        case Theme::Dark:
            return GrayRamp[G_10];
        case Theme::HighContrast:
            return GrayRamp[G_BLACK];
    }
}

NVGcolor LogoColor(Theme theme)
{
    switch (theme) {
        default: 
        case Theme::Unset:
        case Theme::Light:
            return GrayRamp[G_BLACK];
        case Theme::Dark:
            return GrayRamp[G_75];
        case Theme::HighContrast:
            return GrayRamp[G_95];
    }
}

const NVGcolor GrayRamp[] = {
    BLACK,
    GRAY05,GRAY10,GRAY15,GRAY18,GRAY20,
    GRAY25,GRAY30,GRAY35,GRAY40,
    GRAY45,GRAY50,GRAY55,GRAY60,
    GRAY65,GRAY70,GRAY75,GRAY80,
    GRAY85,GRAY90,GRAY95,WHITE
};

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

// for light/dark overlay use something like
// nvgRGBAf(0.9f,0.9f,0.9f,0.2f), nvgRGBAf(0.,0.,0.,0.3f)
void CircleGradient(NVGcontext * vg, float cx, float cy, float r, NVGcolor top, NVGcolor bottom)
{
    nvgBeginPath(vg);
    auto gradient = nvgLinearGradient(vg, cx, cy - r, cx, cy + r, top, bottom);
    nvgFillPaint(vg, gradient);
    nvgCircle(vg, cx, cy, r);
    nvgFill(vg);
}

// void CircleGradient(NVGcontext * vg, float cx, float cy, float r, float dy1, float dy2, NVGcolor top, NVGcolor bottom)
// {
//     nvgBeginPath(vg);
//     auto gradient = nvgLinearGradient(vg, cx, 0.f + dy1, cx, 2.f * r + dy2, top, bottom);
//     nvgFillPaint(vg, gradient);
//     nvgCircle(vg, cx, cy, r);
//     nvgFill(vg);
// }

void Circle(NVGcontext * vg, float cx, float cy, float r, NVGcolor fill)
{
    nvgBeginPath(vg);
    nvgFillColor(vg, fill);
    nvgCircle(vg, cx, cy, r);
    nvgFill(vg);
}

void OpenCircle(NVGcontext * vg, float cx, float cy, float r, NVGcolor stroke, float stroke_width)
{
    nvgBeginPath(vg);
    nvgStrokeColor(vg, stroke);
    nvgStrokeWidth(vg, stroke_width);
    nvgCircle(vg, cx, cy, r);
    nvgStroke(vg);
}

}