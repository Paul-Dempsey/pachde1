#pragma once
#include <rack.hpp>
using namespace ::rack;
#include "services/colors.hpp"
#include "services/rack-help.hpp"
#include "fader.hpp"
using namespace packed_color;

namespace pachde {

struct OverlayData
{
    OverlayPosition position{OverlayPosition::OnPanel};
    PackedColor color{0x4cbf723f};
    bool on{false};

    void init(const OverlayData* source) {
        position = source->position;
        color = source->color;
        on = source->on;
    }
    json_t * toJson();
    void fromJson(json_t * root);
    void reset();
};

struct PanelToneUi;

struct PanelOverlay : TransparentWidget {
    using Base = TransparentWidget;

    PanelToneUi* host{nullptr};
    OverlayData data;
    float fade{0.f};

    PanelOverlay(PanelToneUi* host);
    virtual ~PanelOverlay();
    void size_to_parent();
    void step() override;
    void draw(const DrawArgs& args) override;
};

inline PanelOverlay* getPanelOverlay(Widget* host) { return host->getFirstDescendantOfType<PanelOverlay>(); }

}