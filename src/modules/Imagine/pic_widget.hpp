#pragma once
#include <rack.hpp>
#include "Imagine.hpp"

using namespace rack;
namespace pachde {


struct PicWidget : OpaqueWidget {
    Imagine * module = nullptr;
    Vec mousepos;
    cachePic cpic;
    IProvideImage* image_source = nullptr;

    explicit PicWidget(Imagine *module);

    void setImageSource(IProvideImage* source) {
        image_source = source;
    }

    void onContextCreate(const ContextCreateEvent& e) override
    {
        OpaqueWidget::onContextCreate(e);
        cpic.onContextCreate(e);
    }
    
    void onContextDestroy(const ContextDestroyEvent& e) override
    {
        OpaqueWidget::onContextDestroy(e);
        cpic.onContextDestroy(e);
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

    void onButton(const event::Button& e) override;
    void onDragMove(const event::DragMove& e) override;
    void onPathDrop(const PathDropEvent& e) override;
    void updateClient();

    void drawPic(const DrawArgs &args);
    void drawSample(const DrawArgs &args);
    void drawLayer(const DrawArgs &args, int layer) override;
    void draw(const DrawArgs &args) override;
};

}