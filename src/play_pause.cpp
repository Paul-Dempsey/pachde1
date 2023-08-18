#include "play_pause.hpp"

namespace pachde {

PlayPauseButton::PlayPauseButton()
{
    pressed = false;
    box.size.x = box.size.y = 24.f;
}

void PlayPauseButton::applyTheme(Theme theme)
{
    collar = RampGray(G_35);
    edge = RampGray(G_20);
    face = COLOR_BRAND;
    face2 = COLOR_BRAND_HI;
    symbol = RampGray(G_WHITE);

    switch (theme)
    {
        default:
        case Theme::Unset:
        case Theme::Light:
            collar = RampGray(G_50);
            edge = RampGray(G_75);
            break;
        case Theme::Dark:
            face = COLOR_BRAND_LO;
            face2 = COLOR_BRAND;
            break;
        case Theme::HighContrast:
            collar = RampGray(G_WHITE);
            edge = RampGray(G_05);
            face = RampGray(G_15);
            face2 = RampGray(G_35);
            break;
    }
}

void PlayPauseButton::setTheme(Theme theme)
{
    IBasicTheme::setTheme(theme);
    applyTheme(theme);
}

void PlayPauseButton::draw(const DrawArgs &args)
{
    auto vg = args.vg;
    auto m = box.size.x / 2.f;
    auto u = box.size.x / 4.f;
    auto q = box.size.x / 8.f;
    auto sw = m/8.f;

    Circle(vg, m, m, m,  collar);
    Circle(vg, m, m, m - .8f, edge);

    if (pressed) {
        CircleGradient(vg, m, m, .75f * m, face, face2);
        nvgBeginPath(vg);
        nvgMoveTo(vg, u + q, u);
        nvgLineTo(vg, box.size.x - u, m);
        nvgLineTo(vg, u + q, box.size.y - u);
        nvgFillColor(vg, symbol);
        nvgFill(vg);
    } else {
        sw *= 1.5f;
        CircleGradient(vg, m, m, .75f * m, face2, face);
        nvgStrokeWidth(vg, sw);
        nvgStrokeColor(vg, symbol);

        nvgBeginPath(vg);
        nvgMoveTo(vg, m - sw, u);
        nvgLineTo(vg, m - sw, box.size.y - u);
        nvgStroke(vg);

        nvgBeginPath(vg);
        nvgMoveTo(vg, m + sw, u);
        nvgLineTo(vg, m + sw, box.size.y - u);
        nvgStroke(vg);
    }
}

void PlayPauseButton::onDragEnd(const DragEndEvent & e)
{
    pressed = !pressed;
    rack::Switch::onDragEnd(e);
    if (e.button != GLFW_MOUSE_BUTTON_LEFT)
        return;
    if (clickHandler) {
        clickHandler();
    }
}

}