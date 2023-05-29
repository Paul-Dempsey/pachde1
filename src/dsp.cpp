#include <rack.hpp>
#include "dsp.hpp"

void SlewLimiter::configure(float sampleRate, float milliseconds, float range) {
	assert(sampleRate > 0.0f);
	assert(milliseconds >= 0.0f);
	assert(range > 0.0f);
	_delta = range / ((milliseconds / 1000.0f) * sampleRate);
}

float SlewLimiter::next(float sample, float last) {
	return sample > last
        ? std::min(last + _delta, sample)
        : std::max(last - _delta, sample);
}