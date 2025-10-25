#include "theme-module.hpp"
namespace pachde {

void ThemePanel::draw(const DrawArgs &args)
{
    Widget::draw(args);

    auto color = fromPacked(theme_holder->getMainColor());
    if (isColorTransparent(color)) {
        color = PanelBackground(theme_holder->getTheme());
    }
    auto vg = args.vg;

    nvgBeginPath(vg);
    nvgRect(vg, 0.0f, 0.0f, box.size.x, box.size.y);
    nvgFillColor(vg, color);
    nvgFill(vg);
}

}