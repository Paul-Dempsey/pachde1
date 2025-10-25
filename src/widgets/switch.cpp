#include "switch.hpp"

namespace widgetry {

Switch::Switch()
{
    box.size.x = box.size.y = 15.f;
}

void Switch::setTheme(Theme theme)
{
    switch (theme) {
        default:
        case Theme::Unset:
        case Theme::Light:
            frame = RampGray(G_50);
            thumb = COLOR_BRAND;
            thumb_top = nvgRGB(0xcb, 0xdc, 0xe9);
            thumb_bottom = nvgRGB(0x2e, 0x51, 0x6b);
            background = RampGray(G_75);
            break;
        case Theme::Dark:
            frame = RampGray(G_50);
            thumb = RampGray(G_40);
            thumb_top = RampGray(G_70);
            thumb_bottom = RampGray(G_10);
            background = RampGray(G_30);
            break;
        case Theme::HighContrast:
            frame = RampGray(G_50);
            thumb = RampGray(G_60);
            thumb_top = RampGray(G_85);
            thumb_bottom = RampGray(G_35);
            background = RampGray(G_15);
            break;
    }
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

    auto bevel = 3.f;
    auto bevel_depth = 1.25f;
    auto ty = y + h - bevel;

    nvgBeginPath(vg);
    nvgMoveTo(vg, x, y + h);
    nvgLineTo(vg, x + w, y + h);
    nvgLineTo(vg, x + w, y + h - bevel);
    nvgBezierTo(vg, w * .2f, ty + bevel_depth, w * .8f, ty + bevel_depth, x, ty);
    nvgClosePath(vg);
    nvgFillColor(vg, thumb_bottom);
    nvgFill(vg);

    nvgBeginPath(vg);
    nvgMoveTo(vg, x, y);
    nvgLineTo(vg, x + w, y);
    nvgLineTo(vg, x + w, y + bevel);
    ty = y + bevel;
    nvgBezierTo(vg, w * .2f, ty - bevel_depth, w * .8f, ty - bevel_depth, x, ty);
    nvgClosePath(vg);
    nvgFillColor(vg, thumb_top);
    nvgFill(vg);
}

}