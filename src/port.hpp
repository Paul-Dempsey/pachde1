#pragma once
#include "plugin.hpp"
#include "colors.hpp"
#include "theme.hpp"

namespace pachde {

#define PORT_BLUE   nvgRGB(0x64, 0x9d, 0xcb);
#define PORT_RED    nvgRGB(0xca, 0x27, 0x27);
#define PORT_ORANGE nvgRGB(0xc7, 0x74, 0x25);
#define PORT_YELLOW nvgRGB(0xd5, 0xd3, 0x34);
#define PORT_GREEN  nvgRGB(0x14, 0xad, 0x47);
#define PORT_CYAN   nvgRGB(0x34, 0xaa, 0xd5);
#define PORT_VIOLET nvgRGB(0xc1, 0x3e, 0xca);

struct ColorPort : PortWidget, ThemeLite {

    NVGcolor ring = PORT_BLUE;
    NVGcolor collar1, collar2, edge, bevel1, bevel2, sleeve;

    ColorPort(Theme theme) { 
        box.size.x = box.size.y = 24.f;
        setTheme(theme);
    }
	void draw(const DrawArgs& args) override;
    void setTheme(Theme theme) override;
    void setPanelColor(NVGcolor color) override {
        if (isColorTransparent(color)) {
            ring = PORT_BLUE;
        } else {
            ring = color;
        }
    }
    NVGcolor getPanelColor() override { return ring; }
};

}