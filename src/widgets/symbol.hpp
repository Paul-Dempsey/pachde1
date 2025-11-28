#pragma once
#include "widgetry.hpp"

namespace widgetry {

template <typename TSvg>
struct TSymbol : OpaqueWidget {
    using Base = OpaqueWidget;

    bool bright{false};
    float scale{1.0};
    widget::FramebufferWidget* fb{nullptr};
	widget::SvgWidget* sw{nullptr};

    TSymbol() {
        fb = new widget::FramebufferWidget;
        addChild(fb);
        sw = new widget::SvgWidget;
        fb->addChild(sw);
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

	void drawLayer(const DrawArgs& args, int layer) override {
        if (!bright || layer != -1) return;
        DrawArgs args2 = args;
        args2.clipBox.pos = args2.clipBox.pos.div(scale);
        args2.clipBox.size = args2.clipBox.size.div(scale);
        nvgScale(args.vg, scale, scale);
        Base::drawLayer(args2, layer);
    }

	void draw(const DrawArgs& args) override {
        if (bright) return;
        DrawArgs args2 = args;
        args2.clipBox.pos = args2.clipBox.pos.div(scale);
        args2.clipBox.size = args2.clipBox.size.div(scale);
        nvgScale(args.vg, scale, scale);
        Base::draw(args2);
    }

    void loadSvg(ILoadSvg* loader) {
        setSvg(loader->loadSvg(TSvg::image()));
    }

    void applyTheme(std::shared_ptr<SvgTheme> theme) {
        applySvgTheme(sw->svg, theme);
        fb->dirty = true;
    }

    void update(std::shared_ptr<SvgTheme> theme) {
        sw->svg->loadFile(TSvg::image());
        applyTheme(theme);
    }

};

struct QuestionSvg {
    static std::string image() { return asset::plugin(pluginInstance, "res/widget/question-mark.svg"); }
};

using QuestionSymbol = TSymbol<QuestionSvg>;

}