#pragma once
#include <rack.hpp>
using namespace ::rack;
#include "services/packed-color.hpp"
#include "services/colors.hpp"
#include "services/text.hpp"
#include "services/text-align.hpp"
#include "services/svg-theme.hpp"
using namespace pachde;

namespace widgetry {

struct LabelStyle {
    const char * key{"label"};
    PackedColor color{colors::G50};
    bool bold{false};
    float text_height{12.f};
    // float left_margin{0.f};
    // float right_margin{0.f};
    float baseline{INFINITY};
    HAlign halign{HAlign::Center};
    VAlign valign{VAlign::Middle};
    Orientation orientation{Orientation::Normal};

    LabelStyle() {}
    LabelStyle(const char * key) : key(key) {}

    LabelStyle(const char * key, PackedColor color, float font_size = 12.f, bool bold = false)
        : key(key), color(color), bold(bold), text_height(font_size) {}

    LabelStyle(const char * key, const char * color_spec, float font_size = 12.f, bool bold = false)
        : key(key), color(parseColor(color_spec, colors::G50)), bold(bold), text_height(font_size) {}

    void applyTheme(std::shared_ptr<svg_theme::SvgTheme> theme) {
        if (!theme->getFillColor(color, key, true)) {
            color = colors::PortRed;
        }
    }
};

struct TextLabel : OpaqueWidget, svg_theme::IThemed
{
    using Base = TransparentWidget;

    LabelStyle* format{nullptr};
    std::string text;
    bool own_format{false};

    TextLabel() {}
    TextLabel(LabelStyle* s) : format(s) {}
    ~TextLabel() {
        if (own_format) {
            assert(format);
            delete format;
        }
    }

    LabelStyle* create_owned_format() {
        format = new LabelStyle();
        own_format = true;
        return format;
    }

    bool applyTheme(std::shared_ptr<svg_theme::SvgTheme> theme) override {
        if (format) format->applyTheme(theme);
        return false;
    }

    void draw(const DrawArgs& args) override {
        auto font = ((format && format->bold)) ? GetPluginFontSemiBold() : GetPluginFontRegular();
        if (!FontOk(font)) return;

        if (format) {
            draw_oriented_text_box(
                args.vg, box.zeroPos(), 0.f, 0.f, //format->left_margin, format->right_margin,
                text, font, format->text_height, format->color,
                format->halign, format->valign, format->orientation,
                format->baseline
            );
        } else {
            draw_oriented_text_box(
                args.vg, box.zeroPos(), 0.f, 0.f, text, font, 12.f, colors::Black,
                HAlign::Center, VAlign::Baseline, Orientation::Normal
            );
        }
    }

    static TextLabel* createLabel(Rect bounds, const std::string& text, LabelStyle* format, std::shared_ptr<svg_theme::SvgTheme> theme = nullptr) {
        auto label = new TextLabel(format);
        label->box = bounds;
        label->text = text;
        if (theme) {
            label->applyTheme(theme);
        }
        return label;
    }

    static TextLabel* createLabel(Vec(pos), const std::string& text, LabelStyle* format, float width) {
        auto label = new TextLabel(format);
        label->box.pos = pos;
        label->box.size.x = width;
        label->box.size.y = format->text_height;
        return label;
    }

    static TextLabel* createLabel(Vec(pos), const std::string& text, float width, float text_height) {
        auto label = new TextLabel(new LabelStyle);
        label->box.pos = pos;
        label->box.size.x = width;
        label->box.size.y = text_height;
        label->format->text_height = text_height;
        return label;
    }
};

}
