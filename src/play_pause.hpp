#pragma once
#include "plugin.hpp"
#include "colors.hpp"
#include "theme.hpp"

namespace pachde {

struct PlayPauseButton: Switch, ThemeLite
{
    NVGcolor collar, edge, face, symbol;
    NVGcolor screen = nvgRGBAf(0.9f,0.9f,0.9f,0.3f);
    NVGcolor shade = nvgRGBAf(0.,0.,0.,0.3f);
    bool pressed = false;
    std::function<void(void)> clickHandler;

    PlayPauseButton(Theme theme);

    void setTheme(Theme theme) override;
    void draw(const DrawArgs &args) override;
    //void onButton(const event::Button& e) override;
    void onDragEnd(const DragEndEvent & e) override;
	void initParamQuantity() override {
        Switch::initParamQuantity();
        engine::ParamQuantity* pq = getParamQuantity();
        if (pq) {
            pressed = (pq->getValue() > 0.f);
        }
    }

    void center(Vec pos) {
        box.pos = pos.minus(box.size.div(2));
    }
    void onClick(std::function<void(void)> callback) {
        clickHandler = callback;
    }
};

}