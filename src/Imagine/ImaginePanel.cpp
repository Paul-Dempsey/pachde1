#include "Imagine.hpp"
#include "../text.hpp"

namespace pachde {

ImaginePanel::ImaginePanel(Imagine *mod, Vec size)
{
    module = mod;
    box.size = size;
}

void ImaginePanel::draw(const DrawArgs &args)
{
    Theme theme = module ? module->getTheme() : DefaultTheme;
    NVGcolor panel = PanelBackground(theme), textColor = ThemeTextColor(theme);
    auto vg = args.vg;

    // nvgScissor(args.vg, RECT_ARGS(args.clipBox)); // when is scissor managment necessary?

    // panel background
    FillRect(vg, 0.0, 0.0, box.size.x, box.size.y, panel);

    // image placeholder
    if (!module || !module->image.ok()) {
        FillRect(vg, PANEL_CENTER - PANEL_IMAGE_WIDTH / 2.0f, PANEL_IMAGE_TOP, PANEL_IMAGE_WIDTH, PANEL_IMAGE_HEIGHT, COLOR_BRAND);
    }

    // outputs 1
    RoundRect(vg, 5.f, 314.f, 290.f, 50.f, OutputBackground(theme), 6.f);
    if (Theme::HighContrast == theme) {
        RoundBoxRect(vg, 5.f, 314.f, 290.f, 50.f, RampGray(Ramp::G_WHITE), 6.0f);
    }
    // grid
    // auto gridline = IsLighter(theme) ? GRAY25 : nvgTransRGBAf(COLOR_GREEN_HI, 0.65);
    // for (float y = ONE_HP; y < box.size.y; y += ONE_HP) {
    //     Line(vg, 5.0f, y, box.size.x - 5.0f, y, gridline, 0.35f);
    // }

    auto font = GetPluginFontSemiBold();
    if (FontOk(font))
    {
        // title
        SetTextStyle(vg, font, textColor);
        CenterText(vg, PANEL_CENTER, ONE_HP - 3.f, "imagine", nullptr);

        auto control_row_text = 304.f;
        CenterText(vg, 25.0f, control_row_text, "speed", nullptr);
        if (module) {
            auto p = module->getParamQuantity(Imagine::SPEED_MULT_PARAM);
            CenterText(vg, 55.0f, control_row_text, p->getDisplayValueString().c_str(), nullptr);
        } else {
            CenterText(vg, 55.0f, control_row_text, "1x", nullptr);
        }
        CenterText(vg, 85.0f, control_row_text, ComponentInitial(module? module->color_component : ColorComponent::LUMINANCE) , nullptr);
        CenterText(vg, 115.0f, control_row_text, "path", nullptr);

        // outputs
        auto output_row_text = 355.f;
        SetTextStyle(vg, font, GRAY85, 16.f);
        CenterText(vg, 25.f, output_row_text, "x", nullptr);
        CenterText(vg, 52.5f, output_row_text, "y", nullptr);
        CenterText(vg, 82.5f, output_row_text, "slew", nullptr);
        CenterText(vg, 112.5f, output_row_text, "v", nullptr);
        CenterText(vg, 136.25f, output_row_text, "p", nullptr);
        CenterText(vg, 162.5f, output_row_text, "g", nullptr);
        CenterText(vg, 187.5f, output_row_text, "t", nullptr);
        if (!module || !module->image.ok()) {
            CenterText(vg, PANEL_CENTER, PANEL_IMAGE_TOP + PANEL_IMAGE_HEIGHT - TWO_HP, "[ no image ]", nullptr);
        }
    }
    font = GetPluginFontRegular();
    if (FontOk(font))
    {
        SetTextStyle(vg, font, IsLighter(theme) ? COLOR_BRAND : COLOR_BRAND_HI, 14.0f);
        nvgTextAlign(vg, NVG_ALIGN_MIDDLE);
        nvgText(vg, 134.f, 280.f, TraversalName(module  ? module->traversal_id : Traversal::SCANLINE).c_str(), nullptr);
    }
    Widget::draw(args);
    // nvgResetScissor(args.vg);
}

}