#pragma once
#include <rack.hpp>
#include "services/theme.hpp"

using namespace rack;
namespace pachde {

struct PlayPauseButton: Switch, ISetTheme
{
    NVGcolor collar, edge, face, face2, symbol;
    bool pressed = false;
    std::function<void(void)> clickHandler;

    explicit PlayPauseButton();

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
    void set_handler(std::function<void(void)> callback) {
        clickHandler = callback;
    }
};

}