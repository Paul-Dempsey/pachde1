#include "pic.hpp"
#include "stb_image.h"

namespace pachde {
    
bool Pic::open(std::string filename) {
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

unsigned char * Pic::pixel_offset(int x, int y) {
    if (nullptr == _data || x >= _width || y >= _height) return nullptr;
    auto stride = _width * 4; //_components;
    auto row = _data + stride * y;
    return row + x * 4; //_components;
}

NVGcolor Pic::pixel(int x, int y) {
    auto pixel = pixel_offset(x, y);
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

void Pic::close() {
    if (_data) {
        auto mem = _data;
        _data = nullptr;
        stbi_image_free(mem);
    }
    _name = "";
    _reason = "";
}

}