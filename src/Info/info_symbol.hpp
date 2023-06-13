#pragma once
#include <rack.hpp>
#include "../colors.hpp"
#include "../theme.hpp"
using namespace rack;

namespace pachde {

struct InfoSymbol: OpaqueWidget, ThemeLite
{
    NVGcolor color;
    InfoSymbol(Theme theme) {
        box.size.x = box.size.y = 15.f;
        setTheme(theme);
    }
    void setTheme(Theme theme) override;
    void draw(const DrawArgs& args) override;
};

}