#include "Imagine.hpp"
#include "imagine_layout.hpp"
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
    NVGcolor panel_color = PanelBackground(theme), textColor = ThemeTextColor(theme);
    auto vg = args.vg;

    // nvgScissor(args.vg, RECT_ARGS(args.clipBox)); // when is scissor managment necessary?

    // panel background
    FillRect(vg, 0.0, 0.0, box.size.x, box.size.y, panel_color);

    // image placeholder
    if (!module) {
        FillRect(vg, PANEL_CENTER - PANEL_IMAGE_WIDTH / 2., PANEL_IMAGE_TOP, PANEL_IMAGE_WIDTH, PANEL_IMAGE_HEIGHT, COLOR_BRAND);
        // if (module) {
        //     if (Traversal::XYPAD == module->getTraversalId()) {
        //         auto color = RampGray(G_85);
        //         Line(vg, PANEL_CENTER, PANEL_IMAGE_TOP, PANEL_CENTER, PANEL_IMAGE_TOP+PANEL_IMAGE_HEIGHT, color, .5);
        //         Line(vg, PANEL_CENTER - PANEL_IMAGE_WIDTH/2., PANEL_IMAGE_TOP + PANEL_IMAGE_HEIGHT/2., PANEL_CENTER + PANEL_IMAGE_WIDTH/2, PANEL_IMAGE_TOP + PANEL_IMAGE_HEIGHT/2., color, .5);
        //     }
        // }
    }

    // outputs
    // We're all outputs, so for now, don't add output background
    // RoundRect(vg, 5.f, 314.f, 290.f, 50.f, OutputBackground(theme), 6.f);
    // if (Theme::HighContrast == theme) {
    //     RoundBoxRect(vg, 5.f, 314.f, 290.f, 50.f, RampGray(Ramp::G_WHITE), 6.0f);
    // }

    auto font = GetPluginFontRegular();//GetPluginFontSemiBold();
    if (FontOk(font))
    {
        // title
        SetTextStyle(vg, font, textColor);
        CenterText(vg, PANEL_CENTER, ONE_HP - 2.5, "imagine", nullptr);

        CenterText(vg, 25.0, CONTROL_ROW_TEXT, "speed", nullptr);
        if (module) {
            auto p = module->getParamQuantity(Imagine::SPEED_MULT_PARAM);
            CenterText(vg, 55.0, CONTROL_ROW_TEXT, p->getDisplayValueString().c_str(), nullptr);
        } else {
            CenterText(vg, 55.0, CONTROL_ROW_TEXT, "1x", nullptr);
        }
        CenterText(vg, 85., CONTROL_ROW_TEXT, "slew", nullptr);
        CenterText(vg, 115.0, CONTROL_ROW_TEXT, ComponentInitial(module? module->color_component : ColorComponent::LUMINANCE) , nullptr);
        CenterText(vg, 145.0, CONTROL_ROW_TEXT, "g/t", nullptr);
        CenterText(vg, 175.0, CONTROL_ROW_TEXT, "path:", nullptr);

        // raw outputs
        auto gray50 = RampGray(G_50);

        CenterText(vg, 37, OUTPUT_ROW_TEXT, "xy", nullptr);

        // divider
        const float divider_x = 126.5;
        Line(vg, divider_x, OUTPUT_ROW - 6.f, divider_x, OUTPUT_ROW + 30.f, gray50);
        // rotated "raw"
        nvgSave(vg);
        nvgTranslate(vg, divider_x, (OUTPUT_ROW + 12.f));
        nvgRotate(vg, nvgDegToRad(90.f));
        nvgFontSize(vg, 14.);
        nvgFillColor(vg, gray50);
        CenterText(vg, 0., 8., "raw", nullptr);
        nvgRestore(vg);

        // processed outputs
        CenterText(vg, 180.f + 7.5f, OUTPUT_ROW_TEXT, "p", nullptr);
        CenterText(vg, 200.f + 12.f, OUTPUT_ROW_TEXT, "v", nullptr);
        CenterText(vg, 230.f + 12.f, OUTPUT_ROW_TEXT, "g", nullptr);
        CenterText(vg, 255.f + 12.f, OUTPUT_ROW_TEXT, "t", nullptr);
    }
    font = GetPluginFontSemiBold(); // GetPluginFontRegular();
    if (FontOk(font))
    {
        auto color = IsLighter(theme) ? COLOR_BRAND : COLOR_BRAND_HI;
        Line(vg, 190.f, CONTROL_ROW_TEXT + 5.f, 190.f + 48.f, CONTROL_ROW_TEXT + 5.f, color, 0.5f);
        SetTextStyle(vg, font, color, 14.f);
        nvgTextAlign(vg, NVG_ALIGN_LEFT);
        nvgText(vg, 193.5f, CONTROL_ROW_TEXT, TraversalName(module  ? module->traversal_id : Traversal::SCANLINE).c_str(), nullptr);
    }

    Circle(vg, PANEL_CENTER, RACK_GRID_HEIGHT + 7.5f, 30.f, nvgHSL(40.f/360.f, 0.8f, 0.5f));
    Line(vg, PANEL_CENTER-30.f, RACK_GRID_HEIGHT-1.f, PANEL_CENTER+30.f, RACK_GRID_HEIGHT-1.f, panel_color, 2.f);
    DrawLogo(vg, PANEL_CENTER-12.f, RACK_GRID_HEIGHT-ONE_HP-7.f, (Theme::HighContrast == theme) ? RampGray(G_90) : RampGray(G_20) , 1.5);

    Widget::draw(args);
    // nvgResetScissor(args.vg);
}

}