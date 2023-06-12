#include "components.hpp"

namespace pachde {

Switch::Switch(Theme theme)
{
    box.size.x = box.size.y = 15.f;
    setTheme(theme);
}

void Switch::setTheme(Theme theme)
{
    background = PanelBackground(theme);
    switch (theme) {
        default:
        case Theme::Unset:
        case Theme::Light:
            frame = RampGray(G_50);
            thumb = COLOR_BRAND;
            thumb_top = nvgRGB(0x9f, 0xc3, 0xdf);
            thumb_bottom = nvgRGB(0x2e, 0x51, 0x6b);
            break;
        case Theme::Dark:
            frame = RampGray(G_50);
            thumb = RampGray(G_40);
            thumb_top = RampGray(G_70);
            thumb_bottom = RampGray(G_10);
            break;
        case Theme::HighContrast:
            frame = RampGray(G_WHITE);
            thumb = RampGray(G_80);
            thumb_top = RampGray(G_WHITE);
            thumb_bottom = RampGray(G_25);
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
    bool horizontal = box.size.x >= box.size.y;
    auto vg = args.vg;
    FillRect(vg, 0.f, 0.f, box.size.x, box.size.y, background);
    BoxRect(vg, 0.f, 0.f, box.size.x, box.size.y, frame);
    auto w = horizontal ? box.size.x / units : box.size.x + 0.5f;
    auto h = horizontal ? box.size.y + 0.5f: box.size.y / units;
    auto x = horizontal ? w * value : 0.f;
    auto y = horizontal ? 0.f : (box.size.y - h) - (h * value);
    FillRect(vg, x, y, w, h, thumb);
    Line(vg, x, y, x + w, y, thumb_top);
    Line(vg, x, y + h, x + w, y + h, thumb_bottom);
}

}