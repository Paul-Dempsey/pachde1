#include "element-style.hpp"

namespace widgetry {

PackedColor parse_color(const char * color_spec)
{
    PackedColor result;
    parseColor(result, colors::NoColor, color_spec, nullptr);
    return result;
}

ElementStyle::ElementStyle(const char* key, const char * color_spec, float dx) :
    key(key),
    fill_color(parse_color(color_spec)),
    stroke_color(0),
    dx(dx)
{
}

ElementStyle::ElementStyle(const char *key, PackedColor co, const char *stroke_spec, float dx) :
    key(key),
    fill_color(co),
    stroke_color(parse_color(stroke_spec)),
    dx(dx)
{
}


ElementStyle::ElementStyle(const char *key, const char *color_spec, const char *stroke_spec, float dx) :
    key(key),
    fill_color(parse_color(color_spec)),
    stroke_color(parse_color(stroke_spec)),
    dx(dx)
{
}

void ElementStyle::apply_theme(std::shared_ptr<SvgTheme> theme)
{
    assert(key);
    auto style = theme->getStyle(key);
    if (style) {
        if (style->isApplyStroke()) {
            stroke_color = style->stroke_color();
            dx = style->stroke_width;
        }
        if (style->isApplyFill()) {
            fill_color = style->fill_color();
        }
    }
    // if (!isVisibleColor(fill_color) && isVisibleColor(stroke_color)) {
    //     fill_color = stroke_color;
    // }
}

}