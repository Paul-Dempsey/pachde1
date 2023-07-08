#pragma once
#include <rack.hpp>
#include "theme.hpp"

using namespace rack;
namespace pachde {

// static NVGcolor screen = nvgRGBAf(0.95f, 0.95f, 0.95f, 0.4f);
// static NVGcolor shade = nvgRGBAf(0.,0.,0.,0.4f);
struct PlayPauseButton: Switch, ThemeLite
{
    NVGcolor collar, edge, face, face2, symbol;
    bool pressed = false;
    std::function<void(void)> clickHandler;

    PlayPauseButton(Theme theme);

    void draw(const DrawArgs &args) override;
    void onDragEnd(const DragEndEvent & e) override;
	void initParamQuantity() override {
        Switch::initParamQuantity();
        engine::ParamQuantity* pq = getParamQuantity();
        if (pq) {
            pressed = (pq->getValue() > 0.f);
        }
    }

    void setTheme(Theme theme) override;

    void center(Vec pos) {
        box.pos = pos.minus(box.size.div(2));
    }
    void onClick(std::function<void(void)> callback) {
        clickHandler = callback;
    }
};

}