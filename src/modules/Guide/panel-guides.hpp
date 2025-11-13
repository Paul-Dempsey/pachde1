#pragma once
#include <rack.hpp>
using namespace ::rack;
#include "services/colors.hpp"
#include "services/svg-theme.hpp"
using namespace packed_color;

namespace pachde {
struct GuideUi;
struct GuideData;

struct PanelGuides : OpaqueWidget
{
    using Base = OpaqueWidget;

    GuideUi* ui{nullptr};
    GuideData* data{nullptr};

    PanelGuides() { box = Rect(Vec{0}, Vec{0}); }
    virtual ~PanelGuides();

    void step() override;
    void draw_panel(const DrawArgs& args, PackedColor color);
    void draw(const DrawArgs& args) override;
};

inline PanelGuides* getPanelGuide(Widget* host) { return host->getFirstDescendantOfType<PanelGuides>(); }

}
