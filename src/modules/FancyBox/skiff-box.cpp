#include "skiff-box.hpp"
#include "services/rack-help.hpp"
using namespace pachde;
namespace widgetry {

inline bool adjacent(const Rect& a, const Rect& b, float tolerance) {
    // assume boxes are lrtb
    if (b.pos.x <= a.pos.x + a.size.x + tolerance) {
        return ((b.pos.y == a.pos.y) // same row
            || (b.pos.y == a.pos.y + a.size.y) // next row
        );
    }
    return false;
}

void SkiffBox::make_skiff_boxes() {
    boxes.clear();
    auto modules = APP->scene->rack->getModules();
    float tolerance = options->separation * 15.f;
    std::sort(modules.begin(), modules.end(), widget_order_lrtb);

    auto it = modules.cbegin();
    if (options->floating_info) {
        const ModuleWidget* first = *it++;
        bool info = (0 == first->model->slug.compare("pachde-info"));
        if (!info) {
            boxes.push_back(first->box);
        } else {
            const ModuleWidget* second = *it;
            if (adjacent(first->box, second->box, 0)) {
                boxes.push_back(first->box.expand(second->box));
                it++;
            }
        }
    }
    for (; it != modules.cend(); it++) {
        Rect wbox = (*it)->box;
        if (boxes.empty()) {
            boxes.push_back(wbox);
        } else {
            bool merged{false};
            for (Rect& box: boxes) {
                if (adjacent(box, wbox, tolerance)) {
                    box = box.expand(wbox);
                    merged = true;
                    break;
                }
            }
            if (!merged) {
                if (options->floating_info) {
                    if (0 != (*it)->model->slug.compare("pachde-info")) {
                        boxes.push_back(wbox);
                    }
                } else {
                    boxes.push_back(wbox);
                }

            }
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
