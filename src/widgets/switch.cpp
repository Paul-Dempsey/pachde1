#include "switch.hpp"

namespace widgetry {

Switch::Switch() {
    box.size.x = box.size.y = 15.f;
}

bool Switch::applyTheme(std::shared_ptr<SvgTheme> theme) {
    PackedColor co;;
    background   = theme->getFillColor(co, "sw-bg",      true) ? fromPacked(co) : RampGray(G_30);
    frame        = theme->getFillColor(co, "sw-frame",   true) ? fromPacked(co) : RampGray(G_50);
    thumb        = theme->getFillColor(co, "sw-thumb",   true) ? fromPacked(co) : RampGray(G_40);
    thumb_top    = theme->getFillColor(co, "sw-thumb-t", true) ? fromPacked(co) : RampGray(G_70);
    thumb_bottom = theme->getFillColor(co, "sw-thumb-b", true) ? fromPacked(co) : RampGray(G_10);
    return false;
}

void Switch::initParamQuantity()
{
    rack::Switch::initParamQuantity();
    auto pq = getParamQuantity();
    if (pq) {
        units = 1 + static_cast<int>(std::round(pq->getMaxValue() - pq->getMinValue()));
        assert(units >= 2);
        value = static_cast<int>(std::round(pq->getValue() - pq->getMinValue()));
        assert(value < units);
    }
}

void Switch::onChange(const ChangeEvent& e)
{
    auto pq = getParamQuantity();
    value = static_cast<int>(std::round(pq->getValue() - pq->getMinValue()));
    rack::Switch::onChange(e);
}

void Switch::draw(const DrawArgs &args)
{
    bool vertical = box.size.y >= box.size.x;
    auto vg = args.vg;

    FillRect(vg, 0.f, 0.f, box.size.x, box.size.y, background);
    BoxRect(vg, 0.f, 0.f, box.size.x, box.size.y, frame);

    auto w = vertical ? box.size.x : box.size.x / units;
    auto h = vertical ? box.size.y / units : box.size.y;
    auto x = vertical ? 0.f : w * value;
    auto y = vertical ? (box.size.y - h) - (h * value) : 0.f;

    FillRect(vg, x, y, w, h, thumb);

    auto bevel = h *.25f;
    auto bevel_depth = h*.06f;
    auto ty = y + h - (bevel + bevel_depth);

    nvgBeginPath(vg);
    nvgMoveTo(vg, x, y + h);
    nvgLineTo(vg, x + w, y + h);
    nvgLineTo(vg, x + w, ty);
    nvgBezierTo(vg, x + w * .2f, ty + bevel_depth, x + w * .8f, ty + bevel_depth, x, ty);
    nvgClosePath(vg);
    nvgFillColor(vg, thumb_bottom);
    nvgFill(vg);

    ty = y + bevel;
    nvgBeginPath(vg);
    nvgMoveTo(vg, x, y);
    nvgLineTo(vg, x + w, y);
    nvgLineTo(vg, x + w, ty);
    nvgBezierTo(vg, x + w * .2f, ty - bevel_depth, x + w * .8f, ty - bevel_depth, x, ty);
    nvgClosePath(vg);
    nvgFillColor(vg, thumb_top);
    nvgFill(vg);
}

}