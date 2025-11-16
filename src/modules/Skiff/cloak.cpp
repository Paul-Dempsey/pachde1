#include "cloak.hpp"
#include "services/colors.hpp"

namespace widgetry {
using namespace packed_color;
using namespace pachde;

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

    if (fill.enabled) {
        auto co = fromPacked(fill.color);
        co.a *= fill.fade;
        RefBox r{args.clipBox};
        nvgBeginPath(vg);
        nvgRect(vg, RECT_ARGS(r.box));
        nvgFillColor(vg, co);
        nvgFill(vg);
    }
    if (l_grad.enabled) {
        auto icol = fromPacked(l_grad.icol);
        icol.a *= l_grad.ifade;
        auto ocol = fromPacked(l_grad.ocol);
        ocol.a *= l_grad.ofade;
        RefBox r{args.clipBox};
        float x1 = r.width() * l_grad.x1;
        float y1 = r.height() * l_grad.y1;
        float x2 = r.width() * l_grad.x2;
        float y2 = r.height() * l_grad.y2;
        nvgBeginPath(vg);
        nvgRect(vg, RECT_ARGS(r.box));
        auto paint = nvgLinearGradient(vg, x1, y1, x2, y2, icol, ocol);
        nvgFillPaint(vg, paint);
        nvgFill(vg);
    }
    if (r_grad.enabled) {
        auto icol = fromPacked(r_grad.icol);
        icol.a *= r_grad.ifade;
        auto ocol = fromPacked(r_grad.ocol);
        ocol.a *= r_grad.ofade;
        RefBox r{args.clipBox};
        float w = r.width();
        float h = r.height();
        float cx = r_grad.cx * w;
        float cy = r_grad.cy * h;
        float radius = r_grad.r * std::max(w, h);
        auto paint = nvgRadialGradient(vg, cx, cy, 0, radius, icol, ocol);
        nvgFillPaint(vg, paint);
        nvgFill(vg);
    }
    if (b_grad.enabled) {
        RefBox r{args.clipBox};
        auto icol = fromPacked(b_grad.icol);
        icol.a *= b_grad.ifade;
        auto ocol = fromPacked(b_grad.ocol);
        ocol.a *= b_grad.ofade;
        if (b_grad.xshrink < 1.f || b_grad.yshrink < 1.f) {
            r.box = r.box.shrink(Vec(b_grad.xshrink * r.width(), b_grad.yshrink * r.height()));
        }
        float w = r.width();
        float h = r.height();
        float base = std::max(w, h);
        float radius = b_grad.radius * base;
        float feather = b_grad.feather * base;

        auto paint = nvgBoxGradient(vg, r.left(), r.top(), w, h, radius, feather, icol, ocol);
        nvgFillPaint(vg, paint);
        nvgFill(vg);
    }
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