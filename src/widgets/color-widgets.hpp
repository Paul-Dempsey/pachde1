#pragma once
#include <rack.hpp>
using namespace ::rack;
#include "services/colors.hpp"
#include "pic.hpp"
using namespace ::pachde;

namespace widgetry {

void drawCheckers(const rack::widget::Widget::DrawArgs& args, float x, float y, float width, float height);
void drawCheckers(const rack::widget::Widget::DrawArgs& args, float x, float y, float width, float height, const NVGcolor& color);

struct Swatch: Widget {
    PackedColor color{0};
    void draw(const DrawArgs& args) override {
        FillRect(args.vg, 0.f, 0.f, box.size.x*.5f, box.size.y, RampGray(G_WHITE));
        FillRect(args.vg, box.size.x*.5f, 0.f, box.size.x*.5f, box.size.y, RampGray(G_BLACK));
        drawCheckers(args, 0.f, 0.f, box.size.x, box.size.y);
        if (color) {
            FillRect(args.vg, 0.f, 0.f, box.size.x, box.size.y, fromPacked(color));
        }
    }
};

struct SolidSwatch: Widget {
    PackedColor color{0};
    void draw(const DrawArgs& args) override {
        FillRect(args.vg, 0.f, 0.f, box.size.x, box.size.y, fromPacked(opaque(color)));
    }
};

struct AlphaWidget: OpaqueWidget {
    float alpha{1.0f};
    Vec drag_pos;
    std::function<void(float)> clickHandler;

    bool vertical() { return box.size.x <= box.size.y; }
    float getOpacity() { return alpha; }
    void setOpacity(float f) { alpha = clamp(f); }
    void set_handler(std::function<void(float)> callback) {
        clickHandler = callback;
    }
    void onEnter(const EnterEvent &e) override ;
    void onLeave(const LeaveEvent &e) override;
    void onDragMove(const DragMoveEvent& e) override;
    void onButton(const ButtonEvent& e) override;
    void draw(const DrawArgs& args) override;
};

struct HueWidget : OpaqueWidget
{
    std::function<void(float)> clickHandler;

    float hue = 0.f;
    cachePic* ramp = nullptr;
    Vec drag_pos;

    virtual ~HueWidget();

    bool getHue() { return hue; }
    void setHue(float hue) { this->hue = hue; }
    bool vertical() { return box.size.x <= box.size.y; }
    void set_handler(std::function<void(float)> callback) {
        clickHandler = callback;
    }
    cachePic* getHueRamp();
    void onContextCreate(const ContextCreateEvent& e) override;
    void onContextDestroy(const ContextDestroyEvent& e) override;
    void onEnter(const EnterEvent &e) override;
    void onLeave(const LeaveEvent &e) override;
    void onDragMove(const DragMoveEvent& e) override;
    void onButton(const ButtonEvent& e) override;
    void draw(const DrawArgs & args) override;
};

struct SLWidget : OpaqueWidget
{
    float hue = 0.f;
    float sat = .5f;
    float light = .5f;
    cachePic* ramp = nullptr;
    std::function<void(float, float)> clickHandler;
    Vec drag_pos;

    SLWidget() { }
    explicit SLWidget(float hue) { this->hue = hue; }

    virtual ~SLWidget() ;

    float getSaturation() { return sat; }
    float getLightness() { return light; }
    void setSaturation(float s) { sat = s; }
    void setLightness(float l) { light = l; }
    void set_handler(std::function<void(float, float)> callback) {
        clickHandler = callback;
    }
    void setHue(float new_hue);
    cachePic* getRamp();
    void onContextCreate(const ContextCreateEvent& e) override;
    void onContextDestroy(const ContextDestroyEvent& e) override;
    void onEnter(const EnterEvent &e) override;
    void onLeave(const LeaveEvent &e) override;
    void onDragMove(const DragMoveEvent& e) override;
    void onButton(const ButtonEvent& e) override;
    void draw(const DrawArgs& args) override;

};

enum class ColorSyntax { Unknown = -1, Hex, RGB, HSL };

struct SyntaxSelector : OpaqueWidget
{
    using Base = OpaqueWidget;

    ColorSyntax syntax{ColorSyntax::Hex};
    std::function<void(ColorSyntax syntax)> click_handler{nullptr};

    SyntaxSelector() { box.size = Vec(16.f, 14.f); }
    void set_handler(std::function<void(ColorSyntax syntax)> handler) { click_handler = handler; }
    void onButton(const ButtonEvent& e) override;
    void draw(const DrawArgs& args) override;
};


}