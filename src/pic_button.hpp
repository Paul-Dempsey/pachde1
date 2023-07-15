#pragma once
#include <rack.hpp>
#include "colors.hpp"
#include "theme.hpp"

using namespace rack;
namespace pachde {

struct PicButton: OpaqueWidget, ThemeLite
{
    NVGcolor line, sky1, sky2, mountain, moon;
    float gradient_stop_x;
    float gradient_stop_y;
    Tooltip* tip = nullptr;

    bool pressed = false;
    bool ctrl = false;
    bool shift = false;
    std::function<void(bool,bool)> clickHandler;

    virtual ~PicButton() {
        if (tip) delete tip;
    }

    PicButton(Theme theme);

    void setTheme(Theme theme) override;
    void draw(const DrawArgs &args) override;
    void onHoverKey(const HoverKeyEvent& e) override {
        rack::OpaqueWidget::onHoverKey(e);
        ctrl = (e.mods & RACK_MOD_MASK) & RACK_MOD_CTRL;
        shift = (e.mods & RACK_MOD_MASK) & GLFW_MOD_SHIFT;
    }
    void onButton(const event::Button& e) override;
    void onDragEnd(const DragEndEvent & e) override;
    void onEnter(const EnterEvent& e) override {
        createTooltip();
    }
    void onLeave(const LeaveEvent& e) override {
        destroyTooltip();
    }

    void createTooltip();
    void destroyTooltip();

    void center(Vec pos) {
        box.pos = pos.minus(box.size.div(2));
    }
    void onClick(std::function<void(bool,bool)> callback) {
        clickHandler = callback;
    }
};

}