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
    inline float x_pos(float factor) { return left() + width() * factor; }
    inline float y_pos(float factor) { return top() + height() * factor; }
    inline Vec center() { return Vec(x_pos(.5f), y_pos(.5f)); }
    inline Vec center_top() { return Vec(x_pos(.5f), top()); }
    inline Vec center_left() { return Vec(left(), y_pos(.5f)); }
    inline Vec center_right() { return Vec(right(), y_pos(.5f)); }
    inline Vec center_bottom() { return Vec(x_pos(.5f), bottom()); }
};

inline NVGcolor from_packed_alpha(PackedColor color, float multiplier) {
    auto co = fromPacked(color);
    co.a *= multiplier;
    return co;
}

void CloakBackgroundWidget::step() {
    if (data.image.enabled) {
        if (data.image.options.path.empty()) {
            if (pic) {
                pic->close();
                return;
            }
        }
        if (!pic) {
            pic = new Picture(&data.image.options);
            addChild(pic);
        }
        if (!pic->image_data) {
            pic->open();
        }
    } else {
        if (pic) {
            pic->requestDelete();
            pic = nullptr;
        }
    }
    if (data.skiff.enabled) {
        if (!skiff) {
            skiff = new SkiffBox(&data.skiff.options);
            addChild(skiff);
        }
    } else {
        if (skiff) {
            skiff->requestDelete();
            skiff = nullptr;
        }
    }
    Base::step();
}

//
void CloakBackgroundWidget::draw_fill(const DrawArgs &args)
{
    auto vg = args.vg;
    nvgBeginPath(vg);
    nvgRect(vg, RECT_ARGS(args.clipBox));
    nvgFillColor(vg, from_packed_alpha(data.fill.color, data.fill.fade));
    nvgFill(vg);
}

void CloakBackgroundWidget::draw_linear(const DrawArgs &args) {
    auto vg = args.vg;
    RefBox r{args.clipBox};
    float x1 = r.x_pos(data.linear.x1);
    float y1 = r.y_pos(data.linear.y1);
    float x2 = r.x_pos(data.linear.x2);
    float y2 = r.y_pos(data.linear.y2);
    nvgBeginPath(vg);
    nvgRect(vg, RECT_ARGS(r.box));
    auto paint = nvgLinearGradient(
        vg,
        x1, y1, x2, y2,
        from_packed_alpha(data.linear.icol, data.linear.ifade),
        from_packed_alpha(data.linear.ocol, data.linear.ofade)
    );
    nvgFillPaint(vg, paint);
    nvgFill(vg);
    //debug
    // Circle(vg, x1, y1, 6, RampGray(G_0));
    // Circle(vg, x1, y1, 4, RampGray(G_WHITE));
    // Circle(vg, x2, y2, 6, RampGray(G_0));
    // Circle(vg, x2, y2, 4, RampGray(G_WHITE));
}

void CloakBackgroundWidget::draw_radial(const DrawArgs &args) {
    auto vg = args.vg;
    RefBox r{args.clipBox};
    float w = r.width();
    float h = r.height();
    float base = std::max(w, h);
    float radius = base * data.radial.radius;
    float cx = r.x_pos(data.radial.cx);
    float cy = r.y_pos(data.radial.cy);
    nvgBeginPath(vg);
    nvgRect(vg, RECT_ARGS(r.box));
    auto paint = nvgRadialGradient(
        vg,
        cx, cy, 0.f, radius,
        from_packed_alpha(data.radial.icol, data.radial.ifade),
        from_packed_alpha(data.radial.ocol, data.radial.ofade)
    );
    nvgFillPaint(vg, paint);
    nvgFill(vg);
    //debug
    // Circle(vg, cx, cy, 12, RampGray(G_WHITE));
    // Circle(vg, cx, cy, 6, RampGray(G_BLACK));
    // Line(vg, cx, cy, cx + radius*.5, cy + radius*.5, RampGray(G_WHITE));
}

void CloakBackgroundWidget::draw_box(const DrawArgs &args)
{
    auto vg = args.vg;
    nvgSave(vg);
    RefBox r{args.clipBox};

    if (data.boxg.xshrink < 1.f || data.boxg.yshrink < 1.f) {
        r.box = r.box.shrink(Vec(data.boxg.xshrink * r.width(), data.boxg.yshrink * r.height()));
    }

    float w = r.width();
    float h = r.height();
    float base = std::max(w, h);
    float radius = data.boxg.radius * base;
    float feather = data.boxg.feather * base;

    nvgBeginPath(vg);
    nvgRect(vg, RECT_ARGS(r.box));
    auto paint = nvgBoxGradient(
        vg,
        r.left(), r.top(), w, h,
        radius, feather,
        from_packed_alpha(data.boxg.icol, data.boxg.ifade),
        from_packed_alpha(data.boxg.ocol, data.boxg.ofade)
    );
    nvgFillPaint(vg, paint);
    nvgFill(vg);
    nvgRestore(vg);
}

void CloakBackgroundWidget::draw(const DrawArgs &args)
{
    if (data.image.enabled && pic) {
        pic->box = args.clipBox;
        Widget::drawChild(pic, args, 0);
    }
    if (data.fill.enabled) {
        draw_fill(args);
    }
    if (data.linear.enabled) {
        draw_linear(args);
    }
    if (data.radial.enabled) {
        draw_radial(args);
    }
    if (data.boxg.enabled) {
        draw_box(args);
    }
    if (data.skiff.enabled && skiff) {
        Widget::drawChild(skiff, args, 0);
    }
}

CloakBackgroundWidget * ensureBackgroundCloak(CloakData* data) {
    auto cloak = getBackgroundCloak();
    if (!cloak) {
        auto rail = APP->scene->rack->getFirstDescendantOfType<RailWidget>();
        cloak = new CloakBackgroundWidget(data);
        // cloak becomes child of RackWidget
        rail->getParent()->addChildAbove(cloak, rail);
    }
    return cloak;
}

} // widgetry