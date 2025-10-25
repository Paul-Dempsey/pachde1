#pragma once
#include "../services/theme.hpp"
#include "../widgets/logo-widget.hpp"
using namespace widgetry;

namespace pachde {

struct LogoPort : PortWidget
{
    LogoWidget* logo{nullptr};

    LogoPort() {
        box.size.x = box.size.y = 18.f;
    }

    void setTheme(Theme theme) {
        if (children.empty()) {
            logo = new LogoWidget(theme, .18f);
            logo->box.pos = Vec(0, 0);
            addChild(logo);
        }
    }
};

}
