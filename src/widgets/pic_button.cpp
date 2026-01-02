#include "pic_button.hpp"

namespace pachde {


PicButton::PicButton()
{
    box.size.x = box.size.y = 24.f;
    line.a = 0.f; // force theme init
}

void PicButton::setTheme(Theme new_theme)
{
    theme = new_theme;
    gradient_stop_y = 17.5;
    gradient_stop_x = 10.f;
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
            line = RampGray(G_65); //nvgHSL(210./360., .5, .5);
            sky1 = nvgHSL(210./360., 0.5, .2);
            sky2 = nvgHSL(30./360., 0.3, .70);
            gradient_stop_x = box.size.x *.67;
            mountain = nvgHSL(120./360., 0.5, 0.25);
            moon = nvgHSL(210./360., .25, .7);
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
        switch (prev_theme) {
            default:
            case Theme::Unset:
            case Theme::Light:
                setTheme(Theme::Dark);
                break;
            case Theme::Dark:
            case Theme::HighContrast:
                setTheme(Theme::Light);
                break;
        }
    }

    nvgBeginPath(vg);
    nvgRoundedRect(vg, 0.75, 0.75, box.size.x - 1.5f, box.size.y - 1.5f, 2.25);
    auto grad = nvgLinearGradient(vg, box.size.x / 2.f, 1., gradient_stop_x, gradient_stop_y, sky1, sky2);
    nvgFillPaint(vg, grad);
    nvgFill(vg);

    auto horizon = box.size.y - box.size.y *.26f;
    auto peak_x = box.size.x * .24f;
    auto peak_y = box.size.y * .45f;
    nvgBeginPath(vg);
    nvgMoveTo(vg, 1.f, box.size.y - 1.f);
    nvgLineTo(vg, 1.f, horizon);
    nvgLineTo(vg, peak_x, peak_y);
    horizon += box.size.y *.05;
    nvgLineTo(vg, box.size.x - 1.f, horizon);
    nvgLineTo(vg, box.size.x - 1.f, box.size.y - 1.f);
    nvgFillColor(vg, mountain);
    nvgFill(vg);

    RoundBoxRect(vg, 0.75, 0.75, box.size.x - 1.5f, box.size.y - 1.5f, line, 2.25);

    nvgBeginPath(vg);
    nvgCircle(vg, box.size.x - (box.size.x * 0.3f), (box.size.y * .3f), box.size.y / 10.f);
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
    destroyTooltip();
    if (clickHandler) {
        clickHandler(ctrl, shift);
    }
    pressed = false;
}

void PicButton::createTooltip() {
    if (!settings::tooltips)
        return;
    if (tip) return;
    if (tip_text.empty()) return;
    auto newTip = new Tooltip();
    newTip->text = tip_text;
    APP->scene->addChild(newTip);
    tip = newTip;
}

void PicButton::destroyTooltip() {
    if (tip) {
        tip->requestDelete();
        tip = nullptr;
    }
}

}