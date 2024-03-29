#pragma once
namespace pachde {

// TODO: try replacing with one of the VCV slew limiters
struct SlewLimiter {
	float _delta;
	float _last = 0.0f;

	explicit SlewLimiter(float sampleRate = 1000.0f, float milliseconds = 1.0f, float range = 10.0f) {
		configure(sampleRate, milliseconds, range);
	}

	void configure(float sampleRate = 1000.0f, float milliseconds = 1.0f, float range = 10.0f);
	inline void seed(float last) { _last = last; }
	inline float next(float sample) {
		return _last = next(sample, _last);
	}
	float next(float sample, float last);
};

struct RateTrigger
{
    float rate_ms;
    int steps;
    int step_trigger;

    explicit RateTrigger(float rate = 2.5f)
    {
        configure(rate);
        steps = 0;
    }

    void configure(float rate) {
        rate_ms = rate;
        onSampleRateChanged();
    }

    // after reset, fires on next step
    void reset() { steps = step_trigger; }
    void start() { steps = 0; }

    void onSampleRateChanged()
    {
        step_trigger = APP->engine->getSampleRate() * (rate_ms / 1000.0f);
    }

    bool process()
    {
        ++steps;
        if (steps >= step_trigger)
        {
            steps = 0;
            return true;
        }
        return false;
    }
};

// Look back at up to the last S values
template <typename T, size_t S>
struct LookbackBuffer
{
    bool filled = false;
    T data[2 * S];
    T* begin = data;
    T* end   = data + S;

    LookbackBuffer() {
        std::memset(data, 0, 2*S*sizeof(T));
    }

    void push(T v)
    {
        ++begin;
        ++end;
        if (end >= data + 2*S) {
            filled = true;
            begin = data;
            end = data + S;
        }
        *begin = *end = v;
    }
    bool isFilled() { return filled; }

    T* lastN(size_t n = S) {
        return  (n > S) ? nullptr : begin + (S - n);
    }

    float minimum() {
        T* p = begin;
        auto f = *p++;
        while (p <= end ){
            f = std::min(f, *p++);
        }
        return f;
    }

    float maximum() {
        T* p = begin;
        auto f = *p++;
        while (p <= end) {
            f = std::max(f, *p++);
        }
        return f;
    }
    float spread() {
        T* p = begin;
        auto f1 = *p;
        auto f2 = *p;
        ++p;
        while (p <= end) {
            f1 = std::max(f1, *p++);
            f2 = std::min(f2, *p++);
        }
        return f1 - f2;
    }
};

}