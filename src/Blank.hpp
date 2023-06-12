#pragma once
#include <rack.hpp>
#include "colors.hpp"
#include "components.hpp"
#include "resizable.hpp"

namespace pachde {

struct BlankModuleWidget : ModuleWidget, ITheme
{
    ITheme * alternateTheme = nullptr;
    ModuleResizeHandle* rightHandle = nullptr;
    ScrewCap* topRightScrew = nullptr;
    ScrewCap* bottomRightScrew = nullptr;
    ThemePanel* panel = nullptr;

    virtual ~BlankModuleWidget() {
        if (alternateTheme) {
            delete alternateTheme;
        }
    }
    ITheme * getITheme() {
        auto it = dynamic_cast<ITheme*>(module);
        if (it) {
            return it;
        }
        if (alternateTheme) {
            return alternateTheme;
        }
        alternateTheme = new ThemeBase();
        alternateTheme->setScrews(true);
        return alternateTheme;
    }

    BlankModuleWidget(ResizableModule *module);

    void step() override;
    void draw(const DrawArgs& args) override;
    void appendContextMenu(Menu *menu) override;
    void setTheme(Theme theme) override;
    void setPanelColor(NVGcolor color) override;
    void setScrews(bool showScrews) override;
    NVGcolor getPanelColor() override { return getITheme()->getPanelColor(); }
    Theme getTheme() override { return getITheme()->getTheme(); }
    bool hasScrews() override { return getITheme()->hasScrews(); }
    void addResizeHandles();
    void add_screws();
};

}