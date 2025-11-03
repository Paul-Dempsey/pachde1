
#pragma once
#include <rack.hpp>
using namespace ::rack;

namespace widgetry {

/* todo: separate module for cloaking?
    cloak options:
        linear/radial gradient
        x/y positions in %
        animate gradient ends/center
        light/dark
        color + transparency

*/
struct CloakBackgroundWidget : Widget
{
    void onButton(const ButtonEvent& e) override { e.unconsume(); } // make clicks transparent
    void draw(const DrawArgs& args) override;
};

inline CloakBackgroundWidget * getBackgroundCloak() {
    return APP->scene->rack->getFirstDescendantOfType<CloakBackgroundWidget>();;
}

CloakBackgroundWidget * toggleBackgroundCloak();
CloakBackgroundWidget * ensureBackgroundCloak();

} //widgetry