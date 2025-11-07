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
    ssize_t selected_guide{-1};

    ElementStyle style_bg{"guide-bg", colors::G65};
    ElementStyle style_hover{"guide-hover", colors::NoColor, colors::PortLightOrange, 1.5f};
    ElementStyle style_selected{"guide-sel", colors::PortLightOrange, colors::PortLightOrange, 1.5f};
    ElementStyle style_text{"guide-text", colors::G10};
    ElementStyle style_text_sel{"guide-seltext", colors::G10};

    bool applyTheme(std::shared_ptr<SvgTheme> theme) override;

    void draw(const DrawArgs& args) override;
};

}