#pragma once
#include <rack.hpp>
namespace pachde {

enum class HAlign { Left, Center, Right };
const char* HAlignName(HAlign h);
char HAlignLetter(HAlign h);
NVGalign nvgAlignFromHAlign(HAlign h);
HAlign parseHAlign(std::string text);

enum class VAlign { Top, Middle, Bottom, Baseline };
NVGalign nvgAlignFromVAlign(VAlign v);
const char* VAlignName(VAlign v);
char VAlignLetter(VAlign v);
VAlign parseVAlign(std::string text);

inline NVGalign NVGalign_from_alignment(HAlign h, VAlign v) { return NVGalign(nvgAlignFromHAlign(h)|nvgAlignFromVAlign(v)); }

enum class Orientation { Normal, Down, Up, Inverted };
const char* OrientationName(Orientation orientation);
const char* OrientationJValue(Orientation orientation);
Orientation ParseOrientation(const char* orient);

}
