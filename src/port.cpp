#include "port.hpp"

namespace pachde {

void ColorPort::setTheme(Theme theme) {
    ThemeLite::setTheme(theme);

    sleeve = RampGray(G_25);
    switch (theme) {
        default:
        case Theme::Unset:
        case Theme::Light:
            collar1 = RampGray(G_50);
            collar2 = RampGray(G_35);
            edge    = RampGray(G_75);
            bevel1  = RampGray(G_65);
            bevel2  = RampGray(G_90);
            break;
        case Theme::Dark:
            collar1 = RampGray(G_50);
            collar2 = RampGray(G_35);
            edge    = RampGray(G_25);
            bevel1  = RampGray(G_18);
            bevel2  = RampGray(G_40);
            break;
        case Theme::HighContrast:
            collar1 = RampGray(G_50);
            collar2 = RampGray(G_35);
            edge    = RampGray(G_50);
            bevel1  = RampGray(G_65);
            bevel2  = RampGray(G_90);
            break;
    }

}

void ColorPort::draw(const DrawArgs& args) {
    auto vg = args.vg;
    CircleGradient(vg, 12.f, 12.f, 11.75f, collar1, collar2);
    Circle(vg, 12.f, 12.f, 10.5f, edge);
    CircleGradient(vg, 12.f, 12.f, 9.f, bevel1, bevel2);
    Circle(vg, 12.f, 12.f, 8.f, ring);
    Circle(vg, 12.f, 12.f, 5.25f, sleeve);
    Circle(vg, 12.f, 12.f, 4.f, RampGray(G_BLACK));
}

}