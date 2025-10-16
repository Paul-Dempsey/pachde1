#include "small_push.hpp"

namespace widgetry {

void SmallPush::setTheme(Theme theme)
{
    IBasicTheme::setTheme(theme);
    applyTheme(theme);
}

void SmallPush::applyTheme(Theme theme)
{
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
    float cx = 10.f;
    CircleGradient(vg, cx, cx, 10.f, collar1, collar2);
    Circle(vg, cx, cx, 9.5f, bezel);
    Circle(vg, cx, cx, 6.75f, ring);
    if (pressed) {
        CircleGradient(vg, cx, cx, 5.5f, bevel2, bevel1);
        CircleGradient(vg, cx, cx, 5.f, face2, face1);
    } else {
        CircleGradient(vg, cx, cx, 5.5f, bevel1, bevel2);
        CircleGradient(vg, cx, cx, 5.f, face1, face2);
    }
}

}