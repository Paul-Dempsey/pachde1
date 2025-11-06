#include "color-widgets.hpp"

namespace widgetry {

void AlphaWidget::onEnter(const EnterEvent &e) {
    OpaqueWidget::onEnter(e);
    glfwSetCursor(APP->window->win, glfwCreateStandardCursor(GLFW_CROSSHAIR_CURSOR));
}
void AlphaWidget::onLeave(const LeaveEvent &e) {
    OpaqueWidget::onLeave(e);
    glfwSetCursor(APP->window->win, NULL);
}

void AlphaWidget::onDragMove(const DragMoveEvent& e) {
    OpaqueWidget::onDragMove(e);
    drag_pos = drag_pos.plus(e.mouseDelta.div(getAbsoluteZoom()));

    setOpacity(vertical() ? (drag_pos.y / box.size.y) : (drag_pos.x / box.size.x));
    if (clickHandler) {
        clickHandler(alpha);
    }
}

void AlphaWidget::onButton(const ButtonEvent& e) {
    OpaqueWidget::onButton(e);
    if (!(e.action == GLFW_PRESS && e.button == GLFW_MOUSE_BUTTON_LEFT && (e.mods & RACK_MOD_MASK) == 0)) {
        return;
    }
    drag_pos = e.pos;
    e.consume(this);
}

void drawCheckers(const rack::widget::Widget::DrawArgs& args, float x, float y, float width, float height) {
    auto vg = args.vg;

    float dx = 5.f;
    float m = std::min(width, height);
    if (m < 15.f) {
        dx = m / 3.f;
    }

    nvgScissor(vg, x, y, width, height);
    nvgBeginPath(vg);
    int r = 0;
    for (float iy = y; iy < y + height; iy += dx, r++) {
        bool g = (0 == (r & 1));
        for (float ix = x; ix < x + width; ix += dx) {
            if (g) nvgRect(vg, ix, iy, dx, dx);
            g = !g;
        }
    }
    nvgFillColor(vg, nvgRGBAf(.5,.5f,.5, 1.f));
    nvgFill(vg);
    nvgResetScissor(vg);
}

void AlphaWidget::draw(const DrawArgs& args) {
    auto vg = args.vg;

    auto co_white = nvgRGBAf(1.f, 1.f, 1.f, 1.f);
    nvgBeginPath(vg);
    nvgRect(vg, 0, 0, box.size.x, box.size.y);
    nvgFillColor(vg, co_white);
    nvgFill(vg);

    drawCheckers(args, 0, 0, box.size.x, box.size.y);

    nvgBeginPath(vg);
    int r = 0;
    for (float y = 0; y < box.size.y; y += 5.f, r++) {
        bool g = (0 == (r & 1));
        for (float x = 0; x < box.size.x; x += 5.f) {
            if (g) nvgRect(vg, x, y, 5.f, 5.f);
            g = !g;
        }
    }
    nvgFillColor(vg, nvgRGBAf(.5,.5f,.5, 1.f));
    nvgFill(vg);

    nvgBeginPath(vg);
    nvgRect(vg, 0, 0, box.size.x, box.size.y);
    auto co_nada = nvgRGBAf(1.f,1.f,1.f,0.f);
    NVGpaint paint = vertical()
        ? nvgLinearGradient(vg, 0, 0, 0, box.size.y, co_nada, co_white)
        : nvgLinearGradient(vg, 0, 0, box.size.x, 0, co_nada, co_white);
    nvgFillPaint(vg, paint);
    nvgFill(vg);

    if (vertical()) {
        auto y = alpha * box.size.y;
        Line(vg, 0.f, y, box.size.x, y, RampGray(G_85), .75f);
        Line(vg, 0.f, y + .75f, box.size.x, y + .75f, RampGray(G_10), .75f);
    } else {
        auto x = alpha * box.size.x;
        Line(vg, x, 0.f, x, box.size.y, RampGray(G_85), .75f);
        Line(vg, x + .75f, 0.f, x + .75f, box.size.y, RampGray(G_10), .75f);
    }
}

//
// HueWidget
//

HueWidget::~HueWidget() {
    if (ramp) {
        auto pic = ramp->getPic();
        delete ramp;
        delete pic;
    }
}

cachePic* HueWidget::getHueRamp() {
    if (nullptr == ramp) {
        ramp = new cachePic(CreateHueRamp(box.size.x, box.size.y, vertical()));
    }
    return ramp;
}

void HueWidget::onContextCreate(const ContextCreateEvent& e)
{
    OpaqueWidget::onContextCreate(e);
    if (ramp) {
        ramp->onContextCreate(e);
    }
}

void HueWidget::onContextDestroy(const ContextDestroyEvent& e)
{
    if (ramp) {
        ramp->onContextDestroy(e);
    }
    OpaqueWidget::onContextDestroy(e);
}

void HueWidget::onEnter(const EnterEvent &e)
{
    OpaqueWidget::onEnter(e);
    glfwSetCursor(APP->window->win, glfwCreateStandardCursor(GLFW_CROSSHAIR_CURSOR));
}

void HueWidget::onLeave(const LeaveEvent &e)
{
    OpaqueWidget::onLeave(e);
    glfwSetCursor(APP->window->win, NULL);
}

void HueWidget::onDragMove(const DragMoveEvent& e)
{
    OpaqueWidget::onDragMove(e);
    drag_pos = drag_pos.plus(e.mouseDelta.div(getAbsoluteZoom()));

    hue = vertical() ? drag_pos.y / box.size.y : drag_pos.x / box.size.x;
    if (clickHandler)
        clickHandler(hue);
}

void HueWidget::onButton(const ButtonEvent& e)
{
    OpaqueWidget::onButton(e);
    if (!(e.action == GLFW_PRESS && e.button == GLFW_MOUSE_BUTTON_LEFT && (e.mods & RACK_MOD_MASK) == 0)) {
        return;
    }
    e.consume(this);
    drag_pos = e.pos;
}

void HueWidget::draw(const DrawArgs & args)
{
    auto vg = args.vg;
    auto image_handle = getHueRamp()->getHandle(vg);
    if (image_handle) {
        nvgBeginPath(vg);
        NVGpaint imgPaint = nvgImagePattern(vg, 0.f, 0.f, box.size.x, box.size.y, 0.f, image_handle, 1.f);
        nvgRect(vg,0.f, 0.f, box.size.x, box.size.y);
        nvgFillPaint(vg, imgPaint);
        nvgFill(vg);

    } else {
        BoxRect(vg, 0.f, 0.f, box.size.x, box.size.y, RampGray(G_85));
    }
    if (vertical()) {
        auto y = hue * box.size.y;
        Line(vg, 0.f, y, box.size.x, y, RampGray(G_85), .75f);
        Line(vg, 0.f, y + .75f, box.size.x, y + .75f, RampGray(G_10), .75f);
    } else {
        auto x = hue * box.size.x;
        Line(vg, x, 0.f, x, box.size.y, RampGray(G_85), .75f);
        Line(vg, x + .75f, 0.f, x + .75f, box.size.y, RampGray(G_10), .75f);
    }
}

//
// SLWidget Saturation + Lightness
//

SLWidget::~SLWidget() {
    if (ramp) {
        auto pic = ramp->getPic(); // cache doesn't own the pic.
        delete ramp;
        delete pic;
    }
}

void SLWidget::setHue(float new_hue) {
    if (new_hue == hue) return;
    hue = new_hue;
    SetSLSpectrum(getRamp()->getPic(), hue);
    getRamp()->invalidateImage();
}
cachePic* SLWidget::getRamp() {
    if (nullptr == ramp) {
        ramp = new cachePic(CreateSLSpectrum(hue, box.size.x, box.size.y));
    }
    return ramp;
}
void SLWidget::onContextCreate(const ContextCreateEvent& e)
{
    OpaqueWidget::onContextCreate(e);
    if (ramp) {
        ramp->onContextCreate(e);
    }
}
void SLWidget::onContextDestroy(const ContextDestroyEvent& e)
{
    if (ramp) {
        ramp->onContextDestroy(e);
    }
    OpaqueWidget::onContextDestroy(e);
}

void SLWidget::onEnter(const EnterEvent &e) {
    OpaqueWidget::onEnter(e);
    glfwSetCursor(APP->window->win, glfwCreateStandardCursor(GLFW_CROSSHAIR_CURSOR));
}

void SLWidget::onLeave(const LeaveEvent &e) {
    OpaqueWidget::onLeave(e);
    glfwSetCursor(APP->window->win, NULL);
}

void SLWidget::onDragMove(const DragMoveEvent& e) {
    OpaqueWidget::onDragMove(e);
    drag_pos = drag_pos.plus(e.mouseDelta.div(getAbsoluteZoom()));

    sat = drag_pos.x / box.size.x;
    light = 1.f - (drag_pos.y / box.size.y);
    if (clickHandler) {
        clickHandler(sat, light);
    }
}

void SLWidget::onButton(const ButtonEvent& e) {
    OpaqueWidget::onButton(e);
    if (!(e.action == GLFW_PRESS && e.button == GLFW_MOUSE_BUTTON_LEFT && (e.mods & RACK_MOD_MASK) == 0)) {
        return;
    }
    e.consume(this);
    drag_pos = e.pos;
}

void SLWidget::draw(const DrawArgs& args) {
    auto vg = args.vg;
    auto image_handle = getRamp()->getHandle(vg);
    if (image_handle) {
        nvgBeginPath(vg);
        NVGpaint imgPaint = nvgImagePattern(vg, 0.f, 0.f, box.size.x, box.size.y, 0.f, image_handle, 1.f);
        nvgRect(vg,0.f, 0.f, box.size.x, box.size.y);
        nvgFillPaint(vg, imgPaint);
        nvgFill(vg);
    } else {
        BoxRect(vg, 0.f, 0.f, box.size.x, box.size.y, RampGray(G_85));
    }
    auto x = sat * box.size.x;
    auto y = box.size.y - light * box.size.y;
    NVGcolor crosshair = RampGray(G_WHITE);
    crosshair.a = .5f;
    Line(vg, 0.f, y, box.size.x, y, crosshair);
    Line(vg, x, 0.f, x, box.size.y, crosshair);
    auto pix = getRamp()->getPic()->pixel(Vec(x,y));
    Circle(vg, x, y, 5.f, pix);
    OpenCircle(vg, x, y, 5.25, y <  box.size.y/3.f ? RampGray(G_05) : RampGray(G_95), 0.75f);
}


}