#include "fader.hpp"

namespace pachde {
void Fader::fade_in(double interval) {
    if (interval > 0) {
        fade = 0.f;
        timer.start(interval);
        fading = Fading::In;
    } else {
        fade = 1.0f;
        fading = Fading::Ready;
    }
}

void Fader::fade_out(double interval) {
    if (interval > 0) {
        timer.start(interval);
        fading = Fading::Out;
    } else {
        fading = Fading::Zero;
        fade = 0.f;
    }
}

void Fader::step_fade() {
    switch (fading) {
    case Fading::Zero:
        fade = 0.f;
        break;

    case Fading::In:
        if (timer.finished()) {
            fade = 1.0f;
            fading = Fading::Ready;
        } else {
            fade = timer.progress();
        }
        break;

    case Fading::Out:
        if (timer.finished()) {
            fade = 0.f;
            fading = Fading::Zero;
        } else {
            fade = 1.f - timer.progress();
        }
        break;

    case Fading::Ready:
        fade = 1.0f;
        break;
    }
}

}