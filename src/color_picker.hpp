#pragma once
#include <rack.hpp>
#include "components.hpp"
#include"pic.hpp"

using namespace rack;
namespace pachde {

struct ColorPicker: OpaqueWidget
{
    PickerTextField text_field;

    NVGcolor old_color;
    NVGcolor new_color;
    std::string hex_color;

    Pic * hue_ramp;
    Pic * sl_ramp;
    float hue, saturation, light;

    explicit ColorPicker(const NVGcolor& color)
    {
        text_field.setSize(Vec(100, 16));
        text_field.changeHandler = [this](std::string text) {
            this->setHexText(text);
        };
        text_field.commitHandler = [this](std::string text) {
            this->setHexText(text);
        };
        init(color);
    }

    void init(const NVGcolor& color) {
        new_color = old_color = color;
        hue = Hue(color);
        saturation = Saturation(color);
        light = Lightness(color);
        if (!hue_ramp) {
            hue_ramp = CreateHueRamp(4, 256, true);
        }
        if (!sl_ramp) {
            sl_ramp = Pic::CreateRaw(256, 256);
        }
        SetSLSpectrum(sl_ramp, hue);
        setHexColor(color);
    }

    void setHexColor(const NVGcolor& color) {
        hex_color = rack::color::toHexString(color);
        text_field.setText(hex_color);
    }

    void setHue(float new_hue) {
        if (hue == new_hue) return;
        hue = new_hue;
        new_color = nvgHSL(new_hue, saturation, light);
        setHexColor(new_color);
        SetSLSpectrum(sl_ramp, new_hue);
    }

    void setLS(float new_light, float new_sat) {
        if (new_light == light && new_sat == saturation) return;
        light = new_light;
        saturation = new_sat;
        new_color = nvgHSL(hue, saturation, light);
        setHexColor(new_color);
    }

    void setHexText(std::string new_hex) {
        new_color = rack::color::fromHexString(new_hex);
        auto old_hue = hue;
        hue = Hue(new_color);
        saturation = Saturation(new_color);
        light = Lightness(new_color);
        new_color = nvgHSL(hue, saturation, light);
        if (hue != old_hue) {
            SetSLSpectrum(sl_ramp, hue);
        }
        hex_color = new_hex;
        text_field.setText(new_hex);
    }

    //std::function<void(void)> changeHandler;

    void onButton(const event::Button& e) override
    {
        OpaqueWidget::onButton(e);
    }
    void onDragStart(const DragStartEvent &e) override
    {
        OpaqueWidget::onDragStart(e);
    }
    void onDragMove(const DragMoveEvent &e) override
    {
        OpaqueWidget::onDragMove(e);
    }
    void onDragEnd(const DragEndEvent &e) override
    {
        OpaqueWidget::onDragEnd(e);
    }

    void draw(const DrawArgs& args) override
    {
        OpaqueWidget::draw(args);
    }

}

}
