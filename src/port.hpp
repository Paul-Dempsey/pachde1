#pragma once
#include "plugin.hpp"
#include "colors.hpp"
#include "theme.hpp"

namespace pachde {

// PORT_* colors are HSL 30-degree hue increments
#define PORT_RED     nvgHSL(0, 0.6, 0.5)
#define PORT_ORANGE  nvgHSL(30./360., 0.80, 0.5)
#define PORT_YELLOW  nvgHSL(60./360., 0.65, 0.5)
#define PORT_LIME    nvgHSL(90./360., 0.60, 0.5)
#define PORT_GREEN   nvgHSL(120./360., 0.5, 0.5)
#define PORT_GRASS   nvgHSL(150./360., 0.5, 0.5)
#define PORT_CYAN    nvgHSL(180./360., 0.5, 0.5)
#define PORT_CORN    nvgHSL(210./360., 0.5, 0.55)
#define PORT_BLUE    nvgHSL(240./360., 0.5, 0.55)
#define PORT_VIOLET  nvgHSL(270./360., 0.5, 0.5)
#define PORT_MAGENTA nvgHSL(300./360., 0.5, 0.5)
#define PORT_PINK    nvgHSL(330./360., 0.5, 0.5)

#define PORT_DEFAULT nvgHSL(210./360., 0.5, 0.65)

struct ColorPort : PortWidget, ThemeLite
{
    NVGcolor ring = PORT_DEFAULT;
    NVGcolor collar1, collar2, edge, bevel1, bevel2, sleeve;

    ColorPort(Theme theme) { 
        box.size.x = box.size.y = 24.f;
        setTheme(theme);
    }
	void draw(const DrawArgs& args) override;
    void setTheme(Theme theme) override;
    void setPanelColor(NVGcolor color) override {
        ring = isColorTransparent(color) ? PORT_DEFAULT : color;
    }
    NVGcolor getPanelColor() override { return ring; }
};

}