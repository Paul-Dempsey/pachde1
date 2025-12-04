#pragma once
#include <rack.hpp>
#include "services/colors.hpp"
#include "services/theme.hpp"
#include "widgets/tip-widget.hpp"
using namespace ::rack;
using namespace widgetry;
namespace pachde {

struct InfoSymbol: OpaqueWidget, ISetTheme
{
    using Base = OpaqueWidget;
    TipHolder* tip_holder{nullptr};

    NVGcolor color{RampGray(G_85)};
    NVGcolor co_hover{fromPacked(0xff06809a)};
    bool hovered{false};
    std::function<void()> click_handler{nullptr};

    InfoSymbol() {
        box.size.x = box.size.y = 15.f;
        describe("Info: (F2) Click for Text options");
    }

    void ensureTipHolder() { if (!tip_holder) { tip_holder = new TipHolder(); } }
    void describe(std::string text) { ensureTipHolder(); tip_holder->setText(text); }
    void destroyTip() { if (tip_holder) { tip_holder->destroyTip(); } }
    void createTip() { ensureTipHolder(); if (tip_holder) { tip_holder->createTip(); }}

    void set_handler(std::function<void()> fn) { click_handler = fn; }
    void setTheme(Theme theme) override;
    void onHover(const HoverEvent& e) override { hovered = true; Base::onHover(e); e.consume(this); }
    void onEnter(const EnterEvent& e) override { hovered = true; createTip(); Base::onEnter(e); }
    void onLeave(const LeaveEvent& e) override { hovered = false; destroyTip(); Base::onLeave(e); }
    void onButton(const ButtonEvent& e) override;
    void draw(const DrawArgs& args) override;
};

}