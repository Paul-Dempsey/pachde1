#pragma once
#include <rack.hpp>
using namespace ::rack;

namespace pachde {

struct IHaveColor {
    virtual NVGcolor getColor(int id) { return nvgRGB(id % 255, id % 255, id % 255); }
};

}
