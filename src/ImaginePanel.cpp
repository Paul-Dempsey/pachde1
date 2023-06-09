#include "Imagine.hpp"
#include "text.hpp"

namespace pachde {

ImaginePanel::ImaginePanel(Imagine *mod, Theme t, Vec size)
{
    module = mod;
    theme = ConcreteTheme(t);
    box.size = size;
}

void ImaginePanel::draw(const DrawArgs &args)
{
    NVGcolor panel = PanelBackground(theme), textColor = ThemeTextColor(theme);
    auto vg = args.vg;

    // nvgScissor(args.vg, RECT_ARGS(args.clipBox)); // when is scissor managment necessary?

    // panel background
    FillRect(vg, 0.0, 0.0, box.size.x, box.size.y, panel);

    // image placeholder
    //BoxRect(vg, PANEL_CENTER - PANEL_IMAGE_WIDTH / 2.0f, PANEL_IMAGE_TOP, PANEL_IMAGE_WIDTH, PANEL_IMAGE_HEIGHT, COLOR_BRAND);

    // outputs 1
    RoundRect(vg, 5.0f, 299.0f, 290.0f, 65.0f, OutputBackground(theme), 6.0f);

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
        CenterText(vg, PANEL_CENTER, ONE_HP / 2.0f, "imagine", nullptr);

        CenterText(vg, 20.0f, 235.0f, "speed", nullptr);
        if (module) {
            auto p = module->getParamQuantity(Imagine::SPEED_MULT_PARAM);
            CenterText(vg, 50.0f, 235.0f, p->getDisplayValueString().c_str(), nullptr);
        } else {
            CenterText(vg, 50.0f, 235.0f, "1x", nullptr);
        }
        CenterText(vg, 80.0f, 235.0f, ComponentInitial(module? module->color_component : ColorComponent::LUMINANCE) , nullptr);
        CenterText(vg, 110.0f, 235.0f, "path", nullptr);

        // outputs
        SetTextStyle(vg, font, GRAY85, 16);
        CenterText(vg, 24.5f, 336.0f, "x", nullptr);
        CenterText(vg, 52.5f, 336.0f, "y", nullptr);
        CenterText(vg, 82.5f, 336.0f, "slew", nullptr);
        CenterText(vg, 112.5f, 336.0f, "v", nullptr);
    }
    font = GetPluginFontRegular();
    if (FontOk(font))
    {
        SetTextStyle(vg, font, IsLighter(theme) ? COLOR_BRAND : COLOR_BRAND_HI, 14.0f);
        nvgTextAlign(vg, NVG_ALIGN_MIDDLE);
        nvgText(vg, 128.0f, 215.0f, TraversalName(module  ? module->traversal_id : Traversal::SCANLINE).c_str(), nullptr);
    }
    Widget::draw(args);
    // nvgResetScissor(args.vg);
}

}