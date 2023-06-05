#pragma once
#include <rack.hpp>
#include "Imagine.hpp"

using namespace rack;
namespace pachde {

struct PicWidget : OpaqueWidget {
    Imagine *module = nullptr;
    Vec mousepos;

    PicWidget(Imagine *module);

    void onButton(const event::Button& e) override;
    void onDragMove(const event::DragMove& e) override;

    void updateClient();

    void drawPic(const DrawArgs &args);
    void drawSample(const DrawArgs &args);
    void drawLayer(const DrawArgs &args, int layer) override;

    void draw(const DrawArgs &args) override;
};

}