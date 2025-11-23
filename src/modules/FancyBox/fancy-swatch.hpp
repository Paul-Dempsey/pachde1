#pragma once
#include <rack.hpp>
using namespace ::rack;
#include "services/colors.hpp"
#include "services/text.hpp"
#include "widgets/tip-widget.hpp"
using namespace pachde;
namespace widgetry {

struct FancySwatch : TipWidget {
    PackedColor color;

    FancySwatch() : color(0) {
        box.size = Vec(18.f, 10.f);
    }

    void set_color(PackedColor co) {
        color = co;
        NVGcolor nvg_color = fromPacked(co);
        float hue = Hue1(nvg_color);
        float saturation = Saturation1(nvg_color);
        float lightness = Lightness(nvg_color);
        float alpha = nvg_color.a;
        describe(hsla_string(hue, saturation, lightness, alpha));
    }

    FancySwatch(Rect r, PackedColor color) {
        box = r;
        set_color(color);
    }

    void draw (const DrawArgs& args) override {
        auto vg = args.vg;
        if (!isOpaque(color)) {
            nvgBeginPath(vg);
            nvgMoveTo(vg, 0.f, 0.f);
            nvgLineTo(vg, box.size.x, 0.f);
            nvgLineTo(vg, 0.f, box.size.y);
            nvgClosePath(vg);
            nvgFillColor(vg, RampGray(G_100));
            nvgFill(vg);

            nvgBeginPath(vg);
            nvgMoveTo(vg, 0.f, box.size.y);
            nvgLineTo(vg, box.size.x, 0.f);
            nvgLineTo(vg, box.size.x, box.size.y);
            nvgClosePath(vg);
            nvgFillColor(vg, RampGray(G_0));
            nvgFill(vg);
        }
        Rect r = box.zeroPos();
        BoxRect(vg, RECT_ARGS(r), RampGray(G_50), 1.f);
        if (packed_color::isVisible(color)) {
            Rect inside = r.shrink(Vec(.5,.5));
            FillRect(vg, RECT_ARGS(inside), fromPacked(color));
        }
    }
};

}