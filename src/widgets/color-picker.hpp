#pragma once
#include <rack.hpp>
using namespace ::rack;
#include "color-widgets.hpp"
#include "text-input.hpp"

namespace widgetry {

struct ColorPicker : OpaqueWidget
{
    static Vec get_size() { return  Vec(170.f, 260.f); }

    HueWidget* hue_picker{nullptr};
    SLWidget* sl_picker{nullptr};
    AlphaWidget* alpha_picker{nullptr};
    Swatch* sample{nullptr};
    SolidSwatch* solid{nullptr};
    TextInput* text_input{nullptr};
    ColorSyntax syntax = ColorSyntax::Hex;
    SyntaxSelector* syntax_selector{nullptr};

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