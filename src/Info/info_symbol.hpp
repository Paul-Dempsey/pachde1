#pragma once
#include <rack.hpp>
#include "../colors.hpp"
#include "../theme.hpp"
using namespace ::rack;

namespace pachde {

struct InfoSymbol: OpaqueWidget, IBasicTheme
{
    NVGcolor color;
    InfoSymbol() {
        box.size.x = box.size.y = 15.f;
    }
    void setTheme(Theme theme) override;
    void draw(const DrawArgs& args) override;
};

}