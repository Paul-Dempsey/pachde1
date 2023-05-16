
#pragma once
#include <rack.hpp>

#define GRAY05 nvgRGB(0xd0,0xd0,0xd0)
#define GRAY10 nvgRGB(0x1a,0x1a,0x1a)
#define GRAY20 nvgRGB(0x32,0x32,0x32)
#define GRAY25 nvgRGB(0x40,0x40,0x40)
#define GRAY45 nvgRGB(0x73,0x73,0x73)
#define GRAY50 nvgRGB(0x80,0x80,0x80)
#define GRAY75 nvgRGB(0xc0,0xc0,0xc0)
#define GRAY80 nvgRGB(0xcc,0xcc,0xcc)
#define GRAY85 nvgRGB(0xd9,0xd9,0xd9)
#define GRAY90 nvgRGB(0xe5,0xe5,0xe5)
#define GRAY95 nvgRGB(0xf2,0xf2,0xf2)

#define COLOR_BRAND nvgRGB(0x45,0x7a,0xa6)
#define COLOR_BRAND_HI nvgRGB(0xbd,0xd6,0xfc)
#define COLOR_BRAND_LO nvgRGB(0x40,0x5a,0x80)
#define COLOR_MAGENTA nvgRGB(0xbf,0x60,0xbe)
#define COLOR_GREEN nvgRGB(0x54,0xa7,0x54)
#define COLOR_GREEN_LO nvgRGB(0x39,0x73,0x3a)
#define COLOR_GREEN_HI nvgRGB(0xbd,0xfc,0xbd)

#define IS_SAME_COLOR(p,q) (((p).a == (q).a) && ((p).r == (q).r) && ((p).g == (q).g) && ((p).b == (q).b))
inline NVGcolor Overlay(NVGcolor color) { return nvgTransRGBAf(color, 0.2f); }

enum class Theme {
    Unset,
    Light,
    Dark,
    HighContrast
};
const Theme DefaultTheme = Theme::Light;
inline Theme ConcreteTheme(Theme t) { return Theme::Unset == t ? DefaultTheme : t; }
inline NVGcolor PanelBackground(Theme t)
{
    switch (t) {
        default:
        case Theme::Unset:
        case Theme::Light: return GRAY90;
        case Theme::Dark: return GRAY20;
        case Theme::HighContrast: return GRAY05;
    }
}
inline NVGcolor OutputBackground(Theme t) {
    switch (t) {
        default:
        case Theme::Unset:
        case Theme::Light: return GRAY25;
        case Theme::Dark: return GRAY20;
        case Theme::HighContrast: return nvgRGB(0,0,0);
    }
}

std::string ToString(Theme t);
Theme ParseTheme(std::string text);
Theme ThemeFromJson(json_t * root);