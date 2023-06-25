#pragma once
#include <rack.hpp>
#include "../components.hpp"
#include "../pic.hpp"

using namespace rack;
namespace pachde {

struct HueWidget : OpaqueWidget
{
    std::function<void(float)> clickHandler;
    
    float hue = 0.f;
    cachePic* ramp = nullptr;

    virtual ~HueWidget() {
        if (ramp) {
            auto pic = ramp->getPic();
            delete ramp;
            delete pic;
        }
    }

    bool getHue() { return hue; }
    void setHue(float hue) { this->hue = hue; }

    bool vertical() { return box.size.x <= box.size.y; }

    cachePic* getHueRamp() {
        if (nullptr == ramp) {
            ramp = new cachePic(CreateHueRamp(box.size.x, box.size.y, vertical()));
        }
        return ramp;
    }

    void onContextCreate(const ContextCreateEvent& e)  override
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

    void onButton(const ButtonEvent& e) override
    {
    	OpaqueWidget::onButton(e);
        if (!(e.action == GLFW_PRESS && e.button == GLFW_MOUSE_BUTTON_LEFT && (e.mods & RACK_MOD_MASK) == 0)) {
            return;
        }
        e.consume(this);
        hue = vertical() ? e.pos.y / box.size.y : e.pos.x / box.size.x;
        if (clickHandler)
            clickHandler(hue);
    }

    // TODO: Dragging

    void draw(const DrawArgs & args) override
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

    void onClick(std::function<void(float)> callback)
    {
        clickHandler = callback;
    }
};

}