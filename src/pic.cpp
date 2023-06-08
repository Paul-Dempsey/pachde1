#include <rack.hpp>
#include "pic.hpp"
#include "stb_image.h"

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
    if (!_data || !location || location >= end()) {
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
        auto value = *pixel;
        return nvgRGBA(value, pixel[1], pixel[2], pixel[3]);
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

constexpr const float EPS = 0.00001f;
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

    if (x - fx < EPS && y - fy < EPS) {
        return pixel(ix1, iy1);
    }
    if (x < 0.0f
        || y < 0.0f 
        || x >= static_cast<float>(_width)
        || y >= static_cast<float>(_height)) {
        return nvgRGBA(0,0,0,0);
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
    // exclude points farther than 1 unit
    std::for_each(weight.begin(), weight.end(), [](float &f){ if (f > 1.0f) f = 0.0f; });

    // compute weights
    float sum = std::accumulate(weight.begin(), weight.end(), 0.0f);
    std::for_each(weight.begin(), weight.end(), [sum](float &f){ f = f/sum; });

    // edge pixels same as adjacent, rather than wrapping or something
    int ix2 = std::min(_width - 1, ix1 + 1);
    int iy2 = std::min(_height - 1, iy1 + 1);
    
    NVGcolor p = nvgRGBA(0,0,0,0);
    auto w = weight[0];
    if (w > 0.0f) {
        p = pixel(ix1, iy1);
        for (auto n = 0; n < 4; ++n) {
            p.rgba[n] *= w;
        }
    }
    w = weight[1];
    if (w > 0.0f) {
        auto color = pixel(ix2, iy1);
        for (auto n = 0; n < 4; ++n) {
            p.rgba[n] += color.rgba[n] * w;
        }
    }
    w = weight[2];
    if (w > 0.0f) {
        auto color = pixel(ix1, iy2);
        for (auto n = 0; n < 4; ++n) {
            p.rgba[n] += color.rgba[n] * w;
        }
    }
    w = weight[3];
    if (w > 0.0f) {
        auto color = pixel(ix2, iy2);
        for (auto n = 0; n < 4; ++n) {
            p.rgba[n] += color.rgba[n] * w;
        }
    }
    return p;
}

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

}