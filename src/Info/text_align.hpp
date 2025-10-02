#pragma once
#include <rack.hpp>
namespace pachde {

enum HAlign { Left, Center, Right };

const char* HAlignName(HAlign h);
char HAlignLetter(HAlign h);
NVGalign nvgAlignFromHAlign(HAlign h);
HAlign parseHAlign(std::string text);

}
