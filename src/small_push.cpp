#include "small_push.hpp"

namespace pachde {

void SmallPush::setTheme(Theme theme) {
    ThemeLite::setTheme(theme);
    switch (theme) {
        default:
        case Theme::Unset:
        case Theme::Light:
            collar1 = RampGray(G_85);
            collar2 = RampGray(G_45);
            bezel   = RampGray(G_85);
            bevel1  = RampGray(G_100);
            bevel2  = RampGray(G_50);
            face1   = RampGray(G_90); 
            face2   = RampGray(G_60); 
            break;
        case Theme::Dark:
            collar1 = RampGray(G_60);
            collar2 = RampGray(G_30);
            bezel   = RampGray(G_25);
            bevel1  = RampGray(G_75);
            bevel2  = RampGray(G_25);
            face1   = RampGray(G_65); 
            face2   = RampGray(G_15); 
            break;
        case Theme::HighContrast:
            collar1 = RampGray(G_80);
            collar2 = RampGray(G_60);
            bezel   = RampGray(G_20);
            bevel1  = RampGray(G_75);
            bevel2  = RampGray(G_25);
            face1   = RampGray(G_80); 
            face2   = RampGray(G_10); 
            break;
    }
}

void SmallPush::draw(const DrawArgs& args)
{
    auto vg = args.vg;
    float center = 10.f;
    CircleGradient(vg, center, center, 10.f, collar1, collar2);
    Circle(vg, center, center, 9.5f, bezel);
    Circle(vg, center, center, 6.75f, ring);
    if (pressed) {
        CircleGradient(vg, center, center, 5.5f, bevel2, bevel1);
        CircleGradient(vg, center, center, 5.f, face2, face1);
    } else {
        CircleGradient(vg, center, center, 5.5f, bevel1, bevel2);
        CircleGradient(vg, center, center, 5.f, face1, face2);
    }
}

}