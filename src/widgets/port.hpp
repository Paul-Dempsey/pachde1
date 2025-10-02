#pragma once
#include "../colors.hpp"
#include "../theme.hpp"
using namespace pachde;

namespace widgetry {

struct ColorPort : PortWidget, IBasicTheme
{
    NVGcolor ring = PORT_DEFAULT;
    NVGcolor collar1, collar2, bezel, tube, bevel1, bevel2, sleeve;

    ColorPort()
    {
        box.size.x = box.size.y = 22.f;
    }
	void draw(const DrawArgs& args) override;

    void applyTheme(Theme theme);
    void setTheme(Theme theme) override;
    void setMainColor(NVGcolor color) override
    {
        ring = isColorTransparent(color) ? PORT_DEFAULT : color;
    }
    NVGcolor getMainColor() override { return ring; }
};

}