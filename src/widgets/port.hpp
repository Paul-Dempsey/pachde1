#pragma once
#include "../services/colors.hpp"
#include "../services/theme.hpp"
using namespace pachde;

namespace widgetry {

struct ColorPort : PortWidget, ISetTheme, ISetColor
{
    NVGcolor ring{PORT_DEFAULT};
    NVGcolor collar1, collar2, bezel, tube, bevel1, bevel2, sleeve;

    ColorPort() {
        box.size.x = box.size.y = 22.f;
    }
	void draw(const DrawArgs& args) override;

    // ISetTheme
    void setTheme(Theme theme) override;
    // ISetColor
    void setMainColor(PackedColor color) override {
        if (packed_color::isVisible(color)) {
            ring = fromPacked(color);
        }
    }
    PackedColor getMainColor() override { return toPacked(ring); }
};

}