#pragma once
#include <rack.hpp>
#include "services/colors.hpp"
#include "services/theme.hpp"
using namespace ::rack;

namespace pachde {

struct InfoSymbol: OpaqueWidget, ISetTheme
{
    NVGcolor color;
    InfoSymbol() {
        box.size.x = box.size.y = 15.f;
    }
    void setTheme(Theme theme) override;
    void draw(const DrawArgs& args) override;
};

}