#pragma once
#include <rack.hpp>
using namespace ::rack;
#include <unordered_map>
#include "packed-color.hpp"
using namespace ::packed_color;

namespace svg_theme_2 {

const float NoOffset = std::nanf("");

struct GradientStop {
    int index{-1};
    PackedColor color{colors::NoColor};
    float offset{NoOffset};

    void reset() { index = -1;  color = colors::NoColor; offset = NoOffset; }
    bool hasIndex() const { return index >= 0; }
    bool hasOffset() const { return !_isnanf(offset); }
};

struct Gradient {
    std::vector<GradientStop> stops;
    void clear() { stops.clear(); }
};

enum class PaintKind { Unset, Color, Gradient, None };

class Paint {
    PaintKind kind{PaintKind::Unset};
    union {
        PackedColor color;
        Gradient gradient;
    };
public:
    ~Paint() {
        if (isGradient()) gradient.clear();
    }
    Paint& operator=(const Paint& source) {
        switch (source.kind) {
        case PaintKind::Unset: break;
        case PaintKind::Color: setColor(source.color); break;
        case PaintKind::Gradient: setGradient(source.gradient); break;
        case PaintKind::None: setNone(); break;
        }
        return *this;
    }
    Paint() {}
    Paint(const Paint& source) {
        switch (source.kind) {
        case PaintKind::Unset: break;
        case PaintKind::Color: setColor(source.color); break;
        case PaintKind::Gradient: setGradient(source.gradient); break;
        case PaintKind::None: setNone(); break;
        }
    }
    Paint(PackedColor color) { setColor(color); }
    Paint(const Gradient& gradient) { setGradient(gradient); }

    PaintKind Kind() { return kind; }
    void setColor(PackedColor new_color) {
        if (isGradient()) gradient.clear();
        kind = PaintKind::Color;
        color = new_color;
    }
    void setGradient(const Gradient& g) {
        kind = PaintKind::Gradient;
        gradient = g;
    }
    void setNone() {
        if (isGradient()) gradient.clear();
        kind = PaintKind::None;
    }
    bool isColor()    { return kind == PaintKind::Color; }
    bool isGradient() { return kind == PaintKind::Gradient; }
    bool isNone()     { return kind == PaintKind::None; }
    PackedColor getColor() { return isColor() ? color : colors::NoColor; }
    const Gradient* getGradient() { return isGradient() ? &gradient : nullptr; }
    bool isApplicable() { return kind != PaintKind::Unset; }
};

struct Style
{
    Paint fill;
    Paint stroke;
	float opacity = 1.f;
	float stroke_width = 1.f;
    bool apply_stroke_width = false;
    bool apply_opacity = false;

    void setFill(const Paint& paint) { fill = paint; }
    void setFillColor(PackedColor color) { fill.setColor(color); }
    void setFillGradient(const Gradient& gradient) { fill.setGradient(gradient); }
    void setStroke(const Paint& paint) { stroke = paint; }
    void setStrokeColor(PackedColor color) { stroke.setColor(color); }
    void setStrokeGradient(const Gradient& gradient) { stroke.setGradient(gradient); }

    void setOpacity(float alpha) {
        opacity = alpha;
        apply_opacity = true;
    }
    void setStrokeWidth(float width) {
        stroke_width = width;
        apply_stroke_width = true;
    }
    bool isApplyFill() { return fill.isApplicable(); }
    bool isApplyStroke() { return stroke.isApplicable(); }
    bool isApplyOpacity() { return apply_opacity; }
    bool isApplyStrokeWidth() { return apply_stroke_width; }
    PackedColor fill_color() {
        return isApplyFill() ? fill.getColor() : colors::NoColor;
    }
    PackedColor stroke_color() {
        return isApplyStroke() ? stroke.getColor() : colors::NoColor;
    }
    PackedColor fillWithOpacity() {
        if (!isApplyFill()) return colors::NoColor;
        return (apply_opacity) ? transparent(fill.getColor(), opacity) : fill.getColor();
    }
    PackedColor strokeWithOpacity() {
        if (!isApplyStroke()) return colors::NoColor;
        return (apply_opacity) ? transparent(stroke.getColor(), opacity) : stroke.getColor();
    }
};

struct SvgTheme
{
    std::string name;
    std::string file;
    std::unordered_map<std::string, std::shared_ptr<Style>> styles;

    std::shared_ptr<Style> getStyle(const std::string &name);
    bool getGradient(const Gradient** result, const char *name);
    bool getFillColor(PackedColor& result, const char *name, bool with_opacity);
    bool getStroke(PackedColor& result, const char *name, bool with_opacity, float* width);
};

std::string parseTag(const char * id);
std::shared_ptr<SvgTheme> loadFile(std::string path);

}