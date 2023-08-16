#include "info_symbol.hpp"

namespace pachde {

void InfoSymbol::setTheme(Theme theme)
{
    IBasicTheme::setTheme(theme);
    switch (theme) {
        default:
        case Theme::Unset:
        case Theme::Light:
            color = RampGray(G_25);
            break;
        case Theme::Dark:
            color = RampGray(G_65);
            break;
        case Theme::HighContrast:
            color = RampGray(G_95);
            break;
    }
}

void InfoSymbol::draw(const DrawArgs& args)
{
    auto vg = args.vg;
    nvgBeginPath(vg);
    nvgCircle(vg, 7.5, 7.5, 6.45);
    nvgStrokeColor(vg, color);
    nvgStrokeWidth(vg, 1.);
    nvgStroke(vg);

    Circle(vg, 7.5, 4.5, 1., color);

    nvgBeginPath(vg);
    nvgMoveTo(vg, 5.9, 7.3);
    nvgLineTo(vg, 5.9, 6.14);
    nvgLineTo(vg, 8.28, 6.14);
    nvgLineTo(vg, 8.28, 10.02);
    nvgLineTo(vg, 9.03, 10.02);
    nvgLineTo(vg, 9.03, 11.18);
    nvgLineTo(vg, 5.9, 11.18);
    nvgLineTo(vg, 5.9, 10.);
    nvgLineTo(vg, 6.7, 10.);
    nvgLineTo(vg, 6.7, 7.2);
    nvgClosePath(vg);
    nvgFillColor(vg, color);
    nvgFill(vg);
}

}