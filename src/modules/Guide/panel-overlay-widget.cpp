#include "panel-overlay-widget.hpp"
#include "Guide.hpp"
namespace pachde {

PanelGuide::~PanelGuide()
{
    if (ui) {
        ui->onDestroyGuide();
    }
}

void PanelGuide::step() {
    Base::step();
    auto p = getParent();
    if (p) {
        box.size = p->box.size;
    }
}
void PanelGuide::draw_panel(const DrawArgs& args, PackedColor color) {
    auto vg = args.vg;
    nvgBeginPath(vg);
    nvgRect(vg, 0, 0, box.size.x, box.size.y);
    nvgFillColor(vg, fromPacked(color));
    nvgFill(vg);
}

void draw_guide(PanelGuide*self, NVGcontext* vg, std::shared_ptr<GuideLine> guide) {
    if (!guide->color || (0.f == guide->width)) return;

    //float theta = nvgDegToRad(guide->angle);
    float angle = guide->angle;
    if (angle >= 180.f) angle = 0.f;
    NVGcolor co = fromPacked(guide->color);
    float width = guide->width;
    if (0.f == guide->angle) {
        // horizontal
        bool wrapped = guide->origin.y < 0;
        float dir = wrapped ? -1.f : 1.f;
        float y = wrapped ? 380.f + guide->origin.y : guide->origin.y;
        nvgBeginPath(vg);
        nvgRect(vg, guide->origin.x, y, self->box.size.x, width);
        nvgFillColor(vg, co);
        nvgFill(vg);
        if (guide->repeat > 0.1f) {
            for (y += dir * guide->repeat; in_range(y, 0.f, self->box.size.y); y += dir * guide->repeat) {
                nvgBeginPath(vg);
                nvgRect(vg, guide->origin.x, y, self->box.size.x, width);
                nvgFillColor(vg, co);
                nvgFill(vg);
            }
        }
    } else if (90.f == guide->angle) {
        // vertical
        bool wrapped = guide->origin.x < 0;
        float dir = wrapped ? -1.f : 1.f;
        float x = wrapped ? self->box.size.x + guide->origin.x : guide->origin.x;

        nvgBeginPath(vg);
        nvgRect(vg, x, guide->origin.y, width, self->box.size.y);
        nvgFillColor(vg, co);
        nvgFill(vg);
        if (guide->repeat > 0.1f) {
            for (x += dir * guide->repeat; in_range(x, 0.f, self->box.size.x); x += dir* guide->repeat) {
                nvgBeginPath(vg);
                nvgRect(vg, x, guide->origin.y, width, self->box.size.y);
                nvgFillColor(vg, co);
                nvgFill(vg);
            }
        }
    } else {
        //TODO
    }
}

void PanelGuide::draw(const DrawArgs& args) {
    //Base::draw(args);
    auto vg = args.vg;
    if (data) {
        draw_panel(args, data->co_overlay);

        for (auto guide: data->guides) {
            if (guide->width > 0) {
                draw_guide(this, vg, guide);
            }
        }
    } else {
        draw_panel(args, colors::PortRed);
    }
}


}