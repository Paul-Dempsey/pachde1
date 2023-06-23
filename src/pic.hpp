#pragma once
#include <rack.hpp>
#include "point.hpp"

namespace pachde {
constexpr const float PIC_EPSILON = 0.00001f;

struct Pic {
    int _width = 0;
    int _height = 0;
    int _components = 0;
    unsigned char * _data = nullptr;
    std::string _name;
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
    Point position(unsigned char * location) const;
    unsigned char * pixel_address(int x, int y) const;
    unsigned char * pixel_address(const Point& point) const { return pixel_address(point.x, point.y); }
    NVGcolor pixel(int x, int y) const;
    NVGcolor pixel(const Point& point) const { return pixel(point.x, point.y); }
    NVGcolor pixel(float x, float y) const;
    NVGcolor pixel(rack::Vec pos) const { return pixel(pos.x, pos.y); }

    std::string name() const { return _name; }
    std::string reason() const { return _reason; }
    bool open(std::string filename);
    void close();

    ~Pic() {
        close();
    }
};

// caching of associate nvg structure for a pic
// does not own the pic.
struct cachePic
{
    Pic* pic = nullptr;
    int image_handle = 0; // nvg Image handle
    intptr_t image_cookie = 0; // cookie for image data
    intptr_t vg_cookie = 0; // cookie for graphics context

    cachePic() { }
    cachePic(Pic * pic) { setPic(pic); }

    void setPic(Pic * picture) {
        pic = picture;
        if (!pic || image_cookie != reinterpret_cast<intptr_t>(pic->data())) {
            clearImageCache();
        }
    }
    Pic * getPic() { return pic; }

    int getHandle(NVGcontext* vg) {
        updateImageCache(vg);
        return image_handle;
    }

    void clearImageCache() {
        if (vg_cookie && image_handle) {
            auto vg = reinterpret_cast<NVGcontext*>(vg_cookie);
            nvgDeleteImage(vg, image_handle);
        }
        image_handle = 0;
        vg_cookie = 0;
    }

    void updateImageCache(NVGcontext* vg)
    {
        if (!pic) {
            clearImageCache();
            return;
        }
        auto icookie = reinterpret_cast<intptr_t>(pic->data());
        auto vcookie = reinterpret_cast<intptr_t>(vg);
        if (!image_cookie && !image_handle && !vg_cookie) {
            image_handle = nvgCreateImageRGBA(vg, pic->width(),  pic->height(), 0, pic->data());
            image_cookie = icookie;
            vg_cookie = vcookie;
        } else {
            if (icookie != image_cookie || vcookie != vg_cookie) {
                if (image_handle) {
                    nvgDeleteImage(vg, image_handle);
                    image_handle = 0;
                    image_handle = nvgCreateImageRGBA(vg, pic->width(),  pic->height(), 0, pic->data());
                }
                if (image_handle) {
                    image_cookie = icookie;
                    vg_cookie = vcookie;
                } else {
                    image_cookie = 0;
                    vg_cookie = 0;
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