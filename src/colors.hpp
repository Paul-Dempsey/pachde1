
#pragma once
#include <rack.hpp>

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
#define COLOR_BRAND_LO nvgRBG(0x40,0x5a,0x80)

#define IS_SAME_COLOR(p,q) (((p).a == (q).a) && ((p).r == (q).r) && ((p).g == (q).g) && ((p).b == (q).b))

enum class Theme {
    Unset,
    Light,
    Dark,
    HighContrast
};

inline Theme ConcreteTheme(Theme t) { return Theme::Unset == t ? Theme::Light : t; }
std::string ToString(Theme t);
Theme ParseTheme(std::string text);
Theme ThemeFromJson(json_t * root);