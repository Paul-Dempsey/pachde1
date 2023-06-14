#include "pic_button.hpp"

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
    gradient_stop_y = 13.5;
    gradient_stop_x = 7.5;
    switch (theme) {
        default:
        case Theme::Unset:
        case Theme::Light:
            line = COLOR_BRAND_LO;
            sky1 = nvgHSL(210./360., 0.3, .5);
            sky2 = nvgHSL(210./360., 0.5, .85);
            gradient_stop_y = 7.5;
            mountain = nvgHSL(120./360., 0.35, 0.4);
            moon = nvgHSL(210./360., .5, .5);
            break;
        case Theme::Dark:
            line = nvgHSL(210./360., .5, .5);
            sky1 = nvgHSL(210./360., 0.5, .2);
            sky2 = nvgHSL(30./360., 0.3, .60);
            gradient_stop_x = box.size.x *.67;
            mountain = nvgHSL(120./360., 0.35, 0.25);
            moon = nvgHSL(210./360., .25, .65);
            break;
        case Theme::HighContrast:
            line = nvgHSL(210./360., .5, .95);
            sky1 = nvgHSL(210./360., 0.5, .15);
            sky2 = nvgHSL(30./360., 0.2, .5);
            gradient_stop_y = 5;
            gradient_stop_x = box.size.x *.58;
            mountain = nvgHSL(120./360., 0.35, 0.3 );
            moon = nvgHSL(210./360., .5, .8);
            break;
    }
}

void PicButton::draw(const DrawArgs &args) {
    OpaqueWidget::draw(args);

    auto vg = args.vg;
    auto prev_theme = theme;
    if (pressed) {
        switch (theme) {
            default:
            case Theme::Unset:
            case Theme::Light:
                setTheme(Theme::Dark);
                break;
            case Theme::Dark:
            case Theme::HighContrast:
                setTheme(Theme::Light);
        }
    }

    nvgBeginPath(vg);
    nvgRoundedRect(vg, 0.75, 0.75, 13.5, 13.5, 2.25);
    auto grad = nvgLinearGradient(vg, 7.5, 1., gradient_stop_x, gradient_stop_y, sky1, sky2);
    nvgFillPaint(vg, grad);
    nvgFill(vg);

    RoundBoxRect(vg, 0.75, 0.75, 13.5, 13.5, line, 2.25);

    nvgBeginPath(vg);
    nvgMoveTo(vg, 1.5, 13.5);
    nvgLineTo(vg, 6., 8.);
    nvgLineTo(vg, 13.5, 13.5);
    nvgFillColor(vg, mountain);
    nvgFill(vg);

    nvgBeginPath(vg);
    nvgCircle(vg, 10.2, 5.25, 1.5);
    nvgFillColor(vg, moon);
    nvgFill(vg);

    if (prev_theme != theme) {
        setTheme(prev_theme);
    }
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