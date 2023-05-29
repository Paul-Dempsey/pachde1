#pragma once

struct SlewLimiter {
	float _delta;
	float _last = 0.0f;

	SlewLimiter(float sampleRate = 1000.0f, float milliseconds = 1.0f, float range = 10.0f) {
		configure(sampleRate, milliseconds, range);
	}

	void configure(float sampleRate = 1000.0f, float milliseconds = 1.0f, float range = 10.0f);
	inline void seed(float last) { _last = last; }
	inline float next(float sample) {
		return _last = next(sample, _last);
	}
	float next(float sample, float last);
};