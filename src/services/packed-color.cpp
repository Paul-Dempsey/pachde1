#include "packed-color.hpp"

namespace packed_color {

const char * HEX_CHARS = "0123456789abcdef";
inline char hex_hi(uint8_t byte) { return HEX_CHARS[(byte >> 4) & 0x0f]; }
inline char hex_lo(uint8_t byte) { return HEX_CHARS[byte & 0x0f]; }

int hexFormat(PackedColor color, int buffer_size, char * buffer) {
    if (buffer_size < 10) {
        return 0;
    }

    char* poke = buffer;
    *poke++ = '#';
    uint8_t byte;
    // r
    byte = color & 0xff; color >>= 8;
    *poke++ = hex_hi(byte);
    *poke++ = hex_lo(byte);
    // g
    byte = color & 0xff; color >>= 8;
    *poke++ = hex_hi(byte);
    *poke++ = hex_lo(byte);
    // b
    byte = color & 0xff; color >>= 8;
    *poke++ = hex_hi(byte);
    *poke++ = hex_lo(byte);
    // a
    byte = color & 0xff;
    if (byte != 0xff) {
        *poke++ = hex_hi(byte);
        *poke++ = hex_lo(byte);
    }
    *poke = 0;
    return poke-buffer;
}

float clamp(float a, float min, float max) { return a < min ? min : (a > max ? max : a); }

float intermediate_hue(float h, float m1, float m2)
{
	if (h < 0) h += 1;
	if (h > 1) h -= 1;
	if (h < 1.0f/6.0f)
		return m1 + (m2 - m1) * h * 6.0f;
	else if (h < 3.0f/6.0f)
		return m2;
	else if (h < 4.0f/6.0f)
		return m1 + (m2 - m1) * (2.0f/3.0f - h) * 6.0f;
	return m1;
}

PackedColor packHsla(float h, float s, float l, float a)
{
	float m1, m2;
	h = fmodf(h, 1.0f);
	if (h < 0.0f) h += 1.0f;
	s = clamp(s, 0.0f, 1.0f);
	l = clamp(l, 0.0f, 1.0f);
	m2 = l <= 0.5f ? (l * (1 + s)) : (l + s - l * s);
	m1 = 2 * l - m2;
	auto r = static_cast<uint8_t>(255 * clamp(intermediate_hue(h + 1.0f/3.0f, m1, m2), 0.0f, 1.0f));
	auto g = static_cast<uint8_t>(255 * clamp(intermediate_hue(h, m1, m2), 0.0f, 1.0f));
	auto b = static_cast<uint8_t>(255 * clamp(intermediate_hue(h - 1.0f/3.0f, m1, m2), 0.0f, 1.0f));
    auto A = static_cast<uint8_t>(255 * clamp(a, 0.0f, 1.0f));
    return packRgba(r, g, b, A);
}


// true if "rgb(" or "rgba(" prefix
bool is_rgb_prefix(const char * scan)
{
    while (' ' == *scan) scan++;
    if ('r' == *scan++) {
        if ('g' == *scan++) {
            if ('b' == *scan++) {
                if ('a' == *scan) scan++;
                return '(' == *scan;
            }
        }
    }
    return false;
}

// true if "hsl(" or "hsla(" prefix (leading space ok)
bool is_hsl_prefix(const char * scan)
{
    while (' ' == *scan) scan++;
    if ('h' == *scan++) {
        if ('s' == *scan++) {
            if ('l' == *scan++) {
                if ('a' == *scan) scan++;
                return '(' == *scan;
            }
        }
    }
    return false;
}

bool parseColor(PackedColor &result, PackedColor default_value, const char *pos, const char **stop)
{
    if (parseHexColor(result, default_value, pos, stop)) return true;
    if (parseHslaColor(result, default_value, pos, stop)) return true;
    return parseRgbaColor(result, default_value, pos, stop);
}

int hex_value(char ch)
{
    if (ch > 'f' || ch < '0') { return -1; }
    if (ch <= '9') { return ch & 0xF; }
    if (ch < 'A') { return -1; }
    if (ch < 'G') { return ch - 'A' + 10; }
    if (ch < 'a') { return -1; }
    return ch - 'a' + 10;
}

uint64_t parse_uint64(const char * pos, const char **stop)
{
    uint64_t r = 0;
    while (std::isdigit(*pos)) {
        r *= 10;
        r += *pos - '0';
        pos++;
    }
    *stop = pos;
    return r;
}

float parse_float(const char * pos, const char ** stop)
{
    char * end;
    float r = std::strtof(pos, &end);
    if (pos == end) { *stop = pos; return std::nanf(""); }
    *stop = end;
    return r;
}

inline bool is_number_sep(char ch) { return ' ' == ch || ',' == ch; }

bool parse_byte(uint8_t& result, const char *pos, const char **stop)
{
    const char * dummy{nullptr};
    if (!stop) stop = &dummy;

    while (' ' == *pos) pos++;
    uint64_t number = parse_uint64(pos, stop);
    if ((number > 255) || (pos == *stop)) {
        result = 0;
        return false;
    }
    result = number;
    return true;
}

bool parseRgbaColor(PackedColor& result, PackedColor default_value, const char *pos, const char **stop)
{
    const char * dummy{nullptr};
    if (!stop) stop = &dummy;
    result = default_value;

    if (!is_rgb_prefix(pos)) {
        *stop = pos;
        return false;
    }
    while (' ' == *pos) pos++;
    pos += 3; // "rgb"
    bool is_alpha = ('a' == *pos++);
    if (is_alpha) pos++;

    uint8_t r, g, b, a;
    const char * end;

    if (!parse_byte(r, pos, &end)) { *stop = pos; return false; }
    pos = end;

    while (is_number_sep(*pos)) pos++;
    if (!parse_byte(g, pos, &end)) { *stop = pos; return false; }
    pos = end;

    while (is_number_sep(*pos)) pos++;
    if (!parse_byte(b, pos, &end)) { *stop = pos; return false; }
    pos = end;

    a = 0xff;
    if (is_alpha) {
        while (is_number_sep(*pos)) pos++;
        if (!parse_byte(a, pos, &end)) { *stop = pos; return false; }
        pos = end;
    }

    while (' ' == *pos) pos++;
    if (')' == *pos) {
        *stop = pos + 1;
        result =  packRgba(r, b, g, a);
        return true;
    } else {
        *stop = pos;
        return false;
    }
}

bool parse_hsla_part(float &result, const char *pos, const char **stop)
{
    const char * dummy{nullptr};
    if (!stop) stop = &dummy;

    while (is_number_sep(*pos)) pos++;
    const char * end;
    float f = parse_float(pos, &end);
    if ((pos == end) || _isnanf(f)) {
        result = f;
        *stop = pos;
        return false;
    }
    pos = end;
    if ('%' == *pos) {
        pos++;
        result = (f > 100.f) ? 1.f : f * .01;
    } else if (f > 1.0) {
        result = 1.f;
    }
    *stop = pos;
    return true;
}

bool parseHslaColor(PackedColor& result, PackedColor default_value, const char *pos, const char **stop)
{
    const char * dummy{nullptr};
    if (!stop) stop = &dummy;
    result = default_value;

    if (!is_hsl_prefix(pos)) {
        *stop = pos;
        return false;
    }
    while (' ' == *pos) pos++;
    pos += 3; // "hsl"
    bool is_alpha = ('a' == *pos++);
    if (is_alpha) pos++;

    float h, s, l, a;
    const char * end;

    uint64_t number = parse_uint64(pos, &end);
    if (pos == end) { *stop = end; return false; }
    h = static_cast<float>(number % 360)/360.f;
    pos = end;

    if (!parse_hsla_part(s, pos, &end)) { *stop = end; return false; }
    pos = end;

    if (!parse_hsla_part(l, pos, &end)) { *stop = end; return false; }
    pos = end;

    if (is_alpha) {
        if (!parse_hsla_part(a, pos, &end)) { *stop = end; return false; }
        pos = end;
    } else {
        a = 1.0f;
    }
    while (' ' == *pos) pos++;
    if (')' == *pos) {
        pos++;
    } else {
        *stop = pos;
        return false;
    }
    result = packHsla(h, s, l, a);
    *stop = pos;
    return true;
}

// Hex color representations:
//  short: #rgb, #rgba
//  long: #rrggbb, #rrggbbaa

bool parseHexColor(PackedColor& result, PackedColor default_value, const char *pos, const char **stop)
{
    const char * dummy{nullptr};
    if (!stop) stop = &dummy;

    while (' ' == *pos) pos++;
    if ('#' != *pos) {
        result = default_value;
        *stop = pos;
        return false;
    }
    pos++;
    bool short_hex{true};
    bool is_alpha(false);
    auto scan = pos;
    while (hex_value(*scan) > -1) scan++;
    auto len{scan-pos};
    switch (len) {
        case 3: break;
        case 4: is_alpha = true; break;
        case 6: short_hex = false; break;
        case 8: is_alpha = true; short_hex = false; break;
        default:
            *stop = scan;
            result = default_value;
            return false;
    }
    int r,g,b,a;
    a = 0xff;
    if (short_hex) {
        r = hex_value(*pos++);
        g = hex_value(*pos++);
        b = hex_value(*pos++);
        if (is_alpha) {
            a = hex_value(*pos++);
        }
    } else {
        auto hi = hex_value(*pos++);
        r = (hi << 4) | hex_value(*pos++);

        hi = hex_value(*pos++);
        g = (hi << 4) | hex_value(*pos++);

        hi = hex_value(*pos++);
        b = (hi << 4) | hex_value(*pos++);

        if (is_alpha) {
            hi = hex_value(*pos++);
            a = hex_value(*pos++);
        }
    }
    assert(pos == scan);
    result = packRgba(r,g,b,a);
    *stop = pos;
    return true;
}

namespace colors {
    const PackedColor PortRed          {packHsla(0.f, 0.6f, 0.5f, 1.f)};
    const PackedColor PortOrange       {packHsla(30.f/360.f, 0.80f, 0.5f, 1.f)};
    const PackedColor PortYellow       {packHsla(60.f/360.f, 0.65f, 0.5f, 1.f)};
    const PackedColor PortLime         {packHsla(90.f/360.f, 0.60f, 0.5f, 1.f)};
    const PackedColor PortGreen        {packHsla(120.f/360.f, 0.5f, 0.5f, 1.f)};
    const PackedColor PortGrass        {packHsla(150.f/360.f, 0.5f, 0.5f, 1.f)};
    const PackedColor PortCyan         {packHsla(180.f/360.f, 0.5f, 0.5f, 1.f)};
    const PackedColor PortCorn         {packHsla(210.f/360.f, 0.5f, 0.55f, 1.f)};
    const PackedColor PortBlue         {packHsla(240.f/360.f, 0.5f, 0.55f, 1.f)};
    const PackedColor PortViolet       {packHsla(270.f/360.f, 0.5f, 0.5f, 1.f)};
    const PackedColor PortMagenta      {packHsla(300.f/360.f, 0.5f, 0.5f, 1.f)};
    const PackedColor PortPink         {packHsla(330.f/360.f, 0.65f, 0.65f, 1.f)};
    const PackedColor PortDefault      {packHsla(210.f/360.f, 0.5f, 0.65f, 1.f)};
    const PackedColor PortLightOrange  {packHsla(30.f/360.f, 0.80f, 0.75f, 1.f)};
    const PackedColor PortLightLime    {packHsla(90.f/360.f, 0.75f, 0.75f, 1.f)};
    const PackedColor PortLightViolet  {packHsla(270.f/360.f, 0.75f, 0.75f, 1.f)};
}

} // namespace packed_color