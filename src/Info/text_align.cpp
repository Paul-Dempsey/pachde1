#include "text_align.hpp"

namespace pachde {

const char* HAlignName(HAlign h) {
    switch (h) {
        default:
        case HAlign::Left: return "Left";
        case HAlign::Center: return "Center";
        case HAlign::Right: return "Right";
    }
}

char HAlignLetter(HAlign h) {
    switch (h) {
        default:
        case HAlign::Left: return 'l';
        case HAlign::Center: return 'c';
        case HAlign::Right: return 'r';
    }
}

NVGalign nvgAlignFromHAlign(HAlign h) {
    switch (h) {
        default:
        case HAlign::Left: return NVGalign::NVG_ALIGN_LEFT;
        case HAlign::Center: return NVGalign::NVG_ALIGN_CENTER;
        case HAlign::Right: return NVGalign::NVG_ALIGN_RIGHT;
    }
}

HAlign parseHAlign(std::string text) {
    if (text.empty()) {
        return HAlign::Left;
    }
    switch (*text.begin()) {
        default:
        case 'l': return HAlign::Left;
        case 'c': return HAlign::Center;
        case 'r': return HAlign::Right;
    }
}

}