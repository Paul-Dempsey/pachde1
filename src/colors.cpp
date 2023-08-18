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


NamedColor stock_colors[] = {
	{ "#d blue", PackRGB(0x45, 0x7a, 0xa6) },
	{ "#d blue dark", PackRGB(0x40,0x5a,0x80) },
	{ "#d blue medium", PackRGB(0x4e, 0x8b, 0xbf) },
	{ "#d blue light", PackRGB(0xbd,0xd6,0xfc) },
	{ "#d default port", toPacked(nvgHSL(210.f/360.f, 0.5f, 0.65f)) },
	{ "Black", PackRGB( 0, 0, 0) },
    { "5% Gray", PackRGB(0x0d, 0x0d, 0x0d) },
    { "10% Gray", PackRGB(0x1a, 0x1a, 0x1a) },
    { "15% Gray", PackRGB(0x26, 0x26, 0x26) },
    { "20% Gray", PackRGB(0x33, 0x33, 0x33) },
    { "25% Gray", PackRGB(0x40, 0x40, 0x40) },
    { "30% Gray", PackRGB(0x4d, 0x4d, 0x4d) },
    { "35% Gray", PackRGB(0x59, 0x59, 0x59) },
    { "40% Gray", PackRGB(0x66, 0x66, 0x66) },
    { "45% Gray", PackRGB(0x73, 0x73, 0x73) },
    { "50% Gray", PackRGB(0x80, 0x80, 0x80) },
    { "55% Gray", PackRGB(0x8c, 0x8c, 0x8c) },
    { "60% Gray", PackRGB(0x99, 0x99, 0x99) },
    { "65% Gray", PackRGB(0xa6, 0xa6, 0xa6) },
    { "70% Gray", PackRGB(0xb2, 0xb2, 0xb2) },
    { "75% Gray", PackRGB(0xbf, 0xbf, 0xbf) },
    { "80% Gray", PackRGB(0xcc, 0xcc, 0xcc) },
    { "85% Gray", PackRGB(0xd9, 0xd9, 0xd9) },
    { "90% Gray", PackRGB(0xe5, 0xe5, 0xe5) },
    { "95% Gray", PackRGB(0xf2, 0xf2, 0xf2) },
	{ "White", PackRGB(255, 255, 255) },
    { "VCV Rack white", PackRGB(0xef, 0xef, 0xef) },
	{ "Red", PackRGB(255, 0, 0) },
	{ "Green", PackRGB( 0, 128, 0) },
	{ "Blue", PackRGB( 0, 0, 255) },
	{ "Yellow", PackRGB(255, 255, 0) },
	{ "Magenta", PackRGB(255, 0, 255) },
	{ "Cyan", PackRGB( 0, 255, 255) },
	{ "Alice blue", PackRGB(240, 248, 255) },
	{ "Antique white", PackRGB(250, 235, 215) },
	{ "Aqua", PackRGB( 0, 255, 255) },
	{ "Aquamarine (Medium)", PackRGB(102, 205, 170) },
	{ "Aquamarine", PackRGB(127, 255, 212) },
	{ "Azure", PackRGB(240, 255, 255) },
	{ "Beige", PackRGB(245, 245, 220) },
	{ "Bisque", PackRGB(255, 228, 196) },
	{ "Blanched almond", PackRGB(255, 235, 205) },
	{ "Blue (Dark)", PackRGB( 0, 0, 139) },
	{ "Blue (Light)", PackRGB(173, 216, 230) },
	{ "Blue (Medium)", PackRGB( 0, 0, 205) },
	{ "Blue violet", PackRGB(138, 43, 226) },
	{ "Brown", PackRGB(165, 42, 42) },
	{ "Burlywood", PackRGB(222, 184, 135) },
	{ "Cadet blue", PackRGB( 95, 158, 160) },
	{ "Chartreuse", PackRGB(127, 255, 0) },
	{ "Chocolate", PackRGB(210, 105, 30) },
	{ "Coral (Light)", PackRGB(240, 128, 128) },
	{ "Coral", PackRGB(255, 127, 80) },
	{ "Cornflower blue", PackRGB(100, 149, 237) },
	{ "Cornsilk", PackRGB(255, 248, 220) },
	{ "Crimson", PackRGB(220, 20, 60) },
	{ "Cyan (Dark)", PackRGB( 0, 139, 139) },
	{ "Cyan (Light)", PackRGB(224, 255, 255) },
	{ "Deep pink", PackRGB(255, 20, 147) },
	{ "Deep sky blue", PackRGB( 0, 191, 255) },
	{ "Dim gray", PackRGB(105, 105, 105) },
	{ "Dodger blue", PackRGB( 30, 144, 255) },
	{ "Firebrick", PackRGB(178, 34, 34) },
	{ "Floral white", PackRGB(255, 250, 240) },
	{ "Forest green", PackRGB( 34, 139, 34) },
	{ "Fuchsia", PackRGB(255, 0, 255) },
	{ "Gainsboro", PackRGB(220, 220, 220) },
	{ "Ghost white", PackRGB(248, 248, 255) },
	{ "Gold", PackRGB(255, 215, 0) },
	{ "Goldenrod (Dark)", PackRGB(184, 134, 11) },
	{ "Goldenrod (Light)", PackRGB(250, 250, 210) },
	{ "Goldenrod (Pale)", PackRGB(238, 232, 170) },
	{ "Goldenrod", PackRGB(218, 165, 32) },
	{ "Gray (Dark)", PackRGB(169, 169, 169) },
	{ "Gray (Light)", PackRGB(211, 211, 211) },
	{ "Green (Dark)", PackRGB( 0, 100, 0) },
	{ "Green (Light)", PackRGB(144, 238, 144) },
	{ "Green (Pale)", PackRGB(152, 251, 152) },
	{ "Green yellow", PackRGB(173, 255, 47) },
	{ "Honeydew", PackRGB(240, 255, 240) },
	{ "Hot pink", PackRGB(255, 105, 180) },
	{ "Indian red", PackRGB(205, 92, 92) },
	{ "Indigo", PackRGB( 75, 0, 130) },
	{ "Ivory", PackRGB(255, 255, 240) },
	{ "Khaki (Dark)", PackRGB(189, 183, 107) },
	{ "Khaki", PackRGB(240, 230, 140) },
	{ "Lavender blush", PackRGB(255, 240, 245) },
	{ "Lavender", PackRGB(230, 230, 250) },
	{ "Lawngreen", PackRGB(124, 252, 0) },
	{ "Lemon chiffon", PackRGB(255, 250, 205) },
	{ "Lime green", PackRGB( 50, 205, 50) },
	{ "Lime", PackRGB( 0, 255, 0) },
	{ "Linen", PackRGB(250, 240, 230) },
	{ "Magenta (Dark)", PackRGB(139, 0, 139) },
	{ "Maroon", PackRGB(128, 0, 0) },
	{ "Midnight blue", PackRGB( 25, 25, 112) },
	{ "Mint cream", PackRGB(245, 255, 250) },
	{ "Misty rose", PackRGB(255, 228, 225) },
	{ "Moccasin", PackRGB(255, 228, 181) },
	{ "Navajo white", PackRGB(255, 222, 173) },
	{ "Navy", PackRGB( 0, 0, 128) },
	{ "Old lace", PackRGB(253, 245, 230) },
	{ "Olive drab", PackRGB(107, 142, 35) },
	{ "Olive green (Dark)", PackRGB( 85, 107, 47) },
	{ "Olive", PackRGB(128, 128, 0) },
	{ "Orange (Dark)", PackRGB(255, 140, 0) },
	{ "Orange red", PackRGB(255, 69, 0) },
	{ "Orange", PackRGB(255, 165, 0) },
	{ "Orchid (Dark)", PackRGB(153, 50, 204) },
	{ "Orchid (Medium)", PackRGB(186, 85, 211) },
	{ "Orchid", PackRGB(218, 112, 214) },
	{ "Papaya whip", PackRGB(255, 239, 213) },
	{ "Peach puff", PackRGB(255, 218, 185) },
	{ "Peru", PackRGB(205, 133, 63) },
	{ "Pink (Light)", PackRGB(255, 182, 193) },
	{ "Pink", PackRGB(255, 192, 203) },
	{ "Plum", PackRGB(221, 160, 221) },
	{ "Powder blue", PackRGB(176, 224, 230) },
	{ "Purple (Medium)", PackRGB(147, 112, 219) },
	{ "Purple", PackRGB(128, 0, 128) },
	{ "Red (Dark)", PackRGB(139, 0, 0) },
	{ "Rosy brown", PackRGB(188, 143, 143) },
	{ "Royal blue", PackRGB( 65, 105, 225) },
	{ "Saddle brown", PackRGB(139, 69, 19) },
	{ "Salmon (Dark)", PackRGB(233, 150, 122) },
	{ "Salmon (Light)", PackRGB(255, 160, 122) },
	{ "Salmon", PackRGB(250, 128, 114) },
	{ "Sandy brown", PackRGB(244, 164, 96) },
	{ "Sea green (Dark)", PackRGB( 32, 178, 170) },
	{ "Sea green (Light)", PackRGB(143, 188, 143) },
	{ "Sea green (Medium)", PackRGB( 60, 179, 113) },
	{ "Sea green", PackRGB( 46, 139, 87) },
	{ "Seashell", PackRGB(255, 245, 238) },
	{ "Sienna", PackRGB(160, 82, 45) },
	{ "Silver", PackRGB(192, 192, 192) },
	{ "Sky blue (Light)", PackRGB(135, 206, 250) },
	{ "Sky blue", PackRGB(135, 206, 235) },
	{ "Slate blue (Dark)", PackRGB( 72, 61, 139) },
	{ "Slate blue (Medium)", PackRGB(123, 104, 238) },
	{ "Slate blue", PackRGB(106, 90, 205) },
	{ "Slate gray (Dark)", PackRGB( 47, 79, 79) },
	{ "Slate gray (Light)", PackRGB(119, 136, 153) },
	{ "Slate gray", PackRGB(112, 128, 144) },
	{ "Snow", PackRGB(255, 250, 250) },
	{ "Spring green (Medium)", PackRGB( 0, 250, 154) },
	{ "Spring green", PackRGB( 0, 255, 127) },
	{ "Steel blue (Light)", PackRGB(176, 196, 222) },
	{ "Steel blue", PackRGB( 70, 130, 180) },
	{ "Tan", PackRGB(210, 180, 140) },
	{ "Teal", PackRGB( 0, 128, 128) },
	{ "Thistle", PackRGB(216, 191, 216) },
	{ "Tomato", PackRGB(255, 99, 71) },
	{ "Turquoise (Dark)", PackRGB( 0, 206, 209) },
	{ "Turquoise (Medium)", PackRGB( 72, 209, 204) },
	{ "Turquoise (Pale)", PackRGB(175, 238, 238) },
	{ "Turquoise", PackRGB( 64, 224, 208) },
	{ "Violet (Dark)", PackRGB(148, 0, 211) },
	{ "Violet red (Medium)", PackRGB(199, 21, 133) },
	{ "Violet red (Pale)", PackRGB(219, 112, 147) },
	{ "Violet", PackRGB(238, 130, 238) },
	{ "Wheat", PackRGB(245, 222, 179) },
	{ "White smoke", PackRGB(245, 245, 245) },
	{ "Yellow (Light)", PackRGB(255, 255, 224) },
	{ "Yellow green", PackRGB(154, 205, 50) },
    { nullptr, 0 }
};

Theme ParseTheme(std::string text) {
    if (text.empty()) return DefaultTheme;
    switch (text[0]) {
        case 'l': case 'L': return Theme::Light;
        case 'd': case 'D': return Theme::Dark;
        case 'h': case 'H': return Theme::HighContrast;
    }
    return DefaultTheme;
}

std::string ToString(Theme t) {
    switch (t) {
        default:
        case Theme::Light: return "light";
        case Theme::Dark: return "dark";
        case Theme::HighContrast: return "highcontrast";
    }
}

Theme ThemeFromJson(json_t * root) {
    json_t* j = json_object_get(root, "theme");
    return j ? ParseTheme(json_string_value(j)) : DefaultTheme;
}
Theme DarkThemeFromJson(json_t * root) {
    json_t* j = json_object_get(root, "dark-theme");
    return j ? ParseTheme(json_string_value(j)) : Theme::Dark;
}

NVGcolor PanelBackground(Theme theme)
{
    switch (theme) {
        default:
        case Theme::Unset:
        case Theme::Light: return GrayRamp[G_90];
        case Theme::Dark: return GrayRamp[G_10];
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