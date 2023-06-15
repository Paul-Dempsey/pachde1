#include "play_pause.hpp"

namespace pachde {

PlayPauseButton::PlayPauseButton(Theme theme)
{
    box.size.x = box.size.y = 20.f;
    setTheme(theme);
}

void PlayPauseButton::setTheme(Theme theme)
{
    collar = RampGray(G_35);
    edge = RampGray(G_20);
    face = COLOR_BRAND;
    symbol = RampGray(G_WHITE);

    switch (theme)
    {
        default:
        case Theme::Unset:
        case Theme::Light:
            collar = RampGray(G_50);
            edge = RampGray(G_65);
            break;
        case Theme::Dark:
            face = COLOR_BRAND_LO;
            break;
        case Theme::HighContrast:
            collar = RampGray(G_05);
            edge = RampGray(G_85);
            face = RampGray(G_20);
            break;
    }

}

void PlayPauseButton::draw(const DrawArgs &args)
{
    auto vg = args.vg;
    Circle(vg, 10.f, 10.f, 10.f,  collar);
    Circle(vg, 10.f, 10.f, 8.75, edge);
    Circle(vg, 10.f, 10.f, 7.5,  face);

    if (pressed) {
        CircleGradient(vg, 10.f, 10.f, 7.5, shade, screen);
        nvgBeginPath(vg);
        nvgMoveTo(vg, 7.f, 5.f);
        nvgLineTo(vg, 15.f, 10.f);
        nvgLineTo(vg, 7.f, 15.f);
        nvgFillColor(vg, symbol);
        nvgFill(vg);
    } else {
        CircleGradient(vg, 10.f, 10.f, 7.5, screen, shade);
        nvgStrokeWidth(vg, 2.f);
        nvgStrokeColor(vg, symbol);
        nvgBeginPath(vg);
        nvgMoveTo(vg, 8.25f, 5.f);
        nvgLineTo(vg, 8.25f, 15.f);
        nvgStroke(vg);
        nvgBeginPath(vg);
        nvgMoveTo(vg, 11.75f, 5.f);
        nvgLineTo(vg, 11.75f, 15.f);
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