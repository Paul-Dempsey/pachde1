#include "port.hpp"

namespace widgetry {

void ColorPort::applyTheme(Theme theme)
{
    sleeve = RampGray(G_20);
    tube    = RampGray(G_05);
    switch (theme) {
        default:
        case Theme::Unset:
        case Theme::Light:
            collar1 = RampGray(G_60);
            collar2 = RampGray(G_30);
            bezel   = RampGray(G_90);
            bevel1  = RampGray(G_75);
            bevel2  = RampGray(G_90);
            break;
        case Theme::Dark:
            collar1 = RampGray(G_50);
            collar2 = RampGray(G_30);
            bezel   = RampGray(G_25);
            bevel1  = RampGray(G_75);
            bevel2  = RampGray(G_90);
            break;
        case Theme::HighContrast:
            collar1 = RampGray(G_40);
            collar2 = RampGray(G_25);
            bezel   = RampGray(G_85);
            bevel1  = RampGray(G_75);
            bevel2  = RampGray(G_90);
            break;
    }
}

void ColorPort::setTheme(Theme theme)
{
    IBasicTheme::setTheme(theme);
    applyTheme(theme);
}

void ColorPort::draw(const DrawArgs& args)
{
    auto vg = args.vg;
    float center = 11.f;
    CircleGradient(vg, center, center, 11.75f, collar1, collar2);
    Circle(vg, center, center, 11.25f, bezel);
    Circle(vg, center, center, 8.5f, tube);
    CircleGradient(vg, center, center, 7.75f, bevel1, bevel2);
    Circle(vg, center, center, 6.6f, ring);
    Circle(vg, center, center, 3.7f, sleeve);
    Circle(vg, center, center, 3.f, RampGray(G_BLACK));
}

}