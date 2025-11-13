#include "guide-list.hpp"
#include "services/text.hpp"
#include "services/svg-theme.hpp"
#include "widgets/widgetry.hpp"
using namespace svg_theme;

namespace pachde {

std::shared_ptr<GuideLine> GuideList::get_selected_guide() {
    return (selected_guide >= 0) ? guide_data->guides[selected_guide] : nullptr;
}

void GuideList::onButton(const ButtonEvent &e)
{
    if (e.button != GLFW_MOUSE_BUTTON_LEFT) {
        Base::onButton(e);
        return;
    }
    if (e.action == GLFW_PRESS) {
        ActionEvent eAction;
        onAction(eAction);
    }
}
void GuideList::onAction(const ActionEvent& e)  {
    e.consume(this) ;
    selected_guide = hovered_guide;
    if (click_handler) {
        click_handler((selected_guide >= 0) ? guide_data->guides[selected_guide] : nullptr);
    }
    Base::onAction(e);
}

int GuideList::item_at_position(Vec pos)
{
    Rect target_rect{Vec(1.5f,1.5f), Vec(box.size.x - 3.f, 16.f * guide_data->guides.size())};
    return (target_rect.contains(pos)) ? static_cast<int>((pos.y - target_rect.pos.y) / 16) : -1;
}

void GuideList::onHover(const HoverEvent &e)
{
    Base::onHover(e);
    e.consume(this);
    hovered_guide = item_at_position(e.pos);
    glfwSetCursor(APP->window->win, (hovered_guide >= 0) ? glfwCreateStandardCursor(GLFW_HAND_CURSOR) : nullptr);
}

void GuideList::onHoverKey(const HoverKeyEvent &e) {
    Base::onHoverKey(e);
}

void GuideList::onLeave(const LeaveEvent &e)
{
    hovered_guide = -1;
    glfwSetCursor(APP->window->win, nullptr);
}

bool GuideList::applyTheme(std::shared_ptr<SvgTheme> theme)
{
    style_list.apply_theme(theme);
    style_item.apply_theme(theme);
    style_hovered.apply_theme(theme);
    style_selected.apply_theme(theme);
    style_text.apply_theme(theme);
    style_text_sel.apply_theme(theme);
    return false;
}

void GuideList::draw(const DrawArgs &args)
{
    auto vg = args.vg;
    if (style_list.fill_color) {
        FillRect(vg, 0, 0, box.size.x, box.size.y, style_list.nvg_color());
    }
    if (style_list.stroke_color) {
        FittedBoxRect(vg,  0, 0, box.size.x, box.size.y, style_list.nvg_stroke_color(), Fit::Inside, style_list.width());
    }

    auto font = GetPluginFontRegular();
    if (!FontOk(font)) return;

    nvgTextAlign(vg, NVG_ALIGN_TOP|NVG_ALIGN_LEFT);
    nvgFontSize(vg, 12.f);

    auto co_bg = style_item.nvg_color();
    auto co_text = style_text.nvg_color();

    int guide_id = 0;
    float x = 1.5f;
    float y = 1.5f;
    float w = box.size.x - 3.f;
    float h = 16;

    for (auto guide: guide_data->guides) {

        if (guide_id == selected_guide) {
            if (style_selected.fill_color) {
                FillRect(vg, x, y, w, h-.5f, style_selected.nvg_color());
            }
            if (style_selected.stroke_color) {
                FittedBoxRect(vg, x, y, w, h, style_selected.nvg_stroke_color(), Fit::Inside, style_selected.width());
            }
        } else {
            FillRect(vg, x, y, w, h-.5f, co_bg);
        }

        nvgFillColor(vg, (guide_id == selected_guide) ? style_text_sel.nvg_color() : co_text);
        nvgText(vg, x + 2.5f, y + 1.5f, guide->name.c_str(), nullptr);

        if (guide_id == hovered_guide) {
            FittedBoxRect(vg, x, y, w, h, style_hovered.nvg_stroke_color(), Fit::Inside, style_hovered.width());
        }
        guide_id++;
        y += h;
    }

}

}