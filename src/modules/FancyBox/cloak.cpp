#include "cloak.hpp"
#include "stb_image.h"
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

void CloakBackgroundWidget::add_client(ICloakBackgroundClient *client)
{
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
        client->onDeleteCloak(this);
    }
}

inline NVGcolor from_packed_alpha(PackedColor color, float multiplier) {
    auto co = fromPacked(color);
    co.a *= multiplier;
    return co;
}

void CloakBackgroundWidget::step() {
    if (data.image.enabled) {
        if (data.image.path.empty()) return;
        if (!pic) {
            pic = new Picture(&data.image);
            addChild(pic);
        }
        if (!pic->image_data) {
            if (!pic->open()) {
                return;
            }
        }
    } else {
        if (pic) {
            pic->requestDelete();
            pic = nullptr;
        }
    }
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
    if (data.image.enabled) {
        if (pic) {
            pic->box = args.clipBox;
            Widget::drawChild(pic, args, 0);
        }
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
}

CloakBackgroundWidget * ensureBackgroundCloak(Widget*host, CloakData* data) {
    auto cloak = getBackgroundCloak();
    if (!cloak) {
        auto rail = APP->scene->rack->getFirstDescendantOfType<RailWidget>();
        cloak = new CloakBackgroundWidget(data);
        rail->getParent()->addChildAbove(cloak, rail);
    }
    return cloak;
}


bool Picture::open() {
    close();
    image_data = stbi_load(options->path.c_str(), &image_width, &image_height, &image_components, 4);
    if (!image_data) {
        image_width = image_height = 0;
        fail_reason = stbi_failure_reason();
        return false;
    }
    if (options->gray) {
        black_and_white();
    }
    return true;
}

void Picture::close() {
    if (image_data) {
        auto mem = image_data;
        image_data = nullptr;
        stbi_image_free(mem);
    }

    auto window = APP->window;
    if (window && window->vg) {
        clearImageCache(window->vg);
    } else {
        // leak handle
        image_handle = 0;
    }
}

constexpr const float b2f_factor {1./255.f};
inline float LuminanceLinear(uint8_t r, uint8_t g, uint8_t b) {
    return 0.2126f * r*b2f_factor + 0.7152f * g * b2f_factor + 0.0722f * b * b2f_factor;
}

void Picture::black_and_white()
{
    if (image_data) {
        uint8_t* scan = image_data;
        for (int i = 0; i < image_width * image_height; i++) {
            uint8_t* poke = scan;
            auto r = *scan++;
            auto g = *scan++;
            auto b = *scan++;
            scan++;
            uint8_t l = static_cast<uint8_t>(255.f * LuminanceLinear(r,g,b));
            *poke++ = l;
            *poke++ = l;
            *poke = l;
        }
    }
}

void Picture::clearImageCache(NVGcontext* vg) {
    if (image_handle) {
        nvgDeleteImage(vg, image_handle);
        image_handle = 0;
    }
}

void Picture::updateImageCache(NVGcontext* vg) {
    if (!image_data) {
        clearImageCache(vg);
        return;
    }
    if (!image_cookie && !image_handle) {
        image_handle = nvgCreateImageRGBA(vg, width(),  height(), 0, image_data);
        image_cookie = reinterpret_cast<intptr_t>(image_data);
    } else {
        auto cookie = reinterpret_cast<intptr_t>(image_data);
        if (cookie != image_cookie) {
            if (image_handle) {
                nvgDeleteImage(vg, image_handle);
                image_handle = nvgCreateImageRGBA(vg, width(),  height(), 0, image_data);
            }
            image_cookie = image_handle ? cookie : 0;
        }
    }
}

void Picture::draw(const DrawArgs &args)
{
    updateImageCache(args.vg);
    if (image_handle) {
        auto vg = args.vg;
        float scale;
        float x_scale = box.size.x / image_width;
        float y_scale = box.size.y / image_height;
        switch (options->fit) {
            case ImageFit::Fit:
                scale = std::min(x_scale, y_scale);
                x_scale = y_scale = scale;
                break;
            case ImageFit::Stretch:
                break;
            case ImageFit::Cover:
                scale = std::max(x_scale, y_scale);
                x_scale = y_scale = scale;
                break;
        }

        float x = (box.size.x/x_scale - image_width)/2.f;
        float y = (box.size.y/y_scale - image_height)/2.f;

        nvgSave(vg);
        nvgBeginPath(vg);
        nvgScale(vg, x_scale, y_scale);
        nvgRect(vg, x, y, image_width, image_height);
        NVGpaint paint = nvgImagePattern(vg, x, y, image_width, image_height, 0.f, image_handle, 1.f);
        nvgFillPaint(vg, paint);
        nvgFill(vg);
        nvgRestore(vg);
    }

}

} // widgetry