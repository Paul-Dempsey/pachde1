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
        nvgBeginPath(vg);
        nvgMoveTo(vg, 0.f, guide->origin.y);
        nvgLineTo(vg, self->box.size.x, guide->origin.y);
        nvgLineTo(vg, self->box.size.x, guide->origin.y + width);
        nvgLineTo(vg, 0.f, guide->origin.y + width);
        nvgClosePath(vg);
        nvgFillColor(vg, co);
        nvgFill(vg);
        if (guide->repeat > 0.f) {
            for (float y = guide->origin.y + guide->repeat; y < self->box.size.y; y += guide->repeat) {
                nvgBeginPath(vg);
                nvgMoveTo(vg, 0.f, y);
                nvgLineTo(vg, self->box.size.x, y);
                nvgLineTo(vg, self->box.size.x, y + width);
                nvgLineTo(vg, 0.f, y + width);
                nvgClosePath(vg);
                nvgFillColor(vg, co);
                nvgFill(vg);
            }
        }
    } else if (90.f == guide->angle) {
        // vertical
        nvgBeginPath(vg);
        nvgMoveTo(vg, guide->origin.x, 0.f);
        nvgLineTo(vg, guide->origin.x, self->box.size.y);
        nvgLineTo(vg, guide->origin.x + width, self->box.size.y);
        nvgLineTo(vg, guide->origin.x + width, 0.f);
        nvgClosePath(vg);
        nvgFillColor(vg, co);
        nvgFill(vg);
        if (guide->repeat > 0.f) {
            for (float x = guide->origin.x + guide->repeat; x < self->box.size.x; x += guide->repeat) {
               nvgBeginPath(vg);
                nvgMoveTo(vg, x, 0.f);
                nvgLineTo(vg, x, self->box.size.y);
                nvgLineTo(vg, x + width, self->box.size.y);
                nvgLineTo(vg, x + width, 0.f);
                nvgClosePath(vg);
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
        draw_panel(args, colors::White);
    }
}


}