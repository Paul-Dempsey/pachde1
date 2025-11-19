#include "text-align.hpp"

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
    case HAlign::Left: return 'l';
    default:
    case HAlign::Center: return 'c';
    case HAlign::Right: return 'r';
    }
}

NVGalign nvgAlignFromHAlign(HAlign h) {
    switch (h) {
    case HAlign::Left: return NVGalign::NVG_ALIGN_LEFT;
    default:
    case HAlign::Center: return NVGalign::NVG_ALIGN_CENTER;
    case HAlign::Right: return NVGalign::NVG_ALIGN_RIGHT;
    }
}

HAlign parseHAlign(std::string text) {
    if (text.empty()) return HAlign::Center;
    switch (*text.begin()) {
    case 'l': return HAlign::Left;
    default:
    case 'c': return HAlign::Center;
    case 'r': return HAlign::Right;
    }
}

NVGalign nvgAlignFromVAlign(VAlign v)
{
    switch (v) {
    case VAlign::Top: return NVGalign::NVG_ALIGN_TOP;
    case VAlign::Middle: return NVGalign::NVG_ALIGN_MIDDLE;
    case VAlign::Bottom: return NVGalign::NVG_ALIGN_BOTTOM;
    default:
    case VAlign::Baseline: return NVGalign::NVG_ALIGN_BASELINE;
    }
}

const char * VAlignName(VAlign v) {
    switch (v) {
    default: return nullptr;
    case VAlign::Top: return "Top";
    case VAlign::Middle: return "Middle";
    case VAlign::Bottom: return "Bottom";
    case VAlign::Baseline: return "Baseline";
    }
}

char VAlignLetter(VAlign v) {
    switch (v) {
    default: return '?';
    case VAlign::Top: return 't';
    case VAlign::Middle: return 'm';
    case VAlign::Bottom: return 'b';
    case VAlign::Baseline: return 's';
    }
    return 0;
}

VAlign parseVAlign(std::string text)
{
    if (text.empty()) return VAlign::Baseline;
    switch (*text.begin()) {
    case 't': return VAlign::Top;
    case 'm': return VAlign::Middle;
    case 'b': return VAlign::Bottom;
    default:
    case 's': return VAlign::Baseline;
    }
}

const char* OrientationName(Orientation orientation) {
    switch (orientation) {
    default:
    case Orientation::Normal: return "Normal";
    case Orientation::Down: return "Down (right)";
    case Orientation::Up: return "Up (left)";
    case Orientation::Inverted: return "Inverted";
    }
}
const char* OrientationJValue(Orientation orientation) {
    switch (orientation ) {
    default:
    case Orientation::Normal: return "normal";
    case Orientation::Down: return "down";
    case Orientation::Up: return "up";
    case Orientation::Inverted: return "invert";
    }
}
Orientation ParseOrientation(const char* orient){
    if (!orient) return Orientation::Normal;
    switch (*orient) {
    default:
    case 'n': return Orientation::Normal;
    case 'd': return Orientation::Down;
    case 'u': return Orientation::Up;
    case 'i': return Orientation::Inverted;
    }
}

}