#include "svg-query.hpp"

namespace svg_query {

::rack::math::Rect elementBounds(std::shared_ptr<::rack::window::Svg> svg, const char* id)
{
    for (NSVGshape* shape = svg->handle->shapes; nullptr != shape; shape = shape->next) {
        if (shape->id[0] && (0 == strncmp(shape->id, id, 64))) {
            float * r = &shape->bounds[0];
            return ::rack::math::Rect(*r, r[1], r[2] - *r, r[3] - r[1]);
        }
    }
    return ::rack::math::Rect(INFINITY, INFINITY, 0, 0);
}

void boundsIndex(std::shared_ptr<::rack::window::Svg> svg, const char *prefix, std::map<std::string, ::rack::math::Rect> &map, bool hide)
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

void shapeIndex(std::shared_ptr<::rack::window::Svg> svg, const char *prefix, std::map<std::string, NSVGshape *> &map)
{
    int len = strlen(prefix);
    for (NSVGshape* shape = svg->handle->shapes; nullptr != shape; shape = shape->next) {
        if (shape->id[0] && (0 == strncmp(shape->id, prefix, len))) {
            map[std::string(shape->id)] = shape;
        }
    }
}

void hideElement(std::shared_ptr<::rack::window::Svg> svg, const char *id)
{
    for (NSVGshape* shape = svg->handle->shapes; nullptr != shape; shape = shape->next) {
        if (shape->id[0] && (0 == strncmp(shape->id, id, 64))) {
            shape->opacity = 0.f;
            return;
        }
    }
}

void showElement(std::shared_ptr<::rack::window::Svg> svg, const char *id)
{
    for (NSVGshape* shape = svg->handle->shapes; nullptr != shape; shape = shape->next) {
        if (shape->id[0] && (0 == strncmp(shape->id, id, 64))) {
            shape->opacity = 1.f;
            return;
        }
    }
}

void hideElements(std::shared_ptr<::rack::window::Svg> svg, const char *prefix)
{
    int len = strlen(prefix);
    for (NSVGshape* shape = svg->handle->shapes; nullptr != shape; shape = shape->next) {
        if (shape->id[0] && (0 == strncmp(shape->id, prefix, len))) {
            shape->opacity = 0.f;
        }
    }
}

void showElements(std::shared_ptr<::rack::window::Svg> svg, const char *prefix)
{
    int len = strlen(prefix);
    for (NSVGshape* shape = svg->handle->shapes; nullptr != shape; shape = shape->next) {
        if (shape->id[0] && (0 == strncmp(shape->id, prefix, len))) {
            shape->opacity = 1.f;
        }
    }
}

std::shared_ptr<::rack::window::Svg> panelWidgetSvg(::rack::widget::Widget* panel)
{
    if (!panel) return nullptr;

    auto themed = dynamic_cast<::rack::app::ThemedSvgPanel*>(panel);
    if (themed) return themed->lightSvg;

    auto unthemed = dynamic_cast<::rack::app::SvgPanel*>(panel);
    if (unthemed) return unthemed->svg;

    return nullptr;
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
