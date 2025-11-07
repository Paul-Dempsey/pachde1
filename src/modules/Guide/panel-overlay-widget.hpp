#pragma once
#include <rack.hpp>
using namespace ::rack;
#include "services/colors.hpp"
#include "services/svg-theme-2.hpp"
using namespace packed_color;


namespace pachde {
struct GuideUi;
struct GuideData;

struct PanelGuide : OpaqueWidget
{
    using Base = OpaqueWidget;

    GuideUi* ui{nullptr};
    GuideData* data{nullptr};

    PanelGuide() { box = Rect(Vec{0}, Vec{0}); }
    virtual ~PanelGuide();

    void step() override;
    void draw_panel(const DrawArgs& args, PackedColor color);
    void draw(const DrawArgs& args) override;
};

inline PanelGuide* getPanelGuide(Widget* host) { return host->getFirstDescendantOfType<PanelGuide>(); }

}
