#pragma once
#include <rack.hpp>
using namespace ::rack;
#include "services/svg-theme.hpp"
#include "widgets/element-style.hpp"

namespace widgetry {

inline void drawWidgetBox(NVGcontext* vg, Vec size, ElementStyle& style, Fit fit) {
    if (packed_color::isVisible(style.fill_color)) {
        FillRect(vg, 0, 0, size.x, size.y, style.nvg_color());
    }
    if (packed_color::isVisible(style.stroke_color)) {
        FittedBoxRect(vg, 0, 0, size.x, size.y, style.nvg_stroke_color(), fit, style.dx);
    }
}

struct CloseButton : OpaqueWidget, IThemed {
    using Base = OpaqueWidget;
    bool hovered{false};
    bool down{false};

    ElementStyle hover{"close-hover", "hsl(0, .6, .5)", .75f};
    ElementStyle frame{"close-frame", 0, colors::G65, .75};
    ElementStyle glyph{"close-glyph", colors::Black, .75f};

    std::function<void()> click_handler{nullptr};

    CloseButton() {
        box.size = Vec(14.f, 14.f);
    }

    bool applyTheme(std::shared_ptr<SvgTheme> theme) override {
        hover.apply_theme(theme);
        frame.apply_theme(theme);
        glyph.apply_theme(theme);
        return false;
    }

    void set_handler(std::function<void()> click) {
        click_handler = click;
    }
    void onEnter(const EnterEvent& e) override {
        hovered = true;
        Base::onEnter(e);
    }
    void onLeave(const LeaveEvent& e) override {
        hovered = false;
        Base::onLeave(e);
    }

    void onButton(const ButtonEvent& e) override {
        if (e.button != GLFW_MOUSE_BUTTON_LEFT) {
            Base::onButton(e);
            return;
        }
        if (e.action == GLFW_PRESS) {
            down = true;
        }
        if (e.action == GLFW_RELEASE) {
            down = false;
            ActionEvent eAction;
            onAction(eAction);
        }
        e.consume(this);
    }

    void onAction(const ActionEvent& e) override {
        e.consume(this);
        if (click_handler) {
            click_handler();
        }
        Base::onAction(e);
    }

    void onHover(const HoverEvent& e) override { Base::onHover(e); e.consume(this); }

    void draw(const DrawArgs& args) override {
        auto vg = args.vg;
        drawWidgetBox(vg, box.size, (hovered||down) ? hover : frame, (hovered||down) ? Fit::Outside: Fit::None);

        nvgTranslate(vg, 1.5, 1.5); // offset glyph
        // glyph
        nvgBeginPath(vg);
        nvgMoveTo(vg, 5.1f, 5.79f);
        nvgLineTo(vg, 0.f, 0.72f);
        nvgLineTo(vg, 0.7f, 0.f);
        nvgLineTo(vg, 5.8f, 5.07f);
        nvgLineTo(vg, 10.9f, 0.f);
        nvgLineTo(vg, 11.6f, 0.72f);
        nvgLineTo(vg, 6.5f, 5.79f);
        nvgLineTo(vg, 11.6f, 10.89f);
        nvgLineTo(vg, 10.9f, 11.59f);
        nvgLineTo(vg, 5.8f, 6.52f);
        nvgLineTo(vg, 0.7f, 11.59f);
        nvgLineTo(vg, 0.f, 10.89f);
        nvgClosePath(vg);
        nvgFillColor(vg, glyph.nvg_color());
        nvgFill(vg);
    }
};

}