#pragma once
#include "label.hpp"
#include "tip-widget.hpp"

namespace widgetry {

struct TipLabel : TextLabel
{
    using Base = TextLabel;

    TipHolder* tip_holder;

    TipLabel() : tip_holder(nullptr) {}

    virtual ~TipLabel() {
        if (tip_holder) delete tip_holder;
        tip_holder = nullptr;
    }

    void ensureTipHolder()
    {
        if (!tip_holder) {
            tip_holder = new TipHolder();
        }
    }

    std::string description() {
        if (!tip_holder) return "";
        return tip_holder->tip_text;
    }

    void describe(std::string text)
    {
        ensureTipHolder();
        tip_holder->setText(text);
    }

    void destroyTip() {
        if (tip_holder) { tip_holder->destroyTip(); }
    }

    void createTip() {
        ensureTipHolder();
        tip_holder->createTip();
    }

    void onHover(const HoverEvent& e) override
    {
        Base::onHover(e);
        e.consume(this);
    }

    void onEnter(const EnterEvent& e) override {
        Base:: onEnter(e);
        createTip();
    }

    void onLeave(const LeaveEvent& e) override {
        Base:: onLeave(e);
        destroyTip();
    }

    void onDragLeave(const DragLeaveEvent& e) override {
        Base::onDragLeave(e);
        destroyTip();
    }

    void onDragEnd(const DragEndEvent& e) override
    {
        Base::onDragEnd(e);
        destroyTip();
    }

    void onButton(const ButtonEvent& e) override
    {
        Base::onButton(e);
        if (e.action == GLFW_PRESS && e.button == GLFW_MOUSE_BUTTON_RIGHT && (e.mods & RACK_MOD_MASK) == 0) {
            destroyTip();
            createContextMenu();
            e.consume(this);
        }
    }

    void createContextMenu() {
        ui::Menu* menu = createMenu();
    	appendContextMenu(menu);
    }

    virtual void appendContextMenu(ui::Menu* menu) {}

};

}