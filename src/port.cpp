#include "port.hpp"

namespace pachde {

void ColorPort::setTheme(Theme theme) {
    ThemeLite::setTheme(theme);

    sleeve = RampGray(G_25);
    switch (theme) {
        default:
        case Theme::Unset:
        case Theme::Light:
            collar = RampGray(G_35);
            edge   = RampGray(G_70);
            bevel  = RampGray(G_85);
            break;
        case Theme::Dark:
            collar = RampGray(G_25);
            edge   = RampGray(G_50);
            bevel  = RampGray(G_65);
            break;
        case Theme::HighContrast:
            collar = RampGray(G_25);
            edge   = RampGray(G_90);
            bevel  = RampGray(G_WHITE);
            break;
    }

}

void ColorPort::draw(const DrawArgs& args) {
    auto vg = args.vg;
    Circle(vg, 12.f, 12.f, 11.75f, collar);
    CircleGradient(vg, 12.f, 12.f, 11.75f, screen, shade);

    Circle(vg, 12.f, 12.f, 10.75f, edge);
    CircleGradient(vg, 12.f, 12.f, 10.75f, screen, shade);

    Circle(vg, 12.f, 12.f, 9.25f, bevel);
    CircleGradient(vg, 12.f, 12.f, 9.25f, shade, nvgRGBAf(1.,1.,1.,.4));

    Circle(vg, 12.f, 12.f, 8.5f, ring);
    Circle(vg, 12.f, 12.f, 5.25f, sleeve);
    Circle(vg, 12.f, 12.f, 4.f, RampGray(G_BLACK));
}

}