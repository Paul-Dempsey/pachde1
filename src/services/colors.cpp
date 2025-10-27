#include <rack.hpp>
#include "colors.hpp"
#include "theme.hpp"

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

PackedColor packed_gray_ramp[] = {
    colors::G0, colors::G5, colors::G10, colors::G15, colors::G18,
    colors::G20, colors::G25,
    colors::G30, colors::G35,
    colors::G40, colors::G45,
    colors::G50, colors::G55,
    colors::G60, colors::G65,
    colors::G70, colors::G75,
    colors::G80, colors::G85,
    colors::G90, colors::G95,
    colors::G100
};

NVGcolor RampGray(Ramp g) {
    return fromPacked(packed_gray_ramp[rack::math::clamp(g, G_BLACK, G_WHITE)]);
}

NamedColor stock_colors[] = {
    { "#d blue", packRgb(0x45, 0x7a, 0xa6) },
    { "#d blue dark", packRgb(0x40,0x5a,0x80) },
    { "#d blue medium", packRgb(0x4e, 0x8b, 0xbf) },
    { "#d blue light", packRgb(0xbd,0xd6,0xfc) },
    { "#d default port", toPacked(nvgHSL(210.f/360.f, 0.5f, 0.65f)) },
    { "Black", packRgb( 0, 0, 0) },
    { "5% Gray", packRgb(0x0d, 0x0d, 0x0d) },
    { "10% Gray", packRgb(0x1a, 0x1a, 0x1a) },
    { "15% Gray", packRgb(0x26, 0x26, 0x26) },
    { "20% Gray", packRgb(0x33, 0x33, 0x33) },
    { "25% Gray", packRgb(0x40, 0x40, 0x40) },
    { "30% Gray", packRgb(0x4d, 0x4d, 0x4d) },
    { "35% Gray", packRgb(0x59, 0x59, 0x59) },
    { "40% Gray", packRgb(0x66, 0x66, 0x66) },
    { "45% Gray", packRgb(0x73, 0x73, 0x73) },
    { "50% Gray", packRgb(0x80, 0x80, 0x80) },
    { "55% Gray", packRgb(0x8c, 0x8c, 0x8c) },
    { "60% Gray", packRgb(0x99, 0x99, 0x99) },
    { "65% Gray", packRgb(0xa6, 0xa6, 0xa6) },
    { "70% Gray", packRgb(0xb2, 0xb2, 0xb2) },
    { "75% Gray", packRgb(0xbf, 0xbf, 0xbf) },
    { "80% Gray", packRgb(0xcc, 0xcc, 0xcc) },
    { "85% Gray", packRgb(0xd9, 0xd9, 0xd9) },
    { "90% Gray", packRgb(0xe5, 0xe5, 0xe5) },
    { "95% Gray", packRgb(0xf2, 0xf2, 0xf2) },
    { "White", packRgb(255, 255, 255) },
    { "VCV Rack white", packRgb(0xef, 0xef, 0xef) },
    { "Red", packRgb(255, 0, 0) },
    { "Green", packRgb( 0, 128, 0) },
    { "Blue", packRgb( 0, 0, 255) },
    { "Yellow", packRgb(255, 255, 0) },
    { "Magenta", packRgb(255, 0, 255) },
    { "Cyan", packRgb( 0, 255, 255) },
    { "Alice blue", packRgb(240, 248, 255) },
    { "Antique white", packRgb(250, 235, 215) },
    { "Aqua", packRgb( 0, 255, 255) },
    { "Aquamarine (Medium)", packRgb(102, 205, 170) },
    { "Aquamarine", packRgb(127, 255, 212) },
    { "Azure", packRgb(240, 255, 255) },
    { "Beige", packRgb(245, 245, 220) },
    { "Bisque", packRgb(255, 228, 196) },
    { "Blanched almond", packRgb(255, 235, 205) },
    { "Blue (Dark)", packRgb( 0, 0, 139) },
    { "Blue (Light)", packRgb(173, 216, 230) },
    { "Blue (Medium)", packRgb( 0, 0, 205) },
    { "Blue violet", packRgb(138, 43, 226) },
    { "Brown", packRgb(165, 42, 42) },
    { "Burlywood", packRgb(222, 184, 135) },
    { "Cadet blue", packRgb( 95, 158, 160) },
    { "Chartreuse", packRgb(127, 255, 0) },
    { "Chocolate", packRgb(210, 105, 30) },
    { "Coral (Light)", packRgb(240, 128, 128) },
    { "Coral", packRgb(255, 127, 80) },
    { "Cornflower blue", packRgb(100, 149, 237) },
    { "Cornsilk", packRgb(255, 248, 220) },
    { "Crimson", packRgb(220, 20, 60) },
    { "Cyan (Dark)", packRgb( 0, 139, 139) },
    { "Cyan (Light)", packRgb(224, 255, 255) },
    { "Deep pink", packRgb(255, 20, 147) },
    { "Deep sky blue", packRgb( 0, 191, 255) },
    { "Dim gray", packRgb(105, 105, 105) },
    { "Dodger blue", packRgb( 30, 144, 255) },
    { "Firebrick", packRgb(178, 34, 34) },
    { "Floral white", packRgb(255, 250, 240) },
    { "Forest green", packRgb( 34, 139, 34) },
    { "Fuchsia", packRgb(255, 0, 255) },
    { "Gainsboro", packRgb(220, 220, 220) },
    { "Ghost white", packRgb(248, 248, 255) },
    { "Gold", packRgb(255, 215, 0) },
    { "Goldenrod (Dark)", packRgb(184, 134, 11) },
    { "Goldenrod (Light)", packRgb(250, 250, 210) },
    { "Goldenrod (Pale)", packRgb(238, 232, 170) },
    { "Goldenrod", packRgb(218, 165, 32) },
    { "Gray (Dark)", packRgb(169, 169, 169) },
    { "Gray (Light)", packRgb(211, 211, 211) },
    { "Green (Dark)", packRgb( 0, 100, 0) },
    { "Green (Light)", packRgb(144, 238, 144) },
    { "Green (Pale)", packRgb(152, 251, 152) },
    { "Green yellow", packRgb(173, 255, 47) },
    { "Honeydew", packRgb(240, 255, 240) },
    { "Hot pink", packRgb(255, 105, 180) },
    { "Indian red", packRgb(205, 92, 92) },
    { "Indigo", packRgb( 75, 0, 130) },
    { "Ivory", packRgb(255, 255, 240) },
    { "Khaki (Dark)", packRgb(189, 183, 107) },
    { "Khaki", packRgb(240, 230, 140) },
    { "Lavender blush", packRgb(255, 240, 245) },
    { "Lavender", packRgb(230, 230, 250) },
    { "Lawngreen", packRgb(124, 252, 0) },
    { "Lemon chiffon", packRgb(255, 250, 205) },
    { "Lime green", packRgb( 50, 205, 50) },
    { "Lime", packRgb( 0, 255, 0) },
    { "Linen", packRgb(250, 240, 230) },
    { "Magenta (Dark)", packRgb(139, 0, 139) },
    { "Maroon", packRgb(128, 0, 0) },
    { "Midnight blue", packRgb( 25, 25, 112) },
    { "Mint cream", packRgb(245, 255, 250) },
    { "Misty rose", packRgb(255, 228, 225) },
    { "Moccasin", packRgb(255, 228, 181) },
    { "Navajo white", packRgb(255, 222, 173) },
    { "Navy", packRgb( 0, 0, 128) },
    { "Old lace", packRgb(253, 245, 230) },
    { "Olive drab", packRgb(107, 142, 35) },
    { "Olive green (Dark)", packRgb( 85, 107, 47) },
    { "Olive", packRgb(128, 128, 0) },
    { "Orange (Dark)", packRgb(255, 140, 0) },
    { "Orange red", packRgb(255, 69, 0) },
    { "Orange", packRgb(255, 165, 0) },
    { "Orchid (Dark)", packRgb(153, 50, 204) },
    { "Orchid (Medium)", packRgb(186, 85, 211) },
    { "Orchid", packRgb(218, 112, 214) },
    { "Papaya whip", packRgb(255, 239, 213) },
    { "Peach puff", packRgb(255, 218, 185) },
    { "Peru", packRgb(205, 133, 63) },
    { "Pink (Light)", packRgb(255, 182, 193) },
    { "Pink", packRgb(255, 192, 203) },
    { "Plum", packRgb(221, 160, 221) },
    { "Powder blue", packRgb(176, 224, 230) },
    { "Purple (Medium)", packRgb(147, 112, 219) },
    { "Purple", packRgb(128, 0, 128) },
    { "Red (Dark)", packRgb(139, 0, 0) },
    { "Rosy brown", packRgb(188, 143, 143) },
    { "Royal blue", packRgb( 65, 105, 225) },
    { "Saddle brown", packRgb(139, 69, 19) },
    { "Salmon (Dark)", packRgb(233, 150, 122) },
    { "Salmon (Light)", packRgb(255, 160, 122) },
    { "Salmon", packRgb(250, 128, 114) },
    { "Sandy brown", packRgb(244, 164, 96) },
    { "Sea green (Dark)", packRgb( 32, 178, 170) },
    { "Sea green (Light)", packRgb(143, 188, 143) },
    { "Sea green (Medium)", packRgb( 60, 179, 113) },
    { "Sea green", packRgb( 46, 139, 87) },
    { "Seashell", packRgb(255, 245, 238) },
    { "Sienna", packRgb(160, 82, 45) },
    { "Silver", packRgb(192, 192, 192) },
    { "Sky blue (Light)", packRgb(135, 206, 250) },
    { "Sky blue", packRgb(135, 206, 235) },
    { "Slate blue (Dark)", packRgb( 72, 61, 139) },
    { "Slate blue (Medium)", packRgb(123, 104, 238) },
    { "Slate blue", packRgb(106, 90, 205) },
    { "Slate gray (Dark)", packRgb( 47, 79, 79) },
    { "Slate gray (Light)", packRgb(119, 136, 153) },
    { "Slate gray", packRgb(112, 128, 144) },
    { "Snow", packRgb(255, 250, 250) },
    { "Spring green (Medium)", packRgb( 0, 250, 154) },
    { "Spring green", packRgb( 0, 255, 127) },
    { "Steel blue (Light)", packRgb(176, 196, 222) },
    { "Steel blue", packRgb( 70, 130, 180) },
    { "Tan", packRgb(210, 180, 140) },
    { "Teal", packRgb( 0, 128, 128) },
    { "Thistle", packRgb(216, 191, 216) },
    { "Tomato", packRgb(255, 99, 71) },
    { "Turquoise (Dark)", packRgb( 0, 206, 209) },
    { "Turquoise (Medium)", packRgb( 72, 209, 204) },
    { "Turquoise (Pale)", packRgb(175, 238, 238) },
    { "Turquoise", packRgb( 64, 224, 208) },
    { "Violet (Dark)", packRgb(148, 0, 211) },
    { "Violet red (Medium)", packRgb(199, 21, 133) },
    { "Violet red (Pale)", packRgb(219, 112, 147) },
    { "Violet", packRgb(238, 130, 238) },
    { "Wheat", packRgb(245, 222, 179) },
    { "White smoke", packRgb(245, 245, 245) },
    { "Yellow (Light)", packRgb(255, 255, 224) },
    { "Yellow green", packRgb(154, 205, 50) },
    { nullptr, 0 }
};

NVGcolor PanelBackground(Theme theme)
{
    using namespace colors;
    switch (theme) {
    default:
    case Theme::Unset:
    case Theme::Light: return fromPacked(G90);
    case Theme::Dark: return fromPacked(G10);
    case Theme::HighContrast: return fromPacked(G0);
    }
}

NVGcolor ThemeTextColor(Theme theme)
{
    using namespace colors;
    switch (theme)
    {
    default:
    case Theme::Unset:
    case Theme::Light: return fromPacked(G20);
    case Theme::Dark: return fromPacked(G65);
    case Theme::HighContrast: return fromPacked(G90);
    };
}

NVGcolor OutputBackground(Theme theme)
{
    using namespace colors;
    switch (theme) {
    default:
    case Theme::Unset:
    case Theme::Light: return fromPacked(G40);
    case Theme::Dark: return fromPacked(G10);
    case Theme::HighContrast: return fromPacked(G0);
    }
}

NVGcolor LogoColor(Theme theme)
{
    using namespace colors;
    switch (theme) {
    default:
    case Theme::Unset:
    case Theme::Light: return fromPacked(G0);
    case Theme::Dark: return fromPacked(G75);
    case Theme::HighContrast: return fromPacked(G95);
    }
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

void FittedBoxRect(NVGcontext *vg, float x, float y, float width, float height, const NVGcolor& color, Fit fit, float strokeWidth)
{
    auto half_stroke = strokeWidth*.5;
    switch (fit) {
    case Fit::Inside:
        x += half_stroke;
        y += half_stroke;
        width -= strokeWidth;
        height -= strokeWidth;
        break;
    case Fit::Outside:
        x -= half_stroke;
        y -= half_stroke;
        width += strokeWidth;
        height += strokeWidth;
        break;
    }
    BoxRect(vg, x,y,width, height, color, strokeWidth);
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