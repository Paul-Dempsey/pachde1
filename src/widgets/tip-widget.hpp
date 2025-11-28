// Copyright (C) Paul Chase Dempsey
#pragma once
#include "widgetry.hpp"

namespace widgetry {

struct TipHolder
{
    rack::ui::Tooltip* tip;
    std::string tip_text;

    TipHolder() : tip(nullptr) {}
    ~TipHolder() {
        destroyTip();
    }

    void setText(std::string text) {
        tip_text = text;
        if (tip) {
            tip->onChange(widget::Widget::ChangeEvent{});
        }
    }

    void createTip() {
        if (!rack::settings::tooltips) return;
        if (tip_text.empty()) return;
        if (tip) return;
        tip = new Tooltip;
        tip->text = tip_text;
        APP->scene->addChild(tip);
    }

    void destroyTip() {
        if (!tip) return;
        auto t = tip;
        tip = nullptr;
	    APP->scene->removeChild(t);
        delete t;
    }
};

struct TipWidget : ::rack::widget::Widget
{
    using Base = ::rack::widget::Widget;
    TipHolder* tip_holder;

    TipWidget() : tip_holder(nullptr) {}
    virtual ~TipWidget() {
        if (tip_holder) delete tip_holder;
        tip_holder = nullptr;
    }

    bool hasText() { return tip_holder && !tip_holder->tip_text.empty(); }

    void ensureTipHolder() {
        if (!tip_holder) {
            tip_holder = new TipHolder();
        }
    }

    void describe(std::string text) {
        ensureTipHolder();
        tip_holder->setText(text);
    }

    void destroyTip() {
        if (tip_holder) { tip_holder->destroyTip(); }
    }

    void createTip() {
        ensureTipHolder();
        if (tip_holder) { tip_holder->createTip(); }
    }

    void onHover(const HoverEvent& e) override {
        Base::onHover(e);
        e.consume(this);
    }

    void onEnter(const EnterEvent& e) override {
        Base::onEnter(e);
        createTip();
    }

    void onLeave(const LeaveEvent& e) override {
        Base::onLeave(e);
        destroyTip();
    }

    void onDragLeave(const DragLeaveEvent& e) override {
        Base::onDragLeave(e);
        destroyTip();
    }

    void onDragEnd(const DragEndEvent& e) override {
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
