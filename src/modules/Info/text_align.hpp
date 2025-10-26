#pragma once
#include <rack.hpp>
namespace pachde {

enum class HAlign { Left, Center, Right };
const char* HAlignName(HAlign h);
char HAlignLetter(HAlign h);
NVGalign nvgAlignFromHAlign(HAlign h);
HAlign parseHAlign(std::string text);

enum class Orientation { Normal, Down, Up, Inverted };
const char* OrientationName(Orientation orientation);
const char*  OrientationJValue(Orientation orientation);
Orientation ParseOrientation(const char* orient);

}
