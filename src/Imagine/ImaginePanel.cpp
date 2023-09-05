#include "Imagine.hpp"
#include "imagine_layout.hpp"
#include "../text.hpp"

namespace pachde {

ImaginePanel::ImaginePanel(Imagine *mod, Vec size)
{
    module = mod;
    box.size = size;
}

void ImaginePanel::drawTraversal(const DrawArgs &args, TraversalDrawOptions options)
{
    auto vg = args.vg;
    auto x = CONTROL_START + 2.6f * CONTROL_SPACING;
    auto w = 80.f;
    auto y = CONTROL_ROW - 8.5f;
    bool dim = rack::settings::rackBrightness <= .9f;
    auto color = (dim || (IsDarker(ModuleTheme(module))))
        ? nvgRGB(0xe6, 0xa2, 0x1a)
        : COLOR_BRAND;

    if (options & TraversalDrawOptions::Text) {
        if (dim) {
            FillRect(vg, x + 1.f, y + 1.f, w, 16.f, RampGray(G_05));
        }
        auto font = GetPluginFontSemiBold();
        if (FontOk(font)) {
            SetTextStyle(vg, font, color, 14.f);
            CenterText(vg, x + w * .5f, CONTROL_ROW + 5.f, TraversalName(module ? module->traversal_id : Traversal::SCANLINE).c_str(), nullptr);
        }
    }
    if (options & TraversalDrawOptions::Frame) {
        BoxRect(vg, x, y-.5f, w + 2.f, 18.f, color, 0.5f);
    }
}

void ImaginePanel::drawLayer(const DrawArgs &args, int layer)
{
    Widget::drawLayer(args, layer);
    if (!module || layer != 1) return;
    drawTraversal(args, TraversalDrawOptions::Text);
}

void ImaginePanel::draw(const DrawArgs &args)
{
    Theme theme = ModuleTheme(module);
    NVGcolor panel_color = PanelBackground(theme), textColor = ThemeTextColor(theme);
    auto vg = args.vg;

    // panel background
    FillRect(vg, 0.0, 0.0, box.size.x, box.size.y, panel_color);

    // dividers
    auto gray = RampGray(G_60);
    const float divider_x = 134.f;
    Line(vg, divider_x, OUTPUT_ROW - 5.f, divider_x, OUTPUT_ROW + 20.f, gray, 0.5f);
    Line(vg, 7.5f, OUTPUT_ROW - 5.f, PANEL_WIDTH - 7.5f, OUTPUT_ROW - 5.f, gray, 0.5f);

    drawTraversal(args, module ? TraversalDrawOptions::Frame : TraversalDrawOptions::Both);

    std::shared_ptr<rack::window::Font> font = nullptr;
    if (!module || (module->labels && rack::settings::rackBrightness > .35f)) {
        font = GetPluginFontRegular();
        if (FontOk(font))
        {
            // title
            SetTextStyle(vg, font, textColor);
            CenterText(vg, PANEL_CENTER, ONE_HP - 2.5, "imagine", nullptr);

            float x = CONTROL_START;
            CenterText(vg, x, CONTROL_ROW_TEXT, "speed", nullptr);

            x += CONTROL_SPACING;
            if (module) {
                auto p = module->getParamQuantity(Imagine::SPEED_MULT_PARAM);
                CenterText(vg, x, CONTROL_ROW_TEXT, p->getDisplayValueString().c_str(), nullptr);
            } else {
                CenterText(vg, x, CONTROL_ROW_TEXT, "1x", nullptr);
            }
            x += CONTROL_SPACING;
            CenterText(vg, x, CONTROL_ROW_TEXT, "path", nullptr);

            x = box.size.x - CONTROL_START - 3.f * CONTROL_SPACING;
            CenterText(vg, x, CONTROL_ROW_TEXT, "slew", nullptr);
            x += CONTROL_SPACING;
            CenterText(vg, x, CONTROL_ROW_TEXT, ComponentShortName(module? module->color_component : ColorComponent::LUMINANCE) , nullptr);
            x += CONTROL_SPACING;
            CenterText(vg, x, CONTROL_ROW_TEXT, "g/t", nullptr);
            x += CONTROL_SPACING;
            CenterText(vg, x, CONTROL_ROW_TEXT, "minT", nullptr);

            x = CONTROL_START;
            CenterText(vg, x, CONTROL_ROW_2_TEXT, "speed", nullptr);
            x += CONTROL_SPACING + (CONTROL_SPACING - TIGHT) * .5f;
            CenterText(vg, x, CONTROL_ROW_2_TEXT, "x/y", nullptr);

            x = box.size.x - CONTROL_START - 3.f * CONTROL_SPACING;
            CenterText(vg, x, CONTROL_ROW_2_TEXT, "reset", nullptr);
            x += CONTROL_SPACING;
            CenterText(vg, x, CONTROL_ROW_2_TEXT, "r", nullptr);
            x += CONTROL_SPACING;
            CenterText(vg, x, CONTROL_ROW_2_TEXT, "pp", nullptr);
            x += CONTROL_SPACING;
            CenterText(vg, x, CONTROL_ROW_2_TEXT, "minT", nullptr);

            // rotated "raw"
            nvgSave(vg);
            nvgTranslate(vg, divider_x, (OUTPUT_ROW + 12.f));
            nvgRotate(vg, nvgDegToRad(90.f));
            nvgFontSize(vg, 14.f);
            nvgFillColor(vg, gray);
            CenterText(vg, 0, 8.f, "raw", nullptr);
            nvgRestore(vg);

            // raw outputs
            //nvgFillColor(vg, RampGray(G_85));
            CenterText(vg, 45.5f, OUTPUT_ROW_TEXT, "x/y", nullptr);

            // processed outputs
            CenterText(vg, 190.f + 7.5f, OUTPUT_ROW_TEXT, "p", nullptr);
            CenterText(vg, 210.f + 12.f, OUTPUT_ROW_TEXT, "v", nullptr);
            CenterText(vg, 240.f + 12.f, OUTPUT_ROW_TEXT, "g", nullptr);
            CenterText(vg, 265.f + 12.f, OUTPUT_ROW_TEXT, "t", nullptr);
        }
    }

    NVGcolor gold = nvgRGB(0xe6, 0xa2, 0x1a);
    if (!module || module->medallion_fill) {
        Circle(vg, PANEL_CENTER, RACK_GRID_HEIGHT + 7.5f, 30.f, gold);
    } else {
        OpenCircle(vg, PANEL_CENTER, RACK_GRID_HEIGHT + 7.5f, 30.f, gold, .75f);
    }
    Line(vg, PANEL_CENTER-30.f, RACK_GRID_HEIGHT-1.f, PANEL_CENTER+30.f, RACK_GRID_HEIGHT-1.f, panel_color, 2.f);
    DrawLogo(vg, PANEL_CENTER-12.f, RACK_GRID_HEIGHT-ONE_HP-7.f, 
        IsDarker(theme)
            ? (!module || module->medallion_fill) ? RampGray(G_05) : gold
            : RampGray(G_20),
        1.5);

    Widget::draw(args);
}

}