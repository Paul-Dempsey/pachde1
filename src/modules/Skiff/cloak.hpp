
#pragma once
#include <rack.hpp>
using namespace ::rack;
#include "services/packed-color.hpp"
using namespace packed_color;
namespace widgetry {

struct FillData {
    bool enabled{true};
    PackedColor color{0xff808080};
    float fade{1.f};
};

struct LinearGradientData {
    bool enabled{false};
    PackedColor icol{0};
    PackedColor ocol{0xffffffff};
    float ifade{1.f};
    float ofade{1.f};
    float x1{0.f};
    float y1{0.f};
    float x2{0.f};
    float y2{0.f};
};

struct RadialGradientData {
    bool enabled{false};
    PackedColor icol{0xffffffff};
    PackedColor ocol{0};
    float ifade{1.f};
    float ofade{1.f};
    float cx{.5f};
    float cy{.5f};
    float r{.5f};
};

struct BoxGradientData {
    bool enabled{false};
    PackedColor icol{0xff000000};
    PackedColor ocol{0xffffffff};
    float ifade{1.f};
    float ofade{1.f};
    float xshrink{.1f};
    float yshrink{.1f};
    float radius{.2f};
    float feather{.08f};
};

struct CloakBackgroundWidget : Widget
{
    FillData fill;
    LinearGradientData l_grad;
    RadialGradientData r_grad;
    BoxGradientData b_grad;

    void onButton(const ButtonEvent& e) override { e.unconsume(); } // make clicks transparent
    void draw(const DrawArgs& args) override;
};

inline CloakBackgroundWidget * getBackgroundCloak() {
    return APP->scene->rack->getFirstDescendantOfType<CloakBackgroundWidget>();;
}

CloakBackgroundWidget * toggleBackgroundCloak();
CloakBackgroundWidget * ensureBackgroundCloak();

} //widgetry