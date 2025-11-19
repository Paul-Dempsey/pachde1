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

void CloakBackgroundWidget::add_client(ICloakBackgroundClient* client) {
    auto it = std::find(clients.begin(), clients.end(), client);
    if (it == clients.end()) {
        clients.push_back(client);
    }
}

void CloakBackgroundWidget::remove_client(ICloakBackgroundClient* client) {
    auto it = std::find(clients.begin(), clients.end(), client);
    if (it == clients.end()) {
        clients.erase(it);
    }
}

CloakBackgroundWidget::~CloakBackgroundWidget() {
    for (auto client: clients) {
        client->onDelete(this);
    }
}

void CloakBackgroundWidget::draw(const DrawArgs& args) {
    auto vg = args.vg;

    if (data.fill.enabled) {
        auto co = fromPacked(data.fill.color);
        co.a *= data.fill.fade;
        RefBox r{args.clipBox};
        nvgBeginPath(vg);
        nvgRect(vg, RECT_ARGS(r.box));
        nvgFillColor(vg, co);
        nvgFill(vg);
    }
    if (data.linear.enabled) {
        auto icol = fromPacked(data.linear.icol);
        icol.a *= data.linear.ifade;
        auto ocol = fromPacked(data.linear.ocol);
        ocol.a *= data.linear.ofade;
        RefBox r{args.clipBox};
        float x1 = r.width() * data.linear.x1;
        float y1 = r.height() * data.linear.y1;
        float x2 = r.width() * data.linear.x2;
        float y2 = r.height() * data.linear.y2;
        nvgBeginPath(vg);
        nvgRect(vg, RECT_ARGS(r.box));
        auto paint = nvgLinearGradient(vg, x1, y1, x2, y2, icol, ocol);
        nvgFillPaint(vg, paint);
        nvgFill(vg);
    }
    if (data.radial.enabled) {
        auto icol = fromPacked(data.radial.icol);
        icol.a *= data.radial.ifade;
        auto ocol = fromPacked(data.radial.ocol);
        ocol.a *= data.radial.ofade;
        RefBox r{args.clipBox};
        float w = r.width();
        float h = r.height();
        float cx = data.radial.cx * w;
        float cy = data.radial.cy * h;
        float radius = data.radial.radius * std::max(w, h);
        auto paint = nvgRadialGradient(vg, cx, cy, 0, radius, icol, ocol);
        nvgFillPaint(vg, paint);
        nvgFill(vg);
    }
    if (data.boxg.enabled) {
        RefBox r{args.clipBox};
        auto icol = fromPacked(data.boxg.icol);
        icol.a *= data.boxg.ifade;
        auto ocol = fromPacked(data.boxg.ocol);
        ocol.a *= data.boxg.ofade;
        if (data.boxg.xshrink < 1.f || data.boxg.yshrink < 1.f) {
            r.box = r.box.shrink(Vec(data.boxg.xshrink * r.width(), data.boxg.yshrink * r.height()));
        }
        float w = r.width();
        float h = r.height();
        float base = std::max(w, h);
        float radius = data.boxg.radius * base;
        float feather = data.boxg.feather * base;

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

CloakBackgroundWidget * applyCloak(CloakBackgroundWidget* cloak) {
    if (cloak) {
        auto rail = APP->scene->rack->getFirstDescendantOfType<RailWidget>();
        if (!rail->getParent()->hasChild(cloak)) {
            rail->getParent()->addChildAbove(cloak, rail);
        }
    } else {
        cloak = ensureBackgroundCloak();
    }
    return cloak;
}

} // widgetry