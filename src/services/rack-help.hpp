#pragma once
#include <rack.hpp>
using namespace ::rack;
#include "../services/theme.hpp"

namespace pachde {

bool is_singleton(Module*my_module, ModuleWidget* me);

// set displayPrecision = 4
inline ParamQuantity* dp4(ParamQuantity* p) {
    p->displayPrecision = 4;
    return p;
}

// set displayPrecision = 3
inline ParamQuantity* dp3(ParamQuantity* p) {
    p->displayPrecision = 3;
    return p;
}

// set displayPrecision = 2
inline ParamQuantity* dp2(ParamQuantity* p) {
    p->displayPrecision = 2;
    return p;
}

// disable randomization
inline ParamQuantity* no_randomize(ParamQuantity* p) {
    p->randomizeEnabled = false;
    return p;
}

// enable snap (integer rounding)
inline ParamQuantity* snap(ParamQuantity* p) {
    p->snapEnabled = true;
    return p;
}

}