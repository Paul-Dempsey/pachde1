#pragma once
#include <rack.hpp>
using namespace ::rack;
#include "sl_widget.hpp"
#include "hue_widget.hpp"
#include "text-input.hpp"

namespace widgetry {

struct Swatch: Widget {
    PackedColor color{0};
    void draw(const DrawArgs& args) override
    {
        if (color) {
            FillRect(args.vg, 0.f, 0.f, box.size.x, box.size.y, fromPacked(color));
        }
    }
};


struct AlphaWidget: OpaqueWidget {
    float alpha{1.0f};
    Vec drag_pos;
    std::function<void(float)> clickHandler;

    float getOpacity() { return alpha; }
    void setOpacity(float f) { alpha = clamp(f); }
    void onClick(std::function<void(float)> callback) {
        clickHandler = callback;
    }
    void onEnter(const EnterEvent &e) override {
        OpaqueWidget::onEnter(e);
        glfwSetCursor(APP->window->win, glfwCreateStandardCursor(GLFW_CROSSHAIR_CURSOR));
    }
    void onLeave(const LeaveEvent &e) override {
        OpaqueWidget::onLeave(e);
        glfwSetCursor(APP->window->win, NULL);
    }
    bool vertical() { return box.size.x <= box.size.y; }

    void onDragMove(const DragMoveEvent& e) override {
        OpaqueWidget::onDragMove(e);
        drag_pos = drag_pos.plus(e.mouseDelta.div(getAbsoluteZoom()));

        setOpacity(vertical() ? (drag_pos.y / box.size.y) : (drag_pos.x / box.size.x));
        if (clickHandler)
            clickHandler(alpha);
    }

    void onButton(const ButtonEvent& e) override {
    	OpaqueWidget::onButton(e);
        if (!(e.action == GLFW_PRESS && e.button == GLFW_MOUSE_BUTTON_LEFT && (e.mods & RACK_MOD_MASK) == 0)) {
            return;
        }
        e.consume(this);
        drag_pos = e.pos;
    }

    void draw(const DrawArgs& args) override {
        auto vg = args.vg;

        auto co_white = nvgRGBAf(1.f, 1.f, 1.f, 1.f);
        nvgBeginPath(vg);
        nvgRect(vg, 0, 0, box.size.x, box.size.y);
        nvgFillColor(vg, co_white);
        nvgFill(vg);

        nvgBeginPath(vg);
        bool g{true};
        for (float y = 0; y < box.size.y - 4.f; y += 4.f) {
            for (float x = 4; x < box.size.x-4.f; x += 4.f) {
                if (g) nvgRect(vg, x, y, 4.f, 4.f);
                g = !g;
            }
            g = !g;
        }
        nvgFillColor(vg, nvgRGBAf(.5,.5f,.5, 1.f));
        nvgFill(vg);

        nvgBeginPath(vg);
        nvgRect(vg, 0, 0, box.size.x, box.size.y);
        auto co_nada = nvgRGBAf(1.f,1.f,1.f,0.f);
        NVGpaint paint = vertical()
            ? nvgLinearGradient(vg, 0, 0, 0, box.size.y, co_nada, co_white)
            : nvgLinearGradient(vg, 0, 0, box.size.x, 0, co_nada, co_white);
        nvgFillPaint(vg, paint);
        nvgFill(vg);

        if (vertical()) {
            auto y = alpha * box.size.y;
            Line(vg, 0.f, y, box.size.x, y, RampGray(G_85), .75f);
            Line(vg, 0.f, y + .75f, box.size.x, y + .75f, RampGray(G_10), .75f);
        } else {
            auto x = alpha * box.size.x;
            Line(vg, x, 0.f, x, box.size.y, RampGray(G_85), .75f);
            Line(vg, x + .75f, 0.f, x + .75f, box.size.y, RampGray(G_10), .75f);
        }
    }

};

enum class ColorSyntax { Unknown = -1, Hex, RGB, HSL };

struct ColorPicker : OpaqueWidget
{
    static Vec get_size() { return  Vec(170.f, 260.f); }

    HueWidget* hue_picker{nullptr};
    SLWidget* sl_picker{nullptr};
    AlphaWidget* alpha_picker{nullptr};
    Swatch* sample{nullptr};
    TextInput* text_input{nullptr};
    ColorSyntax syntax = ColorSyntax::Hex;

    float hue;
    float saturation;
    float lightness;
    float alpha;

    NVGcolor nvg_color;
    PackedColor color{0};
    std::function<void(PackedColor)> on_new_color{nullptr};

    ColorPicker();

    void set_on_new_color(std::function<void(PackedColor)> callback) {
        on_new_color = callback;
    }
    PackedColor get_color() { return color; }

    void set_text_color();
    void set_color(PackedColor co);
    void refresh_from_hsla();
    void set_hue(float h) {
        hue = h;
        refresh_from_hsla();
    }
    float get_hue() { return hue;  }
    void set_saturation(float s) {
        saturation = s;
        refresh_from_hsla();
    }
    float get_saturation() { return saturation; }
    void set_lightness(float l) {
        lightness = l;
        refresh_from_hsla();
    }
    float get_lightness() { return lightness; }
    void set_alpha(float a) {
        alpha = a;
        refresh_from_hsla();
    }
    float get_alpha() { return alpha; }

};

struct ColorPickerMenu : rack::ui::MenuItem
{
    ColorPicker* picker{nullptr};

    ColorPickerMenu();

    void set_on_new_color(std::function<void(PackedColor)> callback) {
        picker->set_on_new_color(callback);
    }
    void set_color(PackedColor color) { picker->set_color(color); }
    void onAction(const ActionEvent& e) override {
        e.unconsume(); // don't close menu
    }
    void step() override {
        OpaqueWidget::step(); //note: NOT MenuItem::Step
    }

    void draw(const DrawArgs& args) override;
};

}