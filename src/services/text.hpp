#pragma once
#ifndef PACHDE_TEXT_HPP_INCLUDED
#define PACHDE_TEXT_HPP_INCLUDED
#include <rack.hpp>
#include <string>
#include "./colors.hpp"
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

enum class BaselineCorrection {
    None,
    Baseline
};

// The y coordinate is the baseline (BaselineCorrection::none)
// or the bottom of text box (BaselineCorrection::Baseline).
// Text style must have been previously set.
void RightAlignText(NVGcontext *vg, float x, float y, const char * text, const char * end, BaselineCorrection correction = BaselineCorrection::None);

} // namespace pachde
#endif