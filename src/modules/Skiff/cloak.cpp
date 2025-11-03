#include "cloak.hpp"
namespace widgetry {


struct RefBox
{
    Rect box;
    RefBox(Rect r) : box(r) {}
    inline float top() { return box.pos.y; }
    inline float left() { return box.pos.x; }
    inline float bottom() { return top() + height(); }
    inline float right() { return left() + width(); }
    inline float width() { return box.size.x; }
    inline float height() { return box.size.y; }
    inline Vec center() { return Vec(x_pos(.5f), y_pos(.5f)); }
    inline Vec center_top() { return Vec(x_pos(.5f), top()); }
    inline Vec center_left() { return Vec(left(), y_pos(.5f)); }
    inline Vec center_right() { return Vec(right(), y_pos(.5f)); }
    inline Vec center_bottom() { return Vec(x_pos(.5f), bottom()); }
    inline float x_pos(float factor) { return left() + factor*width(); }
    inline float y_pos(float factor) { return top() + factor*height(); }
};

void CloakBackgroundWidget::draw(const DrawArgs& args) {
    auto vg = args.vg;
    NVGcolor icol = nvgRGB(0,0,0); //::rack::settings::preferDarkPanels ? nvgRGB(0x40, 0x40, 0x40) : nvgRGB(0x45,0x45,0x45);
    NVGcolor ocol = nvgRGB(252,252,252); //::rack::settings::preferDarkPanels ? nvgRGB(0x10, 0x10, 0x10) : nvgRGB(0xe6,0xe6,0xe6);

    RefBox r{args.clipBox};
    nvgBeginPath(vg);
    nvgRect(vg, RECT_ARGS(r.box));
    //auto paint = nvgRadialGradient(vg, VEC_ARGS(r.center()), 0, r.width()*.5, icol, ocol);
    auto paint = nvgLinearGradient(vg, r.left(), r.top(), r.right(), r.bottom(), icol, ocol);
    nvgFillPaint(vg, paint);
    //nvgFillColor(vg, color);
    nvgFill(vg);
}

CloakBackgroundWidget * toggleBackgroundCloak() {
    auto cloak = getBackgroundCloak();
    if (cloak) {
        cloak->requestDelete();
        return nullptr;
    } else {
        cloak = new CloakBackgroundWidget;
        auto rail = APP->scene->rack->getFirstDescendantOfType<RailWidget>();
        rail->getParent()->addChildAbove(cloak, rail);
        return cloak;
    }
}

CloakBackgroundWidget * ensureBackgroundCloak() {
    auto cloak = getBackgroundCloak();
    if (!cloak) {
        cloak = new CloakBackgroundWidget;
        auto rail = APP->scene->rack->getFirstDescendantOfType<RailWidget>();
        rail->getParent()->addChildAbove(cloak, rail);
    }
    return cloak;

}

} // widgetry