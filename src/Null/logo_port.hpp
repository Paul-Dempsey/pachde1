#pragma once
#include "../theme.hpp"
#include "../widgets/logo-widget.hpp"
using namespace widgetry;

namespace pachde {

struct LogoPort : PortWidget, IBasicTheme
{
    LogoWidget* logo{nullptr};

    LogoPort() {
        box.size.x = box.size.y = 18.f;
    }

    void setTheme(Theme theme) override {
        if (children.empty()) {
            logo = new LogoWidget(theme, .18f);
            logo->box.pos = Vec(0, 0);
            addChild(logo);
        }
    }
};

}
