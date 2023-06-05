#pragma once
#include <rack.hpp>
#include "primitives.hpp"

namespace pachde {

class Pic {
    int _width = 0;
    int _height = 0;
    int _components = 0;
    unsigned char * _data = nullptr;
    std::string _name;
    std::string _reason;

    public:
    int width() const { return _width; }
    int height() const { return _height; }
    Point extent() const { return Point(_width, _height); }
    int stride() const { return _width * 4; }
    int pixel_advance() const { return 4; }
    int components() const { return _components; }
    bool ok() const { return nullptr != _data; }
    unsigned char * data() const { return _data; }
    unsigned char * end() const { 
        assert(_data);
        return _data + 4 * _height * _width;
    }
    Point position(unsigned char * location) const {
        assert(_data);
        assert(location);
        assert(location < end());
        intptr_t offset = location - _data;
        assert(offset >= 0);
        int y = offset / stride();
        int x = offset % stride();

        assert(pixel_address(x,y) == location);
        return Point(x,y);
    }
    unsigned char * pixel_address(int x, int y) const;
    NVGcolor pixel(int x, int y) const;

    unsigned char * pixel_address(const Point& point) const { return pixel_address(point.x, point.y); }
    NVGcolor pixel(const Point& point) const { return pixel(point.x, point.y); }

    std::string name() const { return _name; }
    std::string reason() const { return _reason; }
    bool open(std::string filename);
    void close();

    ~Pic() {
        close();
    }
};

}