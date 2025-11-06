#pragma once
#include "../myplugin.hpp"
#include "components.hpp"
#include "../services/theme.hpp"
using namespace pachde;
namespace widgetry {

struct SmallPush: OpaqueWidget, ISetTheme, ISetColor
{
    bool pressed = false;
    bool ctrl = false;
    bool shift = false;
    rack::ui::Tooltip* tip = nullptr;
    std::string text;
    NVGcolor ring = COLOR_BRAND_MD;
    NVGcolor collar1, collar2, bezel, bevel1, bevel2, face1, face2;

    virtual ~SmallPush() {
        if (tip) delete tip;
        tip = nullptr;
    }

    SmallPush()
    {
        box.size.x = box.size.y = 20.f;
    }

    void describe(std::string description) {
        text = description;
    }

    void createTip() {
        if (!rack::settings::tooltips) return;
        if (tip) return;
        tip = new Tooltip;
        tip->text = text;
        APP->scene->addChild(tip);
    }

    void destroyTip() {
        if (!tip) return;
        auto t = tip;
        tip = nullptr;
	    APP->scene->removeChild(t);
        delete t;
    }

    void center(Vec pos) {
        box.pos = pos.minus(box.size.div(2));
    }

    std::function<void(bool, bool)> clickHandler;
    void set_handler(std::function<void(bool, bool)> callback)
    {
        clickHandler = callback;
    }

    void onHoverKey(const HoverKeyEvent& e) override {
        rack::OpaqueWidget::onHoverKey(e);
        ctrl = (e.mods & RACK_MOD_MASK) & RACK_MOD_CTRL;
        shift = (e.mods & RACK_MOD_MASK) & GLFW_MOD_SHIFT;
    }

    void onEnter(const EnterEvent& e) override {
        createTip();
    }

    void onLeave(const LeaveEvent& e) override {
        destroyTip();
    }

    void onDragStart(const DragStartEvent& e) override {
        pressed = true;
        rack::OpaqueWidget::onDragStart(e);
    }

    void onDragEnd(const DragEndEvent& e) override
    {
        pressed = false;
        rack::OpaqueWidget::onDragEnd(e);
        if (e.button != GLFW_MOUSE_BUTTON_LEFT)
            return;

        if (clickHandler) {
            destroyTip();
            clickHandler(ctrl, shift);
        }
    }

    void setTheme(Theme theme) override;

    void setMainColor(PackedColor color) override {
        ring = packed_color::isVisible(color) ? fromPacked(color) : COLOR_BRAND_MD;
    }

    PackedColor getMainColor() override { return toPacked(ring);}

    void draw(const DrawArgs& args) override;
};

}