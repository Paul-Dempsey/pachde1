#pragma once
#include <rack.hpp>

namespace pachde {

class Pic {
    int _width = 0;
    int _height = 0;
    int _components = 0;
    unsigned char * _data = nullptr;
    std::string _name;
    std::string _reason;

    public:
    int width() { return _width; }
    int height() { return _height; }
    int components() { return _components; }
    bool ok() { return nullptr != _data; }
    unsigned char * data() { return _data; }
    unsigned char * pixel_offset(int x, int y);
    NVGcolor pixel (int x, int y);

    std::string name() { return _name; }
    std::string reason() { return _reason; }
    bool open(std::string filename);
    void close();

    ~Pic() {
        close();
    }
};

}