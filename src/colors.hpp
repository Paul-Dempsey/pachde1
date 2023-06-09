#pragma once
#include <rack.hpp>

namespace pachde {

#define BLACK  nvgRGB(0,0,0)
#define GRAY05 nvgRGB(0x0d,0x0d,0x0d)  // #0d0d0d
#define GRAY10 nvgRGB(0x1a,0x1a,0x1a)  // #1a1a1a
#define GRAY20 nvgRGB(0x32,0x32,0x32)  // #323232
#define GRAY25 nvgRGB(0x40,0x40,0x40)  // #404040
#define GRAY45 nvgRGB(0x73,0x73,0x73)  // #737373
#define GRAY50 nvgRGB(0x80,0x80,0x80)  // #808080
#define GRAY65 nvgRGB(0xa6,0xa6,0xa6)  // #a6a6a6
#define GRAY75 nvgRGB(0xc0,0xc0,0xc0)  // #c0c0c0
#define GRAY80 nvgRGB(0xcc,0xcc,0xcc)  // #cccccc
#define GRAY85 nvgRGB(0xd9,0xd9,0xd9)  // #d9d9d9
#define GRAY90 nvgRGB(0xe5,0xe5,0xe5)  // #e5e5e5
#define GRAY95 nvgRGB(0xf2,0xf2,0xf2)  // #f2f2f2

#define COLOR_BRAND    nvgRGB(0x45,0x7a,0xa6)  // #457aa6
#define COLOR_BRAND_MD nvgRGB(0x4e,0x8b,0xbf)  // #4e8dbf
#define COLOR_BRAND_HI nvgRGB(0xbd,0xd6,0xfc)  // #bdd6fc
#define COLOR_BRAND_LO nvgRGB(0x40,0x5a,0x80)  // #405a80
#define COLOR_MAGENTA  nvgRGB(0xbf,0x60,0xbe)  // #bf60be
#define COLOR_GREEN    nvgRGB(0x54,0xa7,0x54)  // #54a754
#define COLOR_GREEN_LO nvgRGB(0x39,0x73,0x3a)  // #39733a
#define COLOR_GREEN_HI nvgRGB(0xbd,0xfc,0xbd)  // #bdfcbd

#define IS_SAME_COLOR(p,q) (((p).a == (q).a) && ((p).r == (q).r) && ((p).g == (q).g) && ((p).b == (q).b))
inline NVGcolor Overlay(NVGcolor color) { return nvgTransRGBAf(color, 0.2f); }

enum class Theme {
    Unset = 0,
    Light = 1,
    Dark = 2,
    HighContrast = 3
};
inline bool IsDarker(Theme theme) { return theme > Theme::Light; }
inline bool IsLighter(Theme theme) { return theme <= Theme::Light; }

const Theme DefaultTheme = Theme::Light;
inline Theme ConcreteTheme(Theme theme) { return Theme::Unset == theme ? DefaultTheme : theme; }
inline NVGcolor PanelBackground(Theme theme)
{
    switch (theme) {
        default:
        case Theme::Unset:
        case Theme::Light: return GRAY90;
        case Theme::Dark: return GRAY20;
        case Theme::HighContrast: return GRAY05;
    }
}
inline NVGcolor ThemeTextColor(Theme theme) {
        switch (theme)
        {
        default:
        case Theme::Unset:
        case Theme::Light:
            return GRAY20;
            break;

        case Theme::Dark:
            return GRAY85;

        case Theme::HighContrast:
            return GRAY95;
            break;
        };
}
inline NVGcolor OutputBackground(Theme theme) {
    switch (theme) {
        default:
        case Theme::Unset:
        case Theme::Light:
        case Theme::Dark:
            return GRAY25;
        case Theme::HighContrast:
            return GRAY10;
    }
}

//https://stackoverflow.com/questions/596216/formula-to-determine-perceived-brightness-of-rgb-color

// inline float sRGBToLinear(float c) {
//     //rack::simd::pow
//     return c <= 0.04045 ? c / 12.92 : std::pow(((c + 0.055f)/1.055f), 2.4f);
// }

// inline float Luminance (NVGcolor color) {
//     return 0.2126f * sRGBToLinear(color.r) + 0.7152f * sRGBToLinear(color.g) + 0.0722f * sRGBToLinear(color.b);
// }

constexpr const float PI = 3.14159265358979323846;
constexpr const float TWO_PI = 2.0f * PI;
constexpr const float SQRT3 = 1.732050807568877f; 

// stb linearizes on load so we don';'t have to gamma/correct
inline float LuminanceLinear(NVGcolor color) {
    return 0.2126f * color.r + 0.7152f * color.g + 0.0722f * color.b;
}

inline float Saturation(NVGcolor color) {
    return std::max(std::max(color.r, color.g), color.b) - std::min(std::min(color.r, color.g), color.b);
}

inline float Hue(NVGcolor color) {
    return (rack::simd::atan2(SQRT3*(color.g - color.b), 2*color.r -color.g - color.b)
        + PI) / TWO_PI;
}

std::string ToString(Theme theme);
Theme ParseTheme(std::string text);
Theme ThemeFromJson(json_t * root);

void FillRect(NVGcontext *vg, float x, float y, float width, float height, NVGcolor color);
void RoundRect(NVGcontext *vg, float x, float y, float width, float height, NVGcolor color, float radius);
void BoxRect(NVGcontext *vg, float x, float y, float width, float height, NVGcolor color, float strokeWidth = 1.0);
void RoundBoxRect(NVGcontext *vg, float x, float y, float width, float height, NVGcolor color, float radius, float strokeWidth = 1.0);
void Line(NVGcontext * vg, float x1, float y1, float x2, float y2, NVGcolor color, float strokeWidth = 1.0);

}
