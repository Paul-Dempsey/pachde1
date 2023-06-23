#pragma once
#include <rack.hpp>
#include "Imagine.hpp"

using namespace rack;
namespace pachde {

struct PicWidget : OpaqueWidget {
    Imagine *module = nullptr;

    // experiment
    // Pic * spectrum = nullptr;
    // virtual ~PicWidget() {
    //     if (spectrum) delete spectrum;
    // }

    Vec mousepos;
    cachePic cpic;

    PicWidget(Imagine *module);
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
    void onButton(const event::Button& e) override;
    void onDragMove(const event::DragMove& e) override;

    void updateClient();

    // Consider: Use framebuffer for pic, and only draw the sample each frame
    void drawPic(const DrawArgs &args);
    void drawSample(const DrawArgs &args);
    void drawLayer(const DrawArgs &args, int layer) override;
    void draw(const DrawArgs &args) override;
};

}