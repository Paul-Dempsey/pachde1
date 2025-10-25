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

enum class ColorSyntax { Unknown = -1, Hex, RGB, HSL };

struct ColorPicker : OpaqueWidget
{
    static Vec get_size() { return  Vec(154.f, 260.f); }

    HueWidget* hue_picker{nullptr};
    SLWidget* sl_picker{nullptr};
    Swatch* sample{nullptr};
    TextInput* text_input;
    ColorSyntax syntax = ColorSyntax::Hex;

    float hue{0.0f};
    float saturation{1.f};
    float lightness{1.f};
    NVGcolor nvg_color;
    PackedColor color{0};
    std::function<void(PackedColor)> on_new_color{nullptr};

    void set_on_new_color(std::function<void(PackedColor)> callback)
    {
        on_new_color = callback;
    }
    PackedColor get_color() { return color; }

    void set_text_color();
    void set_color(PackedColor co);
    void refresh_from_hsl();
    void set_hue(float h) {
        hue = h;
        refresh_from_hsl();
    }
    float get_hue() { return hue;  }
    void set_saturation(float s) {
        saturation = s;
        refresh_from_hsl();
    }
    float get_saturation() { return saturation; }
    void set_lightness(float l) {
        lightness = l;
        refresh_from_hsl();
    }
    float get_lightness() { return lightness; }

    ColorPicker();
};

struct ColorPickerMenu : rack::ui::MenuItem
{
    ColorPicker* picker{nullptr};

    ColorPickerMenu() {
        box.size = ColorPicker::get_size().plus(Vec(8.f, 8.f));
        picker = createWidgetCentered<ColorPicker>(box.getCenter());
        addChild(picker);
    }

    void set_on_new_color(std::function<void(PackedColor)> callback)
    {
        picker->set_on_new_color(callback);
    }

    void set_color(PackedColor color) { picker->set_color(color); }

    void onAction(const ActionEvent& e) override
    {
        e.unconsume(); // don't close menu
    }

    void step() override
    {
        OpaqueWidget::step(); //note: NOT MenuItem::Step
    }

    void draw(const DrawArgs& args) override
    {
        auto vg = args.vg;
        FillRect(vg, 0, 0, box.size.x, box.size.y, nvgRGB(0x18, 0x18, 0x18));
        FittedBoxRect(vg, 0, 0, box.size.x, box.size.y, RampGray(G_65), Fit::Inside, 1.5f);
        OpaqueWidget::draw(args);
    }
};

}