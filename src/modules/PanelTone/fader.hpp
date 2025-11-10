#pragma once
#include "services/wall-timer.hpp"

namespace pachde {

enum class Fading { Zero, In, Out, Ready };
struct Fader
{
    float fade{0};
    Fading fading{Fading::Zero};
    WallTimer timer;

    void fade_in(double interval);
    void fade_out(double interval);
    void step_fade();
};

}