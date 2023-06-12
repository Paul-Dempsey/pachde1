#pragma once
#include <rack.hpp>
#include "colors.hpp"

using namespace rack;
namespace pachde {

struct ITheme {
    virtual ~ITheme() {}
    virtual void setTheme(Theme theme) {}
    virtual void setPanelColor(NVGcolor color) {}
    virtual void setScrews(bool showScrews) {}

    virtual Theme getTheme() = 0;
    virtual NVGcolor getPanelColor() = 0;
    virtual bool hasScrews() = 0;
};

struct ThemeBase: ITheme
{
    Theme theme = DefaultTheme;
    NVGcolor panel_color = COLOR_NONE;
    bool screws = false;
    bool isColorOverride() { return isColorVisible(panel_color); }

    void setTheme(Theme theme) override { this->theme = theme; };
    void setPanelColor(NVGcolor color) override { panel_color = color; };
    void setScrews(bool showScrews) override { screws = showScrews;};

    Theme getTheme() override { return ConcreteTheme(theme); };
    NVGcolor getPanelColor() override { return panel_color; };
    bool hasScrews() override { return screws; }

    virtual json_t* save(json_t* root);
    virtual void load(json_t* root);

};


}