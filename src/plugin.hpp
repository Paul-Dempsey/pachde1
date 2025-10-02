#pragma once
#include <rack.hpp>
#include "services/svg-query.hpp"
using namespace svg_query;

// Declare the Plugin, defined in plugin.cpp
extern ::rack::Plugin* pluginInstance;

// Declare each Model, defined in each module source file
extern ::rack::Model* modelBlank;
extern ::rack::Model* modelInfo;
extern ::rack::Model* modelCopper;
extern ::rack::Model* modelCopperMini;
extern ::rack::Model* modelImagine;
extern ::rack::Model* modelSkiff;

