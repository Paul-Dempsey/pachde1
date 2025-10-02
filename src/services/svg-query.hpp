#pragma once
#include <rack.hpp>

namespace svg_query {

// itemBounds returns the bounds of the element with the given id.
// If id isn't found, returns Rect(0,0,0,0).
// Bounds are 4 floats [left, top, right, bottom]
::rack::math::Rect elementBounds(std::shared_ptr<::rack::window::Svg> svg, const char* id);
void boundsIndex(std::shared_ptr<::rack::window::Svg> svg, const char * prefix, std::map<std::string, ::rack::math::Rect>& map, bool hide);
void shapeIndex(std::shared_ptr<::rack::window::Svg> svg, const char * prefix, std::map<std::string, NSVGshape*>& map);

// Hide an item in the SVG by id.
void hideElement(std::shared_ptr<::rack::window::Svg> svg, const char* id);
void showElement(std::shared_ptr<::rack::window::Svg> svg, const char* id);

// Hide all placeholders in the svg whose id has the specified prefix.
void hideElements(std::shared_ptr<::rack::window::Svg> svg, const char * prefix);
void showElements(std::shared_ptr<::rack::window::Svg> svg, const char * prefix);

// Get a panel Svg(s)
inline std::shared_ptr<::rack::window::Svg> panelSvg(::rack::app::SvgPanel* panel) { return panel->svg; }
// The Light Svg panel is used by default
inline std::shared_ptr<::rack::window::Svg> panelSvg(::rack::app::ThemedSvgPanel* panel) { return panel->lightSvg; }
// Can also use the Dark Svg of a themed panel
inline std::shared_ptr<::rack::window::Svg> panelDarkSvg(::rack::app::ThemedSvgPanel* panel) { return panel->darkSvg; }

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
