#include "theme-module.hpp"
namespace pachde {

void ThemePanel::draw(const DrawArgs &args)
{
    Widget::draw(args);

    PackedColor co = theme_holder->getMainColor();
    NVGcolor color = co ? fromPacked(co) : PanelBackground(theme_holder->getTheme());

    auto vg = args.vg;

    nvgBeginPath(vg);
    nvgRect(vg, 0.0f, 0.0f, box.size.x, box.size.y);
    nvgFillColor(vg, color);
    nvgFill(vg);
}

}