#include "packed-color.hpp"

namespace packed_color {

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

PackedColor packedFromHSLA(float h, float s, float l, float a)
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
    return packRGBA(r, g, b, A);
}



}