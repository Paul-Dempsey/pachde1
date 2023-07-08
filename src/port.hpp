#pragma once
#include "colors.hpp"
#include "theme.hpp"

namespace pachde {

struct ColorPort : PortWidget, ThemeLite
{
    NVGcolor ring = PORT_DEFAULT;
    NVGcolor collar1, collar2, bezel, tube, bevel1, bevel2, sleeve;

    ColorPort(Theme theme)
    { 
        box.size.x = box.size.y = 22.f;
        setTheme(theme);
    }
	void draw(const DrawArgs& args) override;
    void setTheme(Theme theme) override;
    void setMainColor(NVGcolor color) override
    {
        ring = isColorTransparent(color) ? PORT_DEFAULT : color;
    }
    NVGcolor getMainColor() override { return ring; }
};

}