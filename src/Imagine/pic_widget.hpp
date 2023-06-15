#pragma once
#include <rack.hpp>
#include "Imagine.hpp"

using namespace rack;
namespace pachde {

struct PicWidget : OpaqueWidget {
    Imagine *module = nullptr;
    Vec mousepos;
    // to manage image data properly, we must manage the NVG image handle properly, 
    // which means invalidating our cache and deleting the NVG image handle when
    // either the Image or the graphics context has changed.
    int image_handle = 0; // nvg Image handle
    intptr_t image_cookie = 0; // cookie for image data
    intptr_t vg_cookie = 0; // cookie for graphics context

    PicWidget(Imagine *module);

    void clearImageCache();

    void onButton(const event::Button& e) override;
    void onDragMove(const event::DragMove& e) override;

    void updateImageCache(NVGcontext* vg, Pic* pic);
    void updateClient();

    void drawPic(const DrawArgs &args);
    void drawSample(const DrawArgs &args);
    void drawLayer(const DrawArgs &args, int layer) override;

    void draw(const DrawArgs &args) override;
};

}