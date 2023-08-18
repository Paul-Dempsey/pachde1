#pragma once
#ifndef LOGOPORT_HPP_INCLUDED
#define LOGOPORT_HPP_INCLUDED
#include "../components.hpp"
#include "../theme.hpp"

namespace pachde {

struct LogoPort : PortWidget, IBasicTheme
{
    NVGcolor logo;
    bool invisible = false;

    LogoPort() { 
        box.size.x = box.size.y = 15.f;
    }
    void setTheme(Theme theme) override {
        IBasicTheme::setTheme(theme);
        logo = LogoColor(getTheme());
        switch (theme) {
            default: 
            case Theme::Unset:
            case Theme::Light:
                logo.a = 0.5f;
                break;
            case Theme::Dark:
                logo.a = 0.5f;
                break;
            case Theme::HighContrast:
                break;
        }
        if (isColorVisible(main_color)) {
            auto lum = LuminanceLinear(main_color);
            if (lum <= 0.5f) {
                logo = Gray(lum + 0.5);
            } else {
                logo = Gray(lum - 0.4);
            }
            if (theme != Theme::HighContrast) {
                logo.a = 0.75;
            }
        }
    }
	void draw(const DrawArgs& args) override
    {
        if (invisible) return;
        DrawLogo(args.vg, 0.f, 0.f, logo);
    }
};

}
#endif