#pragma once
#include "widgetry.hpp"

namespace widgetry {

void DrawLogo(NVGcontext * vg, float x, float y, NVGcolor fill, float scale = 1.0f);

struct LogoOverlayWidget : rack::OpaqueWidget, IBasicTheme
{
    LogoOverlayWidget() {
        box.size.x = box.size.y = 15.0f;
    }
    void draw(const DrawArgs &args) override {
        rack::OpaqueWidget::draw(args);
        auto color = LogoColor(my_theme);
        if (my_theme != Theme::HighContrast) {
            color.a = 0.75f;
        }
        DrawLogo(args.vg, 0, 0, color);
    }
};

}