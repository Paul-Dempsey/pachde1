#pragma once
#include <rack.hpp>
#include "pic.hpp"
#include "../services/colors.hpp"

using namespace rack;
namespace widgetry {

// TODO: when rendering a new hue, it's likely much more efficient to
// use nvgUpdateImage than destroying and creating a new nvgImage.

struct SLWidget : OpaqueWidget
{
    float hue = 0.f;
    float sat = .5f;
    float light = .5f;
    cachePic* ramp = nullptr;
    std::function<void(float, float)> clickHandler;
    Vec drag_pos;

    SLWidget() { }
    explicit SLWidget(float hue) { this->hue = hue; }

    virtual ~SLWidget() {
        if (ramp) {
            auto pic = ramp->getPic(); // cache doesn't own the pic.
            delete ramp;
            delete pic;
        }
    }

    float getSaturation() { return sat; }
    float getLightness() { return light; }
    void setSaturation(float s) { sat = s; }
    void setLightness(float l) { light = l; }
    void setHue(float new_hue) {
        if (new_hue == hue) return;
        hue = new_hue;
        SetSLSpectrum(getRamp()->getPic(), hue);
        getRamp()->invalidateImage();
    }
    cachePic* getRamp() {
        if (nullptr == ramp) {
            ramp = new cachePic(CreateSLSpectrum(hue, box.size.x, box.size.y));
        }
        return ramp;
    }
    void onContextCreate(const ContextCreateEvent& e) override
    {
        OpaqueWidget::onContextCreate(e);
        if (ramp) {
            ramp->onContextCreate(e);
        }
    }
    void onContextDestroy(const ContextDestroyEvent& e) override
    {
        if (ramp) {
            ramp->onContextDestroy(e);
        }
        OpaqueWidget::onContextDestroy(e);
    }

    void onClick(std::function<void(float, float)> callback)
    {
        clickHandler = callback;
    }

    void onEnter(const EnterEvent &e) override
    {
        OpaqueWidget::onEnter(e);
        glfwSetCursor(APP->window->win, glfwCreateStandardCursor(GLFW_CROSSHAIR_CURSOR));
    }

    void onLeave(const LeaveEvent &e) override
    {
        OpaqueWidget::onLeave(e);
        glfwSetCursor(APP->window->win, NULL);
    }

    void onDragMove(const DragMoveEvent& e) override
    {
        OpaqueWidget::onDragMove(e);
        drag_pos = drag_pos.plus(e.mouseDelta.div(getAbsoluteZoom()));

        sat = drag_pos.x / box.size.x;
        light = 1.f - (drag_pos.y / box.size.y);
        if (clickHandler) {
            clickHandler(sat, light);
        }
    }

    void onButton(const ButtonEvent& e) override
    {
     	OpaqueWidget::onButton(e);
        if (!(e.action == GLFW_PRESS && e.button == GLFW_MOUSE_BUTTON_LEFT && (e.mods & RACK_MOD_MASK) == 0)) {
            return;
        }
        e.consume(this);
        drag_pos = e.pos;
    }

    void draw(const DrawArgs& args) override
    {
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

};

}