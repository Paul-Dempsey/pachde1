#include "components.hpp"

namespace pachde {

PicButton::PicButton(Theme theme)
{
    box.size.x = box.size.y = 15.f;
    line.a = 0.f; // force theme init
    setTheme(theme);
}

void PicButton::setTheme(Theme theme)
{
    if (isColorVisible(line) && theme == getTheme()) {
        return;
    }
    ThemeLite::setTheme(theme);
    switch (theme) {
        default:
        case Theme::Unset:
        case Theme::Light:
            line = COLOR_BRAND_LO;
            face = RampGray(G_90);
            break;
        case Theme::Dark:
            line = COLOR_BRAND_HI;
            face = RampGray(G_20);
            break;
        case Theme::HighContrast:
            line = RampGray(G_WHITE);
            face = RampGray(G_BLACK);
            break;
    }
}

void PicButton::draw(const DrawArgs &args) {
    OpaqueWidget::draw(args);

    auto fill = pressed ? line : face;
    auto stroke = pressed ? face : line;
    auto vg = args.vg;

    RoundRect(vg, 0.75f, 0.75f, 13.5f, 13.5f, fill, 2.25f);

    RoundBoxRect(vg, 0.75f, 0.75f, 13.5f, 13.5f, stroke, 2.25f);

    nvgBeginPath(vg);
    nvgMoveTo(vg, 1.f, 14.f);
    nvgLineTo(vg, 6.f, 8.f);
    nvgLineTo(vg, 14.f, 14.f);
    nvgStrokeColor(vg, stroke);
    nvgLineCap(vg, NVG_ROUND);
    nvgStroke(vg);

    nvgBeginPath(vg);
    nvgCircle(vg, 10.2f, 5.2f, 1.5f);
    nvgFillColor(vg, stroke);
    nvgFill(vg);

}

void PicButton::onButton(const event::Button& e)
{
    rack::OpaqueWidget::onButton(e);
    if (!(e.action == GLFW_PRESS
        && e.button == GLFW_MOUSE_BUTTON_LEFT
        && (e.mods & RACK_MOD_MASK) == 0)) {
        return;
    }
    pressed = true;
}
 
void PicButton::onDragEnd(const DragEndEvent & e)
{
    rack::OpaqueWidget::onDragEnd(e);
    if (e.button != GLFW_MOUSE_BUTTON_LEFT)
        return;
    if (clickHandler) {
        clickHandler();
    }
    pressed = false;
}

}