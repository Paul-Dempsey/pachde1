#pragma once
#include <rack.hpp>

namespace pachde {

struct WallTimer
{
    double start_time{0.0};
    double interval{0.0};

    WallTimer() {}
    WallTimer(double timeout) : interval(timeout) { }

    void stop() { start_time = 0.0; }
    bool stopped() { return start_time <= 0.0; }
    bool running() { return !stopped(); }

    void set_interval(double interval) {
        assert(interval > 0.0);
        this->interval = interval;
    }

    // 0..1
    float progress() {
        if (interval <= 0.0) return 0.0f;
        return static_cast<float>((rack::system::getTime() - start_time) / interval);
    }

    void run() {
        assert(interval > 0.0);
        start_time = rack::system::getTime();
    }

    void start(double interval) {
        assert(interval > 0.0);
        this->interval = interval;
        run();
    }

    // For one-shots: return true when interval has elapsed.
    // Call run() or start() to begin a new interval
    bool finished() { return (rack::system::getTime() - start_time) >= interval; }

    // For periodic intervals: returns true once when time passes the interval and resets.
    bool lap()
    {
        auto current = rack::system::getTime();
        bool lapped = current - start_time >= interval;
        if (lapped) {
            start_time = current;
        }
        return lapped;
    }
};

}