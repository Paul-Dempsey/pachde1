#include "color-picker.hpp"
#include "services/text.hpp"
#include "services/packed-color.hpp"

namespace widgetry {

void ColorPicker::set_text_color()
{
    switch (syntax) {
    case ColorSyntax::Unknown:
        break;
    case ColorSyntax::Hex:
        text_input->text = hex_string(color);
        break;
    case ColorSyntax::RGB:
        text_input->text = rgba_string(color);
        break;
    case ColorSyntax::HSL:
        text_input->text = hsla_string(hue, saturation, lightness, nvg_color.a);
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
        PackedColor co;
        if (parseColor(co, colors::NoColor, text.c_str(), nullptr)) {
            if (is_hsl_prefix(text.c_str())) syntax = ColorSyntax::HSL;
            else if (is_rgb_prefix(text.c_str())) syntax = ColorSyntax::RGB;
            else syntax = ColorSyntax::Hex;
            set_color(co);
        };
    });
    addChild(text_input);
}

ColorPickerMenu::ColorPickerMenu() {
    auto picker_size = ColorPicker::get_size();
    box.size = picker_size.plus(Vec(8.f, 8.f));
    picker = createWidgetCentered<ColorPicker>(box.getCenter());
    addChild(picker);
}

void ColorPickerMenu::draw(const DrawArgs& args)
{
    auto vg = args.vg;
    FillRect(vg, 0, 0, box.size.x, box.size.y, nvgRGB(0x18, 0x18, 0x18));
    FittedBoxRect(vg, 0, 0, box.size.x, box.size.y, RampGray(G_65), Fit::Inside, 1.5f);
    OpaqueWidget::draw(args);
}


}