
#pragma once
#include <rack.hpp>
using namespace ::rack;
#include "services/packed-color.hpp"
using namespace packed_color;
namespace widgetry {

struct FillData {
    bool enabled{true};
    PackedColor color{0x5c2393c4};
    float fade{1.f};
    void init(const FillData& source) {
        enabled = source.enabled;
        color = source.color;
        fade = source.fade;
    }
};

struct LinearGradientData {
    bool enabled{false};
    PackedColor icol{colors::White};
    PackedColor ocol{colors::Black};
    float ifade{1.f};
    float x1{0.f};
    float y1{0.f};
    float ofade{1.f};
    float x2{0.f};
    float y2{0.f};
    void init(const LinearGradientData& source) {
        enabled = source.enabled;
        icol = source.icol;
        ocol = source.ocol;
        ifade = source.ifade;
        x1 = source.x1;
        y1 = source.y1;
        ofade = source.ofade;
        x2 = source.x2;
        y2 = source.y2;
    }
};

struct RadialGradientData {
    bool enabled{false};
    PackedColor icol{0};
    PackedColor ocol{colors::Black};
    float ifade{1.f};
    float cx{.5f};
    float cy{.5f};
    float ofade{1.f};
    float radius{.5f};
    void init(const RadialGradientData& source) {
        enabled = source.enabled;
        icol = source.icol;
        ocol = source.ocol;
        ifade = source.ifade;
        cx = source.cx;
        cy = source.cy;
        ofade = source.ofade;
        radius = source.radius;
    }
};

struct BoxGradientData {
    bool enabled{false};
    PackedColor icol{0};
    PackedColor ocol{colors::Black};
    float ifade{1.f};
    float xshrink{.1f};
    float yshrink{.1f};
    float ofade{1.f};
    float radius{.2f};
    float feather{.08f};
    void init(const BoxGradientData& source) {
        enabled = source.enabled;
        icol = source.icol;
        ocol = source.ocol;
        ifade = source.ifade;
        xshrink = source.xshrink;
        yshrink = source.yshrink;
        ofade = source.ofade;
        radius = source.radius;
        feather = source.feather;
    }
};

struct CloakData {
    FillData fill;
    LinearGradientData linear;
    RadialGradientData radial;
    BoxGradientData boxg;

    void init(const CloakData& source) {
        fill.init(source.fill);
        linear.init(source.linear);
        radial.init(source.radial);
        boxg.init(source.boxg);
    }
};

struct CloakBackgroundWidget;

struct ICloakBackgroundClient {
    virtual void onDeleteCloak(CloakBackgroundWidget* cloak) = 0;
};

struct CloakBackgroundWidget : Widget
{
    CloakData data;
    std::vector<ICloakBackgroundClient*> clients;

    ~CloakBackgroundWidget();
    CloakBackgroundWidget() {};
    CloakBackgroundWidget(CloakData* cd) {
        box = Rect{Vec{0.f,0.f}, Vec{INFINITY,INFINITY}};
        if (cd) data.init(*cd);
    }
    void init(const CloakData& source) { data.init(source); }

    void add_client(ICloakBackgroundClient* client);
    void remove_client(ICloakBackgroundClient* client);

    void onButton(const ButtonEvent& e) override { e.unconsume(); } // make clicks transparent
    void draw_fill(const DrawArgs &args);
    void draw_linear(const DrawArgs &args);
    void draw_radial(const DrawArgs &args);
    void draw_box(const DrawArgs &args);
    void draw(const DrawArgs& args) override;
};

inline CloakBackgroundWidget * getBackgroundCloak() {
    return APP->scene->getFirstDescendantOfType<CloakBackgroundWidget>();
}

CloakBackgroundWidget * ensureBackgroundCloak(Widget*host, CloakData* data);

} //widgetry