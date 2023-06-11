#pragma once
#include <rack.hpp>
#include "colors.hpp"
#include "components.hpp"
#include "resizable.hpp"

namespace pachde {

struct BlankModule : ResizableModule {
};

struct BlankModuleWidget : ModuleWidget, IChangeTheme
{
    ModuleResizeHandle* rightHandle = nullptr;
    ScrewCap* topRightScrew = nullptr;
    ScrewCap* bottomRightScrew = nullptr;
    ThemePanel* panel = nullptr;

    BlankModuleWidget(BlankModule *module);

    void step() override;
    void draw(const DrawArgs& args) override;
    void appendContextMenu(Menu *menu) override;
    void setTheme(Theme theme) override;
    void setColor(NVGcolor color) override;
    void setScrews(bool showScrews) override;
    void addResizeHandles();
    void add_screws();
    void setScrewColors(NVGcolor color);
    NVGcolor getPanelColor() {
        return panel ? panel->getColor() : COLOR_NONE;
    }
};

}