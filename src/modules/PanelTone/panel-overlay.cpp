#include "panel-overlay.hpp"
#include "PanelTone.hpp"
#include "services/json-help.hpp"

namespace pachde {

json_t *OverlayData::toJson()
{
    char hex[10];
    json_t* root = json_object();
    set_json(root, "position", (position == OverlayPosition::OnPanel) ? "panel" : "widget");
    packed_color::hexFormat(color, 10, hex);
    set_json(root, "color", hex);
    set_json(root, "on", on);
    return root;
}

void OverlayData::fromJson(json_t *root) {
    auto spec = get_json_string(root, "position");
    if (!spec.empty()) {
        if (0 == spec.compare("panel")) {
            position = OverlayPosition::OnPanel;
        } else if (0 == spec.compare("widget")) {
            position = OverlayPosition::OnTop;
        } else {
            position = OverlayPosition::OnPanel;
        }
    }
    spec = get_json_string(root, "color");
    if (!spec.empty()) {
        color = parseColor(spec.c_str(), color);
    }
    on = get_json_bool(root, "on", on);
}

void OverlayData::reset() {
    position = OverlayPosition::OnPanel;
    color = 0x4cbf723f;
    on = false;
}

// ----  PanelOverlay ---------------------------

PanelOverlay::PanelOverlay(PanelToneUi *host)  : host(host) {
    data.init(host->data);
}

PanelOverlay::~PanelOverlay() {
    if (host) host->onDestroyPanelOverlay(this);
}

void PanelOverlay::size_to_parent() {
    auto p = getParent();
    if (p) {
        box.size = p->box.size;
    }
}

void PanelOverlay::step() {
    Base::step();
    size_to_parent();
    //step_fade();
}

void PanelOverlay::draw(const DrawArgs &args) {
    if (!box.size.isFinite()) { size_to_parent(); }
    if (data.on && data.color) {
        auto vg = args.vg;
        auto co = fromPacked(data.color);
        //co.a *= fader.fade;
        co.a *= fade;
        if (co.a > 0.f) {
            nvgBeginPath(vg);
            nvgRect(vg, 0, 0, box.size.x, box.size.y);
            nvgFillColor(vg, co);
            nvgFill(vg);
        }
    }
    Base::draw(args);
}


}