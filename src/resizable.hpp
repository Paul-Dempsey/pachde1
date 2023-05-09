#pragma once
#include <common.hpp>

inline float NearestHp(float x, float minHp = 3) {
    return std::round(std::fmax(x, minHp) / RACK_GRID_WIDTH) * RACK_GRID_WIDTH;
}

