#pragma once
#include <rack.hpp>
using namespace ::rack;
namespace widgetry {

 // addChildBottom an AntiPanel _after_ setPanel to fake out Skiff::dePanel

struct AntiPanel : TransparentWidget {
    void step() override {
        auto p = getParent();
        if (p) {
            box = p->box.zeroPos();
        }
        TransparentWidget::step();
    }
};
}
