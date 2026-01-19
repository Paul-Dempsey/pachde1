#include "skiff-box.hpp"
#include "services/rack-help.hpp"
#include "widgets/widgetry.hpp"

using namespace pachde;
namespace widgetry {

inline bool adjacent(const Rect& a, const Rect& b, float tolerance, bool horizontal) {
    float tx = horizontal ? tolerance*.5f : 0.f;
    float ty = horizontal ? 0.f : tolerance*.5f;
    Vec growby = Vec(tx, ty);
    Rect a1 = a.grow(growby);
    Rect b1 = b.grow(growby);
    return a1.intersects(b1);
}

void SkiffBox::make_skiff_boxes() {
    boxes.clear();
    auto modules = APP->scene->rack->getModules();
    float tolerance = options->separation * 15.f;
    std::sort(modules.begin(), modules.end(), widget_order_lrtb);

    std::vector<Rect> row_boxes;
    for (const ModuleWidget* mw: modules) {
        auto wbox = mw->box;
        if (row_boxes.empty()) {
            row_boxes.push_back(wbox);
        } else {
            bool merged{false};
            for (Rect& box: row_boxes) {
                if (adjacent(box, wbox, tolerance, true)) {
                    box = box.expand(wbox);
                    merged = true;
                    break;
                }
            }
            if (!merged) {
                row_boxes.push_back(wbox);
            }
        }
    }
    for (const Rect& row_box: row_boxes) {
        if (boxes.empty()) {
            boxes.push_back(row_box);
        } else {
            bool merged{false};
            for (Rect& box: boxes) {
                if (adjacent(box, row_box, tolerance, false)) {
                    box = box.expand(row_box);
                    merged = true;
                    break;
                }
            }
            if (!merged) {
                boxes.push_back(row_box);
            }
        }
    }
    row_boxes.clear();

    // un-skiff isolated boxes
    for (const ModuleWidget* mw: modules) {
        auto it = boxes.begin();
        for (; it != boxes.end(); it++) {
            if ((*it).equals(mw->box)) break;
        }
        if (it != boxes.end()) {
            boxes.erase(it);
        }
    }
}

void SkiffBox::draw_shadow(NVGcontext *vg, const Rect &base_box){
    Rect shadow{base_box.grow(Vec(4, 4))};
    shadow.pos = shadow.pos.plus(Vec(3.f,5.f));
    Rect outer{shadow.grow(Vec(30, 30))};
    nvgBeginPath(vg);
    nvgRect(vg, RECT_ARGS(outer));
    nvgFillPaint(vg, nvgBoxGradient(
        vg,
        RECT_ARGS(shadow),
        30.f, 30.f,
        nvgRGBAf(0,0,0,.8f),
        nvgRGBAf(0,0,0,0)
    ));
    nvgFill(vg);
}

void SkiffBox::draw(const DrawArgs &args) {
    make_skiff_boxes();
    if (boxes.size() < 1) return;
    NVGcolor co_edge = fromPacked(options->edge_color);
    NVGcolor co_face = fromPacked(options->bezel_color);
    NVGcolor co_inside = fromPacked(options->inside_color);
    auto vg = args.vg;
    float width = options->bezel_width + options->edge_width;
    if (width <= 0.f) return;
    for (const Rect& r: boxes) {
        if (options->shadow) draw_shadow(vg, r);
        if (co_inside.a > 0) {
            FillRect(vg,  r.pos.x, r.pos.y, r.size.x, r.size.y, co_inside);
        }
        if (options->edge_width > 0.f) {
            FittedBoxRect(vg,
                r.pos.x - width,
                r.pos.y - width,
                r.size.x + 2*width,
                r.size.y + 2*width,
                co_edge, Fit::Inside, options->edge_width
            );
        }
        if (options->bezel_width > 0.f) {
            FittedBoxRect(vg,
                r.pos.x - options->bezel_width,
                r.pos.y - options->bezel_width,
                r.size.x + 2*options->bezel_width,
                r.size.y + 2*options->bezel_width,
                co_face, Fit::Inside, options->bezel_width
            );
        }
    }
}

}
