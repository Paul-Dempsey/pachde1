#pragma once
#include <rack.hpp>
using namespace ::rack;

namespace pachde {

struct IHaveColor {
    // For copper:
    // id == 0 : unmodulated color
    // id == 1 : modulated color
    virtual NVGcolor getColor(int id) { return nvgRGB(id % 255, id % 255, id % 255); }
};

}
