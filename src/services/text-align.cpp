#include "text-align.hpp"

namespace pachde {

const char* HAlignName(HAlign h) {
    switch (h) {
    case HAlign::Left: return "Left";
    case HAlign::Center: return "Center";
    case HAlign::Right: return "Right";
    }
    return "Left";
}

char HAlignLetter(HAlign h) {
    switch (h) {
    case HAlign::Left: return 'l';
    case HAlign::Center: return 'c';
    case HAlign::Right: return 'r';
    }
    return 'c';
}

NVGalign nvgAlignFromHAlign(HAlign h) {
    switch (h) {
    case HAlign::Left: return NVGalign::NVG_ALIGN_LEFT;
    case HAlign::Center: return NVGalign::NVG_ALIGN_CENTER;
    case HAlign::Right: return NVGalign::NVG_ALIGN_RIGHT;
    }
    return NVGalign::NVG_ALIGN_CENTER;
}

HAlign parseHAlign(std::string text) {
    if (text.empty()) return HAlign::Center;
    switch (*text.begin()) {
    case 'l': case 'L': return HAlign::Left;
    case 'c': case 'C': return HAlign::Center;
    case 'r': case 'R': return HAlign::Right;
    }
    return HAlign::Center;
}

NVGalign nvgAlignFromVAlign(VAlign v)
{
    switch (v) {
    case VAlign::Top: return NVGalign::NVG_ALIGN_TOP;
    case VAlign::Middle: return NVGalign::NVG_ALIGN_MIDDLE;
    case VAlign::Bottom: return NVGalign::NVG_ALIGN_BOTTOM;
    case VAlign::Baseline: return NVGalign::NVG_ALIGN_BASELINE;
    }
    return NVGalign::NVG_ALIGN_BASELINE;
}

const char * VAlignName(VAlign v) {
    switch (v) {
    case VAlign::Top: return "Top";
    case VAlign::Middle: return "Middle";
    case VAlign::Bottom: return "Bottom";
    case VAlign::Baseline: return "Baseline";
    }
    return nullptr;
}

char VAlignLetter(VAlign v) {
    switch (v) {
    case VAlign::Top: return 't';
    case VAlign::Middle: return 'm';
    case VAlign::Bottom: return 'b';
    case VAlign::Baseline: return 's';
    }
    return '?';
}

VAlign parseVAlign(std::string text)
{
    if (text.empty()) return VAlign::Baseline;
    switch (*text.begin()) {
        case 'b': case 'B':
            if (1 == text.size()) return VAlign::Bottom;
            switch (text[1]) {
                case 'o': case 'O': return VAlign::Bottom;
                case 'a': case 'A': return VAlign::Baseline;
            }
            break;
        case 'l': case 'L': return VAlign::Baseline;
        case 'm': case 'M': return VAlign::Middle;
        default:
        case 's': case 'S': return VAlign::Baseline;
        case 't': case 'T': return VAlign::Top;
    }
    return VAlign::Baseline;
}

const char* OrientationName(Orientation orientation) {
    switch (orientation) {
    case Orientation::Normal: return "Normal";
    case Orientation::Down: return "Down (right)";
    case Orientation::Up: return "Up (left)";
    case Orientation::Inverted: return "Inverted";
    }
    return "Normal";
}

const char* OrientationJValue(Orientation orientation) {
    switch (orientation ) {
    case Orientation::Normal: return "normal";
    case Orientation::Down: return "down";
    case Orientation::Up: return "up";
    case Orientation::Inverted: return "invert";
    }
    return "normal";
}
Orientation parseOrientation(std::string text){
    if (text.empty()) return Orientation::Normal;
    switch (text[0]) {
    case 'd': case 'D': return Orientation::Down;
    case 'i': case 'I': return Orientation::Inverted;
    case 'n': case 'N': return Orientation::Normal;
    case 'u': case 'U': return Orientation::Up;
    }
    return Orientation::Normal;
}

}