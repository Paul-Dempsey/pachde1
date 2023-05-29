#pragma once
#include "plugin.hpp"

// G:\repos\Rack\dep\nanovg\src\nanovg.h

const char * const defaultText = "Add your info here";

struct TextStyle {
    std::string fontpath;
    NVGcolor textColor;
    
};

struct TextBox: OpaqueWidget {
    std::string text = defaultText;

    void draw(const DrawArgs& args);

};
