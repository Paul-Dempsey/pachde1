#pragma once
#include <rack.hpp>

// packed colors are 8-bit (0-255) abgr packed into an unsigned int
// This is the color format used by nanoSvg
namespace packed_color {

typedef uint32_t PackedColor;

namespace colors {
    constexpr const PackedColor NoColor{0};
    // opaque grays
    constexpr const PackedColor G0{0xff000000};
    constexpr const PackedColor G5{0xff0d0d0d};
    constexpr const PackedColor G10{0xff1a1a1a};
    constexpr const PackedColor G15{0xff262626};
    constexpr const PackedColor G20{0xff333333};
    constexpr const PackedColor G25{0xff404040};
    constexpr const PackedColor G30{0xff4d4d4d};
    constexpr const PackedColor G35{0xff595959};
    constexpr const PackedColor G40{0xff666666};
    constexpr const PackedColor G45{0xff737373};
    constexpr const PackedColor G50{0xff808080};
    constexpr const PackedColor G55{0xff8c8c8c};
    constexpr const PackedColor G60{0xff999999};
    constexpr const PackedColor G65{0xffa6a6a6};
    constexpr const PackedColor G70{0xffb2b2b2};
    constexpr const PackedColor G75{0xffbfbfbf};
    constexpr const PackedColor G80{0xffcccccc};
    constexpr const PackedColor G85{0xffd9d9d9};
    constexpr const PackedColor G90{0xffe5e5e5};
    constexpr const PackedColor G95{0xfff2f2f2};
    constexpr const PackedColor G100{0xffffffff};
}

namespace constants_ {
    constexpr const uint32_t ALPHA_MASK{0xff000000};
    constexpr const uint32_t COLOR_MASK{0x00ffffff};
    constexpr const float BYTE2F{1.f/255.f};
}

inline bool isVisible(PackedColor co) {
    using namespace constants_;
    return 0 != (co & ALPHA_MASK);
}

// r,g,b assumed to be in range 0-255
inline PackedColor packRGB(unsigned int r, unsigned int g, unsigned int b) {
    using namespace constants_;
    return r | (g << 8u) | (b << 16u) | ALPHA_MASK;
}

// r,g,b,a assumed to be in range 0-255
inline PackedColor packRGBA(unsigned int r, unsigned int g, unsigned int b, unsigned int a) {
    return r | (g << 8u) | (b << 16u) | (a << 24u);
}

// Color without alpha
inline PackedColor opaque(PackedColor co) {
    using namespace constants_;
    return co | ALPHA_MASK;
}

// zero to one (float) alpha from color
inline float alpha(PackedColor co) {
    using namespace constants_;
    return ((co & ALPHA_MASK) >> 24u) * BYTE2F;
}

// Color with alpha.
// Ignores existing transparency and assumes alpha in the range zero to one.
inline PackedColor transparent(PackedColor co, float alpha) {
    using namespace constants_;
    return (co & COLOR_MASK) | (static_cast<uint32_t>(alpha * 255.f) << 24u);
}

PackedColor packedFromHSLA(float h, float s, float l, float a);


}