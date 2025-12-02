#pragma once
#ifndef PACHDE_TEXT_HPP_INCLUDED
#define PACHDE_TEXT_HPP_INCLUDED
#include <rack.hpp>
#include <string>
#include "./colors.hpp"
#include "text-align.hpp"

using namespace ::rack;

namespace pachde {

std::string format_string(const char *fmt, ...);
std::string hsla_string(float hue, float saturation, float lightness, float alpha);
std::string rgba_string(PackedColor color);

std::shared_ptr<window::Font> GetPluginFontSemiBold(const char * path = NULL);

std::shared_ptr<window::Font> GetPluginFontRegular(const char * path = NULL);

inline bool FontOk(std::shared_ptr<window::Font> font) {
    return font && font->handle >= 0;
}

void SetTextStyle(NVGcontext *vg, std::shared_ptr<window::Font> font, NVGcolor color = RampGray(G_20), float height = 16);

// Center text horizontally on the given point
// Text style must have been previously set
void CenterText(NVGcontext *vg, float x, float y, const char * text, const char * end);

// Text style must have been previously set.
void RightAlignText(NVGcontext *vg, float x, float y, const char * text, const char * end);

void draw_text_box (
    NVGcontext *vg,
    float x, float y, float w, float h,
    float left_margin, float right_margin,
    float top_margin, float bottom_margin,
    std::string text,
    std::shared_ptr<rack::window::Font> font,
    float font_size,
    PackedColor text_color,
    HAlign halign,
    VAlign valign,
    PackedColor margin_color = colors::NoColor,
    float first_baseline = INFINITY
);

void draw_oriented_text_box(
    NVGcontext *vg,
    Rect box,
    float left_margin, float right_margin,
    float top_margin, float bottom_margin,
    const std::string& text,
    std::shared_ptr<rack::window::Font> font,
    float font_size,
    PackedColor text_color,
    HAlign halign,
    VAlign valign,
    Orientation orientation,
    PackedColor margin_color = colors::NoColor,
    float first_baseline = INFINITY
);

} // namespace pachde
#endif