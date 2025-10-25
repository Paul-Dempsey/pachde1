#pragma once
#include <rack.hpp>
using namespace ::rack;
#include "../services/colors.hpp"
#include "../services/text.hpp"
#include "../services/svg-theme-2.hpp"
#include "tip-widget.hpp"
using namespace svg_theme_2;
using namespace pachde;
namespace widgetry {

struct HamData
{
    uint8_t patties{3};
    float patty_width{1.5f};
    NVGcolor patty_color{RampGray(G_50)};
    NVGcolor hover_color{RampGray(G_65)};
    bool hovered{false};

    bool applyTheme(std::shared_ptr<SvgTheme> theme) {
        if (theme) {
            PackedColor color;
            if (theme->getFillColor(color, "patty", true)) {
                patty_color = fromPacked(color);
            } else {
                patty_color = RampGray(G_50);
            }
            if (theme->getFillColor(color, "patty-hover", true)) {
                hover_color = fromPacked(color);
            } else {
                hover_color = RampGray(G_65);
            }
        } else {
            patty_color = RampGray(G_50);
            hover_color = RampGray(G_65);
        }
        return false;
    }

    void draw(Widget* host, const Widget::DrawArgs& args) {
        auto vg = args.vg;
        auto co  = patty_color;

        if (hovered) {
            co = hover_color;
            auto half_x = host->box.size.x * .5f;
            auto half_y = host->box.size.y * .5f;
            OpenCircle(vg, half_x, half_y, half_x + .25f, co, .75f);
        }
        float y = 3.5f;
        float step = std::max(2.5f, patty_width + 1);
        for (auto n = 0; n < patties; ++n) {
            Line(vg, 2.f, y, host->box.size.x - 2.f, y, co, patty_width); y += step;
        }
    }

};

struct HamburgerTitle : MenuLabel
{
    NVGcolor co_bg;
    NVGcolor co_text;

    HamburgerTitle() {
        switch (getActualTheme(ThemeSetting::FollowRackUi)) {
        case Theme::Unset:
        case Theme::Light:
            co_bg = nvgHSL(200.f/360.f,.5,.4);
            co_text = nvgRGB(250, 250, 250);
            break;
        case Theme::Dark:
            co_bg = nvgHSL(200.f/360.f,.5,.4);
            co_text = nvgRGB(250, 250, 250);
            break;
        case Theme::HighContrast:
            co_bg = nvgRGB(254, 254, 254);
            co_text = nvgRGB(0,0,0);
            break;
        }
    }

    void draw(const DrawArgs& args) override{
        auto vg = args.vg;
        auto font = GetPluginFontSemiBold();
        if (!FontOk(font)) return;
        FillRect(vg, 0, 0, box.size.x, box.size.y - 1.f, co_bg);
        SetTextStyle(vg, font, co_text, 16.f);
        CenterText(vg, box.size.x*.5, 14.f, text.c_str(), nullptr);
    }
};

struct Hamburger : TipWidget, IThemed
{
    using Base = TipWidget;
    HamData data;

    Hamburger() {
        box.size.x = 12.f;
        box.size.y = 12.f;
    }

    void createContextMenu() {
        ui::Menu* menu = createMenu();
    	appendContextMenu(menu);
    }

    void onHover(const HoverEvent& e) override {
        e.consume(this);
        Base::onHover(e);
    }

    void onEnter(const EnterEvent& e) override {
        data.hovered = true;
        Base::onEnter(e);
    }

    void onLeave(const LeaveEvent& e) override {
        data.hovered = false;
        Base::onLeave(e);
    }

    void onButton(const ::rack::Widget::ButtonEvent& e) override {
        if ((e.action == GLFW_PRESS) && ((e.mods & RACK_MOD_MASK) == 0)) {
            switch (e.button) {
            case GLFW_MOUSE_BUTTON_LEFT:
                Base::createContextMenu();
                e.consume(this);
                return;
            case GLFW_MOUSE_BUTTON_RIGHT:
                e.consume(this);
                return;
            }
        }
        Base::onButton(e);
    }

    bool applyTheme(std::shared_ptr<SvgTheme> theme) override {
        return data.applyTheme(theme);
    }

    void onHoverKey(const HoverKeyEvent& e) override {
        switch (e.key) {
            case GLFW_KEY_ENTER:
            case GLFW_KEY_MENU:
            if (e.action == GLFW_RELEASE) {
                e.consume(this);
                createContextMenu();
                return;
            }
        }
        Base::onHoverKey(e);
    }

    void draw(const DrawArgs& args) override
    {
        Base::draw(args);
        data.draw(this, args);
    }
};

template <class TBaseWidget>
struct HamburgerUi : TBaseWidget
{
    using Base = TBaseWidget;
    HamData data;

    HamburgerUi() {
        Base::box.size.x = 12.f;
        Base::box.size.y = 12.f;
    }

    void onHover(const ::rack::Widget::HoverEvent& e) override {
        e.consume(this);
        Base::onHover(e);
    }

    void onEnter(const ::rack::Widget::EnterEvent& e) override {
        data.hovered = true;
        Base::onEnter(e);
    }

    void onLeave(const ::rack::Widget::LeaveEvent& e) override {
        data.hovered = false;
        Base::onLeave(e);
    }

    void onButton(const ::rack::Widget::ButtonEvent& e) override {
        if ((e.action == GLFW_PRESS) && ((e.mods & RACK_MOD_MASK) == 0)) {
            switch (e.button) {
            case GLFW_MOUSE_BUTTON_LEFT:
                Base::createContextMenu();
                e.consume(this);
                return;
            case GLFW_MOUSE_BUTTON_RIGHT:
                e.consume(this);
                return;
            }
        }
        Base::onButton(e);
    }

    void applyTheme(std::shared_ptr<SvgTheme> theme) { data.applyTheme(theme); }

    void draw(const ::rack::Widget::DrawArgs& args) override {
        Base::draw(args);
        data.draw(this, args);
    }
};

using HamParam = HamburgerUi<ParamWidget>;

}
