#pragma once
#include "services/svg-theme.hpp"
#include "services/colors.hpp"
using namespace ::svg_theme;
using namespace pachde;

namespace widgetry {

struct ElementStyle
{
    const char* key;
    PackedColor fill_color;
    PackedColor stroke_color;
    float dx;

    ElementStyle(const char* key)
        : key(key), fill_color(0), stroke_color(0), dx(1.0f) {}

    ElementStyle(const char* key, const char * color_spec, float dx = 1.f);

    ElementStyle(const char* key, PackedColor co, float dx = 1.f)
        : key(key), fill_color(co), stroke_color(0), dx(dx) {}

    ElementStyle(const char* key, PackedColor co, PackedColor stroke, float dx = 1.f)
        : key(key), fill_color(co), stroke_color(stroke), dx(dx) {}

    ElementStyle(const char* key, PackedColor co, const char * stroke_spec, float dx = 1.f);

    ElementStyle(const char* key, const char * color_spec, const char * stroke_spec, float dx = 1.f);

    NVGcolor nvg_color() const { return fromPacked(fill_color); }
    NVGcolor nvg_stroke_color() const { return fromPacked(stroke_color); }
    float width() const { return dx; }

    void apply_theme(std::shared_ptr<SvgTheme> theme);
};

}