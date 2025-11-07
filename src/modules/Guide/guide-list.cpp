#include "services/text.hpp"
#include "guide-list.hpp"
#include "services/svg-theme-2.hpp"
using namespace svg_theme_2;

namespace pachde {
bool GuideList::applyTheme(std::shared_ptr<SvgTheme> theme)
{
    style_bg.apply_theme(theme);
    style_hover.apply_theme(theme);
    style_selected.apply_theme(theme);
    style_text.apply_theme(theme);
    style_text_sel.apply_theme(theme);
    return false;
}

void GuideList::draw(const DrawArgs &args)
{
    auto vg = args.vg;
    auto font = GetPluginFontRegular();
    if (!FontOk(font)) return;
    nvgTextAlign(vg, NVG_ALIGN_TOP|NVG_ALIGN_LEFT);
    nvgFontSize(vg, 14.f);
    auto co_bg = style_bg.nvg_color();
    auto co_hi = style_selected.nvg_stroke_color();
    int guide_id = 1;
    float x = 1.5f;
    float y = 1.5f;
    float w = box.size.x - 3.f;
    float h = 16;
    for (auto guide: guide_data->guides) {

        FillRect(vg, x, y, w, h-.5f, co_bg);

        nvgFillColor(vg, co_bg);
        std::string name = guide->name.empty() ? format_string("Guide #%d", guide_id) : guide->name;
        nvgText(vg, x + 2.5f, y + 1.5f, name.c_str(), nullptr);

        if ((guide_id - 1) == selected_guide) {
            FittedBoxRect(vg, x, y, w, h, co_hi, Fit::Inside, style_selected.width());
        }
        guide_id++;
        y += h;
    }

}

}