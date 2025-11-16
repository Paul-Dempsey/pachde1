#pragma once
#include <rack.hpp>
//using namespace ::rack;

namespace widgetry {

struct DialogBase : ::rack::widget::OpaqueWidget
{
    using Base = ::rack::widget::OpaqueWidget;

    ::rack::app::ModuleWidget * source{nullptr};

    DialogBase(::rack::app::ModuleWidget * src) : source(src) {}

    void onAction(const ActionEvent& e) override {
        e.unconsume(); // don't close menu overlay
    }

    void close() {
        auto overlay = getAncestorOfType<::rack::ui::MenuOverlay>();
        if (overlay) {
            overlay->requestDelete();
        }
    }
};

template<typename TSvg>
struct SvgDialog : DialogBase
{
    ::rack::widget::FramebufferWidget* fb{nullptr};
    ::rack::widget::SvgWidget* sw{nullptr};

    SvgDialog(::rack::app::ModuleWidget* src) : DialogBase(src) {
        fb = new ::rack::widget::FramebufferWidget;
        addChild(fb);
        sw = new ::rack::widget::SvgWidget;
        fb->addChild(sw);

        set_svg(::rack::window::Svg::load(TSvg::background()));
    }

    void set_svg(std::shared_ptr<::rack::window::Svg> svg) {
        if (!sw->svg) {
            sw->setSvg(svg);
            fb->box.size = box.size = sw->box.size;
            fb->setDirty();
        }
    }
};

struct TrackingZoom : ::rack::widget::ZoomWidget
{
    using Base = ::rack::widget::ZoomWidget;

    ::rack::Widget * pseudo_parent{nullptr};
    ::rack::math::Vec pseudo_pos;

    TrackingZoom(::rack::widget::Widget *pseudo_parent, ::rack::math::Vec pos)
        : pseudo_parent(pseudo_parent), pseudo_pos(pos) {}

    void step() override {
        Base::step();
        float current_zoom = APP->scene->rackScroll->getZoom();
        if (getZoom() != current_zoom) {
            setZoom(current_zoom);
        }
        auto abs_pos = pseudo_parent->getAbsoluteOffset(pseudo_pos);
        auto scene = APP->scene;
        if (scene->menuBar && scene->menuBar->isVisible()) {
            abs_pos.y -= scene->menuBar->box.size.y;
        }
        box.pos = scene->getRelativeOffset(abs_pos, scene->rackScroll);
    }
};

template <typename TDialog>
TDialog* createMinimalMenuDialog() {
    TDialog* dialog = new TDialog();
    ::rack::createMenu()->addChild(dialog);
    return dialog;
}

template <typename TDialog>
TDialog* createDialog(
    ::rack::app::ModuleWidget* source,
    ::rack::math::Vec pos,
    bool center = false,
    NVGcolor screen = NVGcolor{0}
) {
    TDialog* dialog = new TDialog(source);
    if (center) {
        pos = pos.minus(dialog->box.size.div(2));
    }

    auto menuOverlay = new ::rack::ui::MenuOverlay;
    menuOverlay->bgColor = screen; // optionally dim/screen rack

    auto zoomer = new TrackingZoom(source, pos);
    menuOverlay->addChild(zoomer);
    zoomer->addChild(dialog);
    APP->scene->rackScroll->addChild(menuOverlay);

    return dialog;
}

template <typename TDialog>
TDialog* createMenuDialog(
    ::rack::app::ModuleWidget* source,
    ::rack::math::Vec(pos),
    bool center = false,
    NVGcolor screen = NVGcolor{0}
) {
    TDialog* dialog = new TDialog(source);

    auto menu = ::rack::createMenu();
    auto overlay = menu->getAncestorOfType<MenuOverlay>();
    overlay->bgColor = screen;

    bool real_pos = pos.isFinite();
    if (real_pos) {
        menu->box.pos = source->getAbsoluteOffset(pos);
    }
    menu->addChild(dialog);
    if (real_pos && center) {
        menu->step(); // adjust size and position
        menu->box.pos = menu->box.pos.minus(menu->box.size.div(2));
    }
    return dialog;
}

}

/*
MIT License

Copyright (c) 2025 Paul Chase Dempsey

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/