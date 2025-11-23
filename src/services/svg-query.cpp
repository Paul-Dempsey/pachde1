#include "svg-query.hpp"

namespace svg_query {

::rack::math::Rect elementBounds(SharedSvg svg, const char* id)
{
    for (NSVGshape* shape = svg->handle->shapes; nullptr != shape; shape = shape->next) {
        if (shape->id[0] && (0 == strncmp(shape->id, id, 64))) {
            float * r = &shape->bounds[0];
            return ::rack::math::Rect(*r, r[1], r[2] - *r, r[3] - r[1]);
        }
    }
    return ::rack::math::Rect(INFINITY, INFINITY, 0, 0);
}

void addBounds(SharedSvg svg, const char *prefix, BoundsIndex &map, bool hide)
{
    int len = strlen(prefix);
    for (NSVGshape* shape = svg->handle->shapes; nullptr != shape; shape = shape->next) {
        if (shape->id[0] && (0 == strncmp(shape->id, prefix, len))) {
            float * r = &shape->bounds[0];
            map[std::string(shape->id)] = ::rack::math::Rect(*r, r[1], r[2] - *r, r[3] - r[1]);
            if (hide) shape->opacity = 0.f;
        }
    }
}

BoundsIndex makeBounds(SharedSvg svg, const char *prefix, bool hide)
{
    BoundsIndex bounds;
    addBounds(svg, prefix, bounds, hide);
    return bounds;
}

void shapeIndex(SharedSvg svg, const char *prefix, ShapeIndex& map)
{
    int len = strlen(prefix);
    for (NSVGshape* shape = svg->handle->shapes; nullptr != shape; shape = shape->next) {
        if (shape->id[0] && (0 == strncmp(shape->id, prefix, len))) {
            map[std::string(shape->id)] = shape;
        }
    }
}

void hideElement(SharedSvg svg, const char *id)
{
    for (NSVGshape* shape = svg->handle->shapes; nullptr != shape; shape = shape->next) {
        if (shape->id[0] && (0 == strncmp(shape->id, id, 64))) {
            shape->opacity = 0.f;
            return;
        }
    }
}

void showElement(SharedSvg svg, const char *id)
{
    for (NSVGshape* shape = svg->handle->shapes; nullptr != shape; shape = shape->next) {
        if (shape->id[0] && (0 == strncmp(shape->id, id, 64))) {
            shape->opacity = 1.f;
            return;
        }
    }
}

void hideElements(SharedSvg svg, const char *prefix)
{
    int len = strlen(prefix);
    for (NSVGshape* shape = svg->handle->shapes; nullptr != shape; shape = shape->next) {
        if (shape->id[0] && (0 == strncmp(shape->id, prefix, len))) {
            shape->opacity = 0.f;
        }
    }
}

void showElements(SharedSvg svg, const char *prefix)
{
    int len = strlen(prefix);
    for (NSVGshape* shape = svg->handle->shapes; nullptr != shape; shape = shape->next) {
        if (shape->id[0] && (0 == strncmp(shape->id, prefix, len))) {
            shape->opacity = 1.f;
        }
    }
}

SharedSvg panelWidgetSvg(::rack::widget::Widget* panel)
{
    if (!panel) return nullptr;

    auto themed = dynamic_cast<::rack::app::ThemedSvgPanel*>(panel);
    if (themed) return themed->lightSvg;

    auto unthemed = dynamic_cast<::rack::app::SvgPanel*>(panel);
    if (unthemed) return unthemed->svg;

    return nullptr;
}

void positionWidgets(const PositionIndex &positions, const BoundsIndex& bounds)
{
    using namespace ::rack::math;

    for (auto kv: positions) {
        #ifdef DEV_BUILD
        Rect r;
        try {
            r = bounds.at(kv.first);
        } catch (std::exception &e) {
            DEBUG("Bounds missing %s", kv.first);
            continue;
        }
#else
        Rect r{bounds.at(kv.first)};
#endif
        auto widget = kv.second.widget;
        switch (kv.second.kind) {
        default:
        case HotPosKind::Center:
            widget->box.pos = r.getCenter().minus(widget->box.size.div(2));
            break;
        case HotPosKind::Box:
            widget->box = r;
            break;
        case HotPosKind::BoundsCenter:
            widget->box.pos = r.getCenter();
            break;
        case HotPosKind::TopLeft:
            widget->box.pos = r.pos;
            break;
        case HotPosKind::TopMiddle:
            widget->box.pos = Vec(r.pos.x + r.size.x*.5, r.pos.y);
            break;
        case HotPosKind::TopRight:
            widget->box.pos = r.getTopRight();
            break;
        case HotPosKind::MiddleRight:
            widget->box.pos = Vec(r.pos.x + r.size.x, r.pos.y + r.size.y*.5);
            break;
        case HotPosKind::BottomRight:
            widget->box.pos = r.getBottomRight();
            break;
        case HotPosKind::BottomMiddle:
            widget->box.pos = Vec(r.pos.x + r.size.x*.5, r.pos.y + r.size.y);
            break;
        case HotPosKind::BottomLeft:
            widget->box.pos = r.getBottomLeft();
            break;
        case HotPosKind::MiddleLeft:
            widget->box.pos = Vec(r.pos.x, r.pos.y + r.size.y*.5);
            break;
        }
    }
}

}
/*
MIT License (MIT)

Copyright © 2025 Paul Chase Dempsey

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to permit
persons to whom the Software is furnished to do so, subject to the following
conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

---

 This software depends on and extends nanosvg.

 nanosvg, Copyright (c) 2013-14 Mikko Mononen memon@inside.org

 See nanosvg.h for license information.
 */
