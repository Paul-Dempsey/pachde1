#include <rack.hpp>
#include "stb_image.h"
#include "pic.hpp"
#include "../colors.hpp"

using namespace ::rack;
namespace pachde {
    
bool Pic::open(std::string filename)
{
    close();
    _name = filename;
    _data = stbi_load(_name.c_str(), &_width, &_height, &_components, 4);
    if (_data) {
        return true;
    } else {
        _width = _height = 0;
        _reason = stbi_failure_reason();
        return false;
    }
}

unsigned char * Pic::end() const
{ 
    assert(_data);
    return _data + 4 * (_height * _width);
}

unsigned char * Pic::pixel_address(int x, int y) const
{
    if (!_data || x >= _width || y >= _height || x < 0 || y < 0) {
        return nullptr;
    }
    auto row = _data + (stride() * y);
    return row + (x * 4);
}

Point Pic::position(unsigned char * location) const
{
    if (!_data || !location || location < _data || location >= end()) {
        return Point(0,0);
    }

    intptr_t offset = location - _data;
    assert(offset >= 0);

    int y = offset / stride();
    int x = (offset % stride()) / pixel_advance();

    assert(pixel_address(x,y) == location);
    return Point(x,y);
}

NVGcolor Pic::pixel(int x, int y) const
{
    auto pixel = pixel_address(x, y);
    if (pixel) {
        return nvgRGBA(pixel[0], pixel[1], pixel[2], pixel[3]);
        // switch (_components) {
        //     case 1:
        //         return nvgRGB(value, value, value);
        //     case 2: 
        //         return nvgRGBA(value, value, value, pixel[1]);
        //     case 3:
        //         return nvgRGB(value, pixel[1], pixel[2]);
        //     case 4:
        //         return nvgRGBA(value, pixel[1], pixel[2], pixel[3]);
        //     default:
        //         assert(false);
        //         return nvgRGB(value,value,value);
        // }
    } else {
        return nvgRGBA(0,0,0,0);
    }
}

#ifdef USE_SIMD_PIXEL
// Interpolated floating point coordinates
// for same-pixel-dimension image.
NVGcolor Pic::pixel(float x, float y) const
{
    float fx = std::floor(x);
    float fy = std::floor(y);

    // p1 p2
    // p3 p4
    int ix1 = static_cast<int>(fx);
    int iy1 = static_cast<int>(fy);
    auto pixels = pixel_address(ix1, iy1);
    if (!pixels) {
        return nvgRGBA(0,0,0,0);
    }

    if (x - fx < PIC_EPSILON && y - fy < PIC_EPSILON) {
        return pixel(ix1, iy1);
    }

    float a = x - fx;
    float b = 1.0f - a;
    float c = y - fy;
    float d = 1.0f - c;
    std::vector<float> weight = {
        a*a + c*c,
        b*b + c*c,
        a*a + d*d,
        b*b + d*d
    };

    // exclude out of range pixels or distances > 1
    if (weight[0] > 1.f) weight[0] = 0.f;
    if ((ix1 + 1 >= _width) || weight[1] > 1.f) weight[1] = 0.f;
    if ((iy1 + 1 >= _height) || weight[2] > 1.f) weight[2] = 0.f;
    if ((iy1 + 1 >= _height) || (ix1 + 1 >= _width) || weight[3] > 1.f) weight[3] = 0.f;

    // compute weights from squares
    float sum = std::accumulate(weight.begin(), weight.end(), 0.0f);
    // shortcut if only corner pixel is valid
    if (sum == weight[0]) {
        return nvgRGBA(*pixels, *(pixels+1), *(pixels+2), *(pixels+3));
    }
    std::for_each(weight.begin(), weight.end(), [sum](float &f){ f = f/sum; });

    simd::float_4 pd = 0;
    int rowskip = stride();
    unsigned char * quad[4] = { pixels, pixels + 4,  pixels + rowskip, pixels + rowskip + 4};
    for (int i = 0; i < 4; ++i) {
        auto w = weight[i];
        if (w > 0.f) {
            auto px = quad[i];
            simd::float_4 t = { static_cast<float>(*px), static_cast<float>(*(px+1)), static_cast<float>(*(px+2)), static_cast<float>(*(px+3)) };
            t /= 255.f;
            t *= w;
            pd += t;
        }
    }
    return nvgRGBAf(pd[0], pd[1], pd[2], pd[3]);
}
#else
// Interpolated floating point coordinates
// for same-pixel-dimension image.
NVGcolor Pic::pixel(float x, float y) const
{
    float fx = std::floor(x);
    float fy = std::floor(y);

    // p1 p2
    // p3 p4
    int ix1 = static_cast<int>(fx);
    int iy1 = static_cast<int>(fy);
    auto pixels = pixel_address(ix1, iy1);
    if (!pixels) {
        return nvgRGBA(0,0,0,0);
    }

    if (x - fx < PIC_EPSILON && y - fy < PIC_EPSILON) {
        return pixel(ix1, iy1);
    }

    float a = x - fx;
    float b = 1.0f - a;
    float c = y - fy;
    float d = 1.0f - c;
    std::vector<float> weight = {
        a*a + c*c,
        (ix1 + 1 < _width) ? b*b + c*c : 0.f,
        (iy1 + 1 < _height) ? a*a + d*d : 0.f,
        ((iy1 + 1 < _height) && (ix1 + 1 < _width)) ? b*b + d*d : 0.f
    };

    // exclude out of range pixels or distances > 1
    if (weight[0] > 1.f) weight[0] = 0.f;
    if (weight[1] > 1.f) weight[1] = 0.f;
    if (weight[2] > 1.f) weight[2] = 0.f;
    if (weight[3] > 1.f) weight[3] = 0.f;

    // compute weights from squares
    float sum = std::accumulate(weight.begin(), weight.end(), 0.0f);
    // shortuct if only corner pixel is valid
    if (sum == weight[0]) {
        return nvgRGBA(*pixels, *(pixels+1), *(pixels+2), *(pixels+3));
    }
    std::for_each(weight.begin(), weight.end(), [sum](float &f){ f = f/sum; });

    NVGcolor p = nvgRGBA(0,0,0,0);
    auto w = weight[0];
    if (w > 0.0f) {
        p = nvgRGBA(*pixels, *(pixels+1), *(pixels+2), *(pixels+3));
        for (auto n = 0; n < 4; ++n) {
            p.rgba[n] *= w;
        }
    }
    w = weight[1];
    if (w > 0.0f) {
        auto px = pixels + 4;
        for (auto n = 0; n < 4; ++n) {
            p.rgba[n] += *px++/255.0f * w;
        }
    }
    int rowskip = stride();
    w = weight[2];
    if (w > 0.0f) {
        auto px = pixels + rowskip;
        for (auto n = 0; n < 4; ++n) {
            p.rgba[n] += *px++/255.0f * w;
        }
    }
    w = weight[3];
    if (w > 0.0f) {
        auto px = pixels + rowskip + 4;
        for (auto n = 0; n < 4; ++n) {
            p.rgba[n] += *px++/255.0f * w;
        }
    }
    return p;
}
#endif

void Pic::close()
{
    if (_data) {
        auto mem = _data;
        _data = nullptr;
        stbi_image_free(mem);
    }
    _name = "";
    _reason = "";
}

Pic * CreateHSLSpectrum(float saturation)
{
    auto spec = new Pic();
    int w = 256, h = 256;
    spec->_width = w;
    spec->_height = h;
    spec->_data = new unsigned char[w * h * 4];
    unsigned char* rgba = spec->_data;
    for (int y = 0; y < h; ++y) {
        auto L = static_cast<float>(y)/255.f;
        for (int x = 0; x < w; ++x) {
            auto pix = nvgHSL(static_cast<float>(x)/255.f, saturation, L);
            *rgba++ = static_cast<unsigned char>(pix.r * 255.f);
            *rgba++ = static_cast<unsigned char>(pix.g * 255.f);
            *rgba++ = static_cast<unsigned char>(pix.b * 255.f);
            *rgba++ = static_cast<unsigned char>(255);
        }
    }
    return spec;
}

}