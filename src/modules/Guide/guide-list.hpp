#pragma once
#include <rack.hpp>
using namespace ::rack;
#include "guide-data.hpp"
#include "widgets/element-style.hpp"
using namespace ::widgetry;

namespace pachde {

struct GuideList : OpaqueWidget, IThemed
{
    using Base = OpaqueWidget;

    GuideData* guide_data{nullptr};
    int selected_guide{-1};
    int hovered_guide{-1};
    std::function<void(std::shared_ptr<GuideLine> guide)> click_handler{nullptr};

    ElementStyle style_list{"guide-list", colors::G55, colors::G15, .85f};
    ElementStyle style_item{"guide-bg", colors::G65};
    ElementStyle style_hovered{"guide-hover", colors::NoColor, colors::PortLightOrange, 1.5f};
    ElementStyle style_selected{"guide-sel", colors::PortLightOrange};
    ElementStyle style_text{"guide-text", colors::G10};
    ElementStyle style_text_sel{"guide-seltext", colors::G0};

    void set_click_handler(std::function<void(std::shared_ptr<GuideLine> guide)> handler) {
        click_handler = handler;
    }

    bool applyTheme(std::shared_ptr<SvgTheme> theme) override;

    std::shared_ptr<GuideLine> get_selected_guide();

    int item_at_position(Vec pos);

    void onButton(const ButtonEvent&e) override;
    void onHover(const HoverEvent& e) override;
    void onHoverKey(const HoverKeyEvent& e) override;
//    void onEnter(const EnterEvent& e) override;
    void onLeave(const LeaveEvent& e) override;
    void onAction(const ActionEvent& e) override;

    void draw(const DrawArgs& args) override;
};

}