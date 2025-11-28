#pragma once
#include <rack.hpp>
using namespace ::rack;

namespace pachde {

struct IHaveColor {
    // For copper:
    // id == 0 : unmodulated color
    // id == 1 : modulated color
    virtual NVGcolor getColor(int id) { return nvgRGB(id % 255, id % 255, id % 255); }
    // Check if color sharing is enabled or disabled
    // color is available regardless, so best to check
    virtual bool colorExtenderEnabled() = 0;
};

}
