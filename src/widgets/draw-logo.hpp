#pragma once
#include "widgetry.hpp"
#include "../services/theme.hpp"

using namespace pachde;
namespace widgetry {

void DrawLogo(NVGcontext * vg, float x, float y, NVGcolor fill, float scale = 1.0f);

struct LogoOverlayWidget : rack::OpaqueWidget, IBasicTheme
{
    LogoOverlayWidget() {
        box.size.x = box.size.y = 15.0f;
    }
    void draw(const DrawArgs &args) override {
        rack::OpaqueWidget::draw(args);
        auto color = LogoColor(actual_theme);
        if (actual_theme != Theme::HighContrast) {
            color.a = 0.75f;
        }
        DrawLogo(args.vg, 0, 0, color);
    }
};

}