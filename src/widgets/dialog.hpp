#pragma once
//
// Copyright (c) 2025 Paul Chase Dempsey
// License at end of file
//
// Configuration options:
//
// #define DIALOG_THEMED to enable SVG cache control for theming
// #define DIALOG_MODAL_SCREEN to enable modal screening
//

#include <rack.hpp>
//using namespace ::rack;
#ifdef DIALOG_THEMED
#include "services/svg-theme.hpp"
#endif

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

#ifdef DIALOG_THEMED
    ILoadSvg* load_svg{nullptr};

    SvgDialog(::rack::app::ModuleWidget* src, ILoadSvg* load_svg) : DialogBase(src), load_svg(load_svg) {
        fb = new ::rack::widget::FramebufferWidget;
        addChild(fb);
        sw = new ::rack::widget::SvgWidget;
        fb->addChild(sw);

        set_svg(load_svg->loadSvg(TSvg::background()));
    }
#endif

    std::shared_ptr<::rack::window::Svg> get_svg() { return sw->svg; }

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

template <typename TDialog, typename TSource>
TDialog* createDialog(
    TSource* source,
    ::rack::math::Vec pos,
#ifdef DIALOG_THEMED
    ILoadSvg* load_svg,
#endif
#ifdef DIALOG_MODAL_SCREEN
    const NVGcolor& screen,
#endif
    bool center = false
) {
#ifdef DIALOG_THEMED
    TDialog* dialog = new TDialog(source, load_svg);
#else
    TDialog* dialog = new TDialog(source);
#endif
    if (center) {
        pos = pos.minus(dialog->box.size.div(2));
    }

    auto menuOverlay = new ::rack::ui::MenuOverlay;
#ifdef DIALOG_MODAL_SCREEN
    menuOverlay->bgColor = screen; // optionally dim/screen rack
#endif

    auto zoomer = new TrackingZoom(source, pos);
    menuOverlay->addChild(zoomer);
    zoomer->addChild(dialog);
    APP->scene->rackScroll->addChild(menuOverlay);

    return dialog;
}


template <typename TDialog, typename TSource>
TDialog* createMenuDialog(
    TSource* source,
    ::rack::math::Vec(pos),
#ifdef DIALOG_THEMED
    ILoadSvg* load_svg,
#endif
#ifdef DIALOG_MODAL_SCREEN
    const NVGcolor& screen,
#endif
    bool center = false
) {
#ifdef DIALOG_THEMED
    TDialog* dialog = new TDialog(source, load_svg);
#else
    TDialog* dialog = new TDialog(source);
#endif

    auto menu = ::rack::createMenu();
    auto overlay = menu->getAncestorOfType<MenuOverlay>();
#ifdef DIALOG_MODAL_SCREEN
    menuOverlay->bgColor = screen; // optionally dim/screen rack
#endif

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