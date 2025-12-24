#pragma once
#include <rack.hpp>
#include "point.hpp"

namespace widgetry {
constexpr const float PIC_EPSILON = 0.00001f;

struct Pic {
    int _width = 0;
    int _height = 0;
    int _components = 0;
    unsigned char * _data = nullptr;
    bool _raw_data = false; // true when we've new'd _data otherwise use stbi alloc/free
    std::string _path;
    std::string _reason;

    // create a Pic with allocated but uninitialized pixel data.
    static Pic* CreateRaw(int width, int height);

    int width() const { return _width; }
    int height() const { return _height; }
    Point extent() const { return Point(_width, _height); }
    int stride() const { return _width * 4; }
    int pixel_advance() const { return 4; }
    int components() const { return _components; }
    bool ok() const { return nullptr != _data; }
    unsigned char * data() const { return _data; }
    unsigned char * end() const;
    Point position(const unsigned char * location) const;
    unsigned char * pixel_address(int x, int y) const;
    unsigned char * pixel_address(const Point& point) const { return pixel_address(point.x, point.y); }
    NVGcolor pixel(int x, int y) const;
    NVGcolor pixel(const Point& point) const { return pixel(point.x, point.y); }
    NVGcolor pixel(float x, float y) const;
    NVGcolor pixel(rack::Vec pos) const { return pixel(pos.x, pos.y); }

    std::string name() const { return _path; }
    std::string reason() const { return _reason; }
    bool open(const std::string& filename);
    void close();

    ~Pic() {
        close();
    }
};

// Caching of associate nvg structure for a pic.
// Does not own the pic, only the cached nvg handle
struct cachePic
{
    Pic* pic = nullptr;
    int image_handle = 0; // nvg Image handle
    intptr_t image_cookie = 0; // cookie for image data

    ~cachePic() {
        //HACK grab a VG context from anywhere and hope its the right one
        if (image_handle) {
            auto window = APP->window;
            if (window && window->vg) {
                nvgDeleteImage(window->vg, image_handle);
            }
        }
    }

    cachePic() { }
    explicit cachePic(Pic * pic) { setPic(pic); }

#define TRUST_RACK_CONTEXT // depends on a fixed Rack post 2.3
#ifndef TRUST_RACK_CONTEXT
    bool isLaterVersion(int maj, int min) {
        auto parts = rack::string::split(rack::APP_VERSION, ".");
        auto rack_maj = std::strtol(parts[0].c_str(), nullptr, 10);
        auto rack_min = std::strtol(parts[1].c_str(), nullptr, 10);
        if (rack_maj <= maj) return false;
        if (rack_min <= min) return false;
        return true;
    }
#endif

    // Widgets using cachePic must forward the
    // onContextCreate and onContextDestroy events.
    void onContextCreate(const rack::widget::Widget::ContextCreateEvent& e)
    {
#ifndef TRUST_RACK_CONTEXT
        NVGcontext* ctx = nullptr;
        // HACK: workaround Rack bug wrt uninitialized vg in context change events.
        // It's not that bad a hack, because Rack itself does the same thing.
        if (!isLaterVersion(2, 3)) {
            auto window = APP->window;
            if (window && window->vg) {
                ctx = window->vg;
            }
        } else {
            ctx = e.vg;
        }
        if (ctx) {
            updateImageCache(ctx);
        } // else leak
#else
        updateImageCache(e.vg);
#endif
    }

    void onContextDestroy(const rack::widget::Widget::ContextDestroyEvent& e)
    {
#ifndef TRUST_RACK_CONTEXT
        NVGcontext* ctx = nullptr;
        //HACK: work around uninitialized e.vg
        if (!isLaterVersion(2, 3)) {
            auto window = APP->window;
            if (window && window->vg) {
                ctx = window->vg;
            }
        } else {
            ctx = e.vg;
        }
        if (ctx) {
            clearImageCache(ctx);
        } // else leak
#else
        clearImageCache(e.vg);
#endif
    }

    void setPic(Pic * picture) {
        pic = picture;
        if (!pic || image_cookie != reinterpret_cast<intptr_t>(pic->data())) {
            invalidateImage();
        }
    }
    Pic * getPic() { return pic; }

    int getHandle(NVGcontext* vg) {
        updateImageCache(vg);
        return image_handle;
    }

    void invalidateImage() {
        image_cookie = 0;
    }

    void clearImageCache(NVGcontext* vg) {
        if (image_handle) {
            nvgDeleteImage(vg, image_handle);
            image_handle = 0;
        }
        image_cookie = 0;
    }

    void updateImageCache(NVGcontext* vg)
    {
        if (!pic) {
            clearImageCache(vg);
            return;
        }
        if (!image_cookie && !image_handle) {
            image_handle = nvgCreateImageRGBA(vg, pic->width(),  pic->height(), 0, pic->data());
            image_cookie = reinterpret_cast<intptr_t>(pic->data());
        } else {
            auto new_image_cookie = reinterpret_cast<intptr_t>(pic->data());
            if (new_image_cookie != image_cookie) {
                if (image_handle) {
                    nvgDeleteImage(vg, image_handle);
                    image_handle = 0;
                    image_handle = nvgCreateImageRGBA(vg, pic->width(),  pic->height(), 0, pic->data());
                }
                if (image_handle) {
                    image_cookie = new_image_cookie;
                } else {
                    image_cookie = 0;
                }
            }
        }
    }
};

Pic * CreateHSLSpectrum(float saturation);
Pic * CreateHueRamp(int width, int height, bool vertical);
void SetSLSpectrum(Pic* pic, float hue);
Pic * CreateSLSpectrum(float hue, int width, int height);

}