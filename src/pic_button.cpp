#include "components.hpp"

namespace pachde {

PicButton::PicButton(Theme t)
{
    box.size.x = box.size.y = 15.0f;
    setTheme(t);
}

void PicButton::setTheme(Theme t)
{
    assert(Theme::Unset != t);
    switch (t) {
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
            face = COLOR_NONE;
            break;
    }
}

void PicButton::draw(const DrawArgs &args) {
    OpaqueWidget::draw(args);

    auto fill = pressed ? line : face;
    auto stroke = pressed ? face : line;
    auto vg = args.vg;

    if (fill.a > 0.0f) {
        RoundRect(vg, 0.75, 0.75, 13.5, 13.5, fill, 2.25);
    }

    if (stroke.a > 0.0f) {
        RoundBoxRect(vg, 0.75, 0.75, 13.5, 13.5, stroke, 2.25);

        nvgBeginPath(vg);
        nvgMoveTo(vg, 1.0f, 14.0f);
        nvgLineTo(vg, 6.0f, 8.0f);
        nvgLineTo(vg, 14.0f, 14.0f);
        nvgStrokeColor(vg, stroke);
        nvgLineCap(vg, NVG_ROUND);
        nvgStroke(vg);

        nvgBeginPath(vg);
        nvgCircle(vg, 10.2f, 5.2f, 1.5f);
        nvgFillColor(vg, stroke);
        nvgFill(vg);
    }

}

void PicButton::center(Vec pos)
{
    box.pos = pos.minus(box.size.div(2));
}

void PicButton::onClick(std::function<void(void)> callback)
{
    clickHandler = callback;
}
 
void PicButton::onButton(const event::Button& e)
{
    rack::OpaqueWidget::onButton(e);
    if (!(e.action == GLFW_PRESS && e.button == GLFW_MOUSE_BUTTON_LEFT && (e.mods & RACK_MOD_MASK) == 0)) {
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