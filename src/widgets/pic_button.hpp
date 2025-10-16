#pragma once
#include <rack.hpp>
#include "../services/colors.hpp"
#include "../services/theme.hpp"

using namespace rack;
namespace pachde {

struct PicButton: OpaqueWidget, IBasicTheme
{
    NVGcolor line, sky1, sky2, mountain, moon;
    float gradient_stop_x{17.5};
    float gradient_stop_y{10.f};
    Tooltip* tip {nullptr};

    bool pressed {false};
    bool ctrl {false};
    bool shift {false};
    std::function<void(bool,bool)> clickHandler {nullptr};

    virtual ~PicButton() {
        if (tip) delete tip;
    }

    PicButton();

    void applyTheme(Theme theme);
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