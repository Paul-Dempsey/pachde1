#pragma once
#include "widgetry.hpp"

using namespace pachde;

namespace widgetry {

template<typename TSymbol>
struct TSymbolWidget: OpaqueWidget, ISetTheme
{
    using Base = OpaqueWidget;

    bool ignore_theme_changes{false};
    bool bright{false};
    float scale;
    widget::FramebufferWidget* fb{nullptr};
	widget::SvgWidget* sw{nullptr};

    TSymbolWidget(Theme theme, float scale = 1.f) : scale(scale) {
        fb = new widget::FramebufferWidget;
        addChild(fb);
        sw = new widget::SvgWidget;
        fb->addChild(sw);
        setTheme(theme);
    }

    void set_scale(float scale) {
        if (scale != this->scale) {
            this->scale = scale;
            dirtyWidget(this);
        }
    }

    void setSvg(std::shared_ptr<window::Svg> svg) {
        if (svg == sw->svg) return;
        sw->setSvg(svg);
        fb->box.size = sw->box.size * scale;
        box.size = sw->box.size * scale;
        fb->setDirty();
    }

    void setTheme(Theme theme) override {
        if (ignore_theme_changes) return;

        switch (theme) {
        case Theme::Dark:
            setSvg(Svg::load(asset::plugin(pluginInstance, TSymbol::symbol_dark())));
            break;
        case Theme::HighContrast:
            setSvg(Svg::load(asset::plugin(pluginInstance, TSymbol::symbol_hc())));
            break;
        case Theme::Unset:
        case Theme::Light:
        default:
            setSvg(Svg::load(asset::plugin(pluginInstance, TSymbol::symbol())));
            break;
        }
    }

    void drawLayer(const DrawArgs& args, int layer) override {
        if (bright) {
            DrawArgs args2 = args;
            args2.clipBox.pos = args2.clipBox.pos.div(scale);
            args2.clipBox.size = args2.clipBox.size.div(scale);
            nvgScale(args.vg, scale, scale);
            Base::drawLayer(args2, layer);
        }
    }

    void draw(const DrawArgs& args) override {
        if (!bright) {
            DrawArgs args2 = args;
            args2.clipBox.pos = args2.clipBox.pos.div(scale);
            args2.clipBox.size = args2.clipBox.size.div(scale);
            nvgScale(args.vg, scale, scale);
            Base::draw(args2);
        }
    }
};

struct LogoSvg {
    static std::string symbol() { return "res/widget/logo.svg"; }
    static std::string symbol_dark() { return "res/widget/logo-dark.svg"; }
    static std::string symbol_hc() { return "res/widget/logo-hc.svg"; }
};

using LogoWidget = TSymbolWidget<LogoSvg>;

}