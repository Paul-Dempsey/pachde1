#include "color-picker.hpp"
#include "../services/misc.hpp"
#include "../services/color-help.hpp"

namespace pachde {

void ColorPicker::set_text_color()
{
    switch (syntax) {
    case ColorSyntax::Unknown:
        break;
    case ColorSyntax::Hex:
        text_input->text = hex_string(color);
        break;
    case ColorSyntax::RGB:
        text_input->text = format_string("rgb(%d,%d,%d)", Red(color), Green(color), Blue(color));
        break;
    case ColorSyntax::HSL:
        text_input->text = hsl_string(hue, saturation, lightness, nvg_color.a);
        break;
    }
}

void ColorPicker::set_color(PackedColor co)
{
    color = co;
    sample->color = co;
    nvg_color = fromPacked(co);
    hue = Hue1(nvg_color);
    saturation = Saturation(nvg_color);
    lightness = Lightness(nvg_color);
    
    hue_picker->setHue(hue);
    sl_picker->setHue(hue);
    sl_picker->setSaturation(saturation);
    sl_picker->setLightness(lightness);
    set_text_color();
    if (on_new_color) on_new_color(color);
}

void ColorPicker::refresh_from_hsl() {
    nvg_color = nvgHSL(hue, saturation, lightness);
    color = toPacked(nvg_color);
    sample->color = color;
    hue_picker->setHue(hue);
    sl_picker->setHue(hue);
    sl_picker->setSaturation(saturation);
    sl_picker->setLightness(lightness);
    set_text_color();
    if (on_new_color) on_new_color(color);
}

ColorPicker::ColorPicker()
{
    box.size = get_size();

    hue_picker = new HueWidget();
    hue_picker->hue = get_hue();
    hue_picker->box.pos = Vec(0,0);
    hue_picker->box.size = Vec(15.f, 224);
    hue_picker->onClick([=](float hue) {
        set_hue(hue);
    });
    addChild(hue_picker);

    sl_picker = new SLWidget(get_hue());
    sl_picker->setSaturation(get_saturation());
    sl_picker->setLightness(get_lightness());
    sl_picker->box.pos = Vec(18.5f, 0);
    sl_picker->box.size = Vec(136.f, 224.f);
    sl_picker->onClick([=](float sat, float light) {
        set_saturation(sat);
        set_lightness(light);
    });
    addChild(sl_picker);

    sample = createWidget<Swatch>(Vec(0, 227.5f));
    sample->box.size = Vec(149.f, 14.f);
    addChild(sample);

    text_input = createWidget<TextInput>(Vec(0, 244.f));
    text_input->box.size = sample->box.size;
    text_input->text_height = 14.f;
    text_input->set_on_change([=](std::string text) {
        syntax = ColorSyntax::Unknown;
        if (isValidHexColor(text)) syntax = ColorSyntax::Hex;
        else if (is_hsl(text)) syntax = ColorSyntax::HSL;
        else if (is_rgb(text)) syntax = ColorSyntax::RGB;
        if (syntax != ColorSyntax::Unknown) {
            auto co = parse_color(text, RARE_COLOR);
            if (co != RARE_COLOR) {
                set_color(co);
            }
        }
    });
    addChild(text_input);
}

}