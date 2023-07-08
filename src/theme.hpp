#pragma once
#include <rack.hpp>
#include "colors.hpp"

using namespace rack;
namespace pachde {

struct ITheme {
    virtual ~ITheme() {}
    virtual void setTheme(Theme theme) {}
    virtual void setMainColor(NVGcolor color) {}
    virtual void setScrews(bool showScrews) {}

    virtual Theme getTheme() = 0;
    virtual NVGcolor getMainColor() = 0;
    virtual bool hasScrews() = 0;
};

// just a theme -- no screws, no panel override
struct ThemeLite: ITheme
{
    virtual ~ThemeLite() {}
    Theme theme = DefaultTheme;
    void setTheme(Theme theme) override { this->theme = theme; };
    Theme getTheme() override { return theme; }
    NVGcolor getMainColor() override { return COLOR_NONE; }
    bool hasScrews() override { return false; }
};

struct ThemeBase: ITheme
{
    Theme theme = DefaultTheme;
    NVGcolor main_color = COLOR_NONE;
    bool screws = false;

    virtual ~ThemeBase() {}

    bool isColorOverride() { return isColorVisible(main_color); }

    void setTheme(Theme theme) override { this->theme = theme; };
    void setMainColor(NVGcolor color) override { main_color = color; };
    void setScrews(bool showScrews) override { screws = showScrews;};

    Theme getTheme() override { return ConcreteTheme(theme); };
    NVGcolor getMainColor() override { return main_color; };
    bool hasScrews() override { return screws; }

    virtual json_t* save(json_t* root);
    virtual void load(json_t* root);
};


}