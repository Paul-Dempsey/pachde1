#pragma once
#ifndef PACHDE_PLUGIN_HPP_INCLUDED
#define PACHDE_PLUGIN_HPP_INCLUDED
#include <rack.hpp>

// Declare the Plugin, defined in plugin.cpp
extern ::rack::Plugin* pluginInstance;

// Declare each Model, defined in each module source file
extern ::rack::Model* modelBlank;
extern ::rack::Model* modelInfo;
extern ::rack::Model* modelCopper;
extern ::rack::Model* modelImagine;

namespace pachde {

inline bool GetBool(json_t* root, const char* key, bool default_value) {
    auto j = json_object_get(root, key);
    return j ? json_is_true(j) : default_value;
}

inline float GetFloat(json_t* root, const char* key, float default_value) {
    auto j = json_object_get(root, key);
    return j ? json_real_value(j) : default_value;
}

}
#endif