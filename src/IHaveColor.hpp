#pragma once
#if !defined IHAVECOLOR_HPP_INCLUDED
#define IHAVECOLOR_HPP_INCLUDED
#include <rack.hpp>
using namespace ::rack;

namespace pachde {

struct IHaveColor {
    virtual NVGcolor getColor(int id) { return nvgRGB(id % 255, id % 255, id % 255); }
};

}
#endif
