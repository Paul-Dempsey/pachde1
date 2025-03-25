#pragma once
#ifndef PACHDE_TEXT_ALIGN_HPP_INCLUDED
#define PACHDE_TEXT_ALIGN_HPP_INCLUDED
#include <rack.hpp>

namespace pachde {

enum HAlign { Left, Center, Right };

const char* HAlignName(HAlign h);
char HAlignLetter(HAlign h);
NVGalign nvgAlignFromHAlign(HAlign h);
HAlign parseHAlign(std::string text);

}
#endif