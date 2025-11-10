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

enum OverlayPosition { OnPanel, OnTop };
void add_layered_child(Widget* widget, Widget* child, OverlayPosition position);
ModuleWidget* moduleWidgetAtPos(Vec pos);
bool widget_order_lrtb(const Widget* a, const Widget* b); // for sorting widgets in English reading order (left->right, top->bottom)

enum class AppliesTo {
    All,
    Selected,
    Row,
    RowLeft,
    ContinuousLeft,
    RowRight,
    ContinuousRight,
    Left,
    Right
};

std::vector<ModuleWidget*> getModuleWidgets(ModuleWidget* self, AppliesTo which);

}