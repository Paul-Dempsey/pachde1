#include "color-widgets.hpp"
#include "services/text.hpp"
namespace widgetry {

void AlphaWidget::onEnter(const EnterEvent &e) {
    OpaqueWidget::onEnter(e);
    glfwSetCursor(APP->window->win, glfwCreateStandardCursor(GLFW_CROSSHAIR_CURSOR));
}
void AlphaWidget::onLeave(const LeaveEvent &e) {
    OpaqueWidget::onLeave(e);
    glfwSetCursor(APP->window->win, NULL);
}

void AlphaWidget::onDragMove(const DragMoveEvent& e) {
    OpaqueWidget::onDragMove(e);
    drag_pos = drag_pos.plus(e.mouseDelta.div(getAbsoluteZoom()));

    setOpacity(vertical() ? (drag_pos.y / box.size.y) : (drag_pos.x / box.size.x));
    alpha = ::rack::math::clamp(alpha);
    if (clickHandler) {
        clickHandler(alpha);
    }
}

void AlphaWidget::onButton(const ButtonEvent& e) {
    OpaqueWidget::onButton(e);
    if (!(e.action == GLFW_PRESS && e.button == GLFW_MOUSE_BUTTON_LEFT && (e.mods & RACK_MOD_MASK) == 0)) {
        return;
    }
    drag_pos = e.pos;
    e.consume(this);
}

void drawCheckers(const rack::widget::Widget::DrawArgs& args, float x, float y, float width, float height, const NVGcolor& color) {
    auto vg = args.vg;

    float dx = 5.f;
    float m = std::min(width, height);
    if (m < 15.f) {
        dx = m / 3.f;
    }

    nvgScissor(vg, x, y, width, height);
    nvgBeginPath(vg);
    int r = 0;
    for (float iy = y; iy < y + height; iy += dx, r++) {
        bool g = (0 == (r & 1));
        for (float ix = x; ix < x + width; ix += dx) {
            if (g) nvgRect(vg, ix, iy, dx, dx);
            g = !g;
        }
    }
    nvgFillColor(vg, color);
    nvgFill(vg);
    nvgResetScissor(vg);
}

void drawCheckers(const rack::widget::Widget::DrawArgs& args, float x, float y, float width, float height) {
    drawCheckers(args, x, y, width, height, nvgRGBAf(.5,.5,.5,1));
}


void AlphaWidget::draw(const DrawArgs& args) {
    auto vg = args.vg;

    auto co_white = RampGray(G_WHITE);
    auto co_black = RampGray(G_BLACK);
    nvgBeginPath(vg);
    nvgRect(vg, 0, 0, box.size.x, box.size.y);
    nvgFillColor(vg, co_white);
    nvgFill(vg);

    drawCheckers(args, 0, 0, box.size.x, box.size.y);

    nvgBeginPath(vg);
    int r = 0;
    for (float y = 0; y < box.size.y; y += 5.f, r++) {
        bool g = (0 == (r & 1));
        for (float x = 0; x < box.size.x; x += 5.f) {
            if (g) nvgRect(vg, x, y, 5.f, 5.f);
            g = !g;
        }
    }
    nvgFillColor(vg, nvgRGBAf(.5,.5f,.5, 1.f));
    nvgFill(vg);

    nvgBeginPath(vg);
    nvgRect(vg, 0, 0, box.size.x, box.size.y);
    auto co_nada = nvgRGBAf(1.f,1.f,1.f,0.f);
    NVGpaint paint = vertical()
        ? nvgLinearGradient(vg, 0, 0, 0, box.size.y, co_nada, co_white)
        : nvgLinearGradient(vg, 0, 0, box.size.x, 0, co_nada, co_white);
    nvgFillPaint(vg, paint);
    nvgFill(vg);

    auto co_dot{COLOR_BRAND};
    if (vertical()) {
        auto y = alpha * box.size.y;
        Circle(vg, box.size.x *.5, y + .375f, 3.f, co_dot);
        Line(vg, 0.f, y, box.size.x, y, co_white, 1.f);
        Line(vg, 0.f, y + .75f, box.size.x, y + .75f, co_black, 1.f);
    } else {
        auto x = alpha * box.size.x;
        Circle(vg, x + .375f, box.size.y*.5f, 3.f, co_dot);
        Line(vg, x, 0.f, x, box.size.y, co_white, 1.f);
        Line(vg, x + .75f, 0.f, x + .75f, box.size.y, co_black, 1.f);
    }
}

//
// HueWidget
//

HueWidget::~HueWidget() {
    if (ramp) {
        auto pic = ramp->getPic();
        delete ramp;
        delete pic;
    }
}

cachePic* HueWidget::getHueRamp() {
    if (nullptr == ramp) {
        ramp = new cachePic(CreateHueRamp(box.size.x, box.size.y, vertical()));
    }
    return ramp;
}

void HueWidget::onContextCreate(const ContextCreateEvent& e) {
    OpaqueWidget::onContextCreate(e);
    if (ramp) {
        ramp->onContextCreate(e);
    }
}

void HueWidget::onContextDestroy(const ContextDestroyEvent& e) {
    if (ramp) {
        ramp->onContextDestroy(e);
    }
    OpaqueWidget::onContextDestroy(e);
}

void HueWidget::onEnter(const EnterEvent &e) {
    OpaqueWidget::onEnter(e);
    glfwSetCursor(APP->window->win, glfwCreateStandardCursor(GLFW_CROSSHAIR_CURSOR));
}

void HueWidget::onLeave(const LeaveEvent &e) {
    OpaqueWidget::onLeave(e);
    glfwSetCursor(APP->window->win, NULL);
}

void HueWidget::onDragMove(const DragMoveEvent& e) {
    OpaqueWidget::onDragMove(e);
    drag_pos = drag_pos.plus(e.mouseDelta.div(getAbsoluteZoom()));

    hue = vertical() ? drag_pos.y / box.size.y : drag_pos.x / box.size.x;
    hue = ::rack::math::clamp(hue);
    if (clickHandler)
        clickHandler(hue);
}

void HueWidget::onButton(const ButtonEvent& e) {
    OpaqueWidget::onButton(e);
    if (!(e.action == GLFW_PRESS && e.button == GLFW_MOUSE_BUTTON_LEFT && (e.mods & RACK_MOD_MASK) == 0)) {
        return;
    }
    e.consume(this);
    drag_pos = e.pos;
}

void HueWidget::draw(const DrawArgs & args) {
    auto vg = args.vg;
    auto image_handle = getHueRamp()->getHandle(vg);
    if (image_handle) {
        nvgBeginPath(vg);
        NVGpaint imgPaint = nvgImagePattern(vg, 0.f, 0.f, box.size.x, box.size.y, 0.f, image_handle, 1.f);
        nvgRect(vg,0.f, 0.f, box.size.x, box.size.y);
        nvgFillPaint(vg, imgPaint);
        nvgFill(vg);
    } else {
        BoxRect(vg, 0.f, 0.f, box.size.x, box.size.y, RampGray(G_85));
    }

    auto co_white = RampGray(G_WHITE);
    auto co_black = RampGray(G_BLACK);
    auto co_dot{COLOR_BRAND};

    if (vertical()) {
        auto y = hue * box.size.y;
        Circle(vg, box.size.x *.5, y + .375f, 3.f, co_dot);
        Line(vg, 0.f, y, box.size.x, y, co_white, 1.f);
        Line(vg, 0.f, y + .75f, box.size.x, y + .75f, co_black, 1.f);
    } else {
        auto x = hue * box.size.x;
        Circle(vg, x + .375f, box.size.y*.5f, 3.f, co_dot);
        Line(vg, x, 0.f, x, box.size.y, co_white, 1.f);
        Line(vg, x + .75f, 0.f, x + .75f, box.size.y, co_black, 1.f);
    }

}

//
// SLWidget Saturation + Lightness
//

SLWidget::~SLWidget() {
    if (ramp) {
        auto pic = ramp->getPic(); // cache doesn't own the pic.
        delete ramp;
        delete pic;
    }
}

void SLWidget::setHue(float new_hue) {
    if (new_hue == hue) return;
    hue = new_hue;
    SetSLSpectrum(getRamp()->getPic(), hue);
    getRamp()->invalidateImage();
}
cachePic* SLWidget::getRamp() {
    if (nullptr == ramp) {
        ramp = new cachePic(CreateSLSpectrum(hue, box.size.x, box.size.y));
    }
    return ramp;
}
void SLWidget::onContextCreate(const ContextCreateEvent& e)
{
    OpaqueWidget::onContextCreate(e);
    if (ramp) {
        ramp->onContextCreate(e);
    }
}
void SLWidget::onContextDestroy(const ContextDestroyEvent& e)
{
    if (ramp) {
        ramp->onContextDestroy(e);
    }
    OpaqueWidget::onContextDestroy(e);
}

void SLWidget::onEnter(const EnterEvent &e) {
    OpaqueWidget::onEnter(e);
    glfwSetCursor(APP->window->win, glfwCreateStandardCursor(GLFW_CROSSHAIR_CURSOR));
}

void SLWidget::onLeave(const LeaveEvent &e) {
    OpaqueWidget::onLeave(e);
    glfwSetCursor(APP->window->win, NULL);
}

void SLWidget::onDragMove(const DragMoveEvent& e) {
    OpaqueWidget::onDragMove(e);
    drag_pos = drag_pos.plus(e.mouseDelta.div(getAbsoluteZoom()));

    sat = drag_pos.x / box.size.x;
    light = 1.f - (drag_pos.y / box.size.y);
    sat = ::rack::math::clamp(sat);
    light = ::rack::math::clamp(light);
    if (clickHandler) {
        clickHandler(sat, light);
    }
}

void SLWidget::onButton(const ButtonEvent& e) {
    OpaqueWidget::onButton(e);
    if (!(e.action == GLFW_PRESS && e.button == GLFW_MOUSE_BUTTON_LEFT && (e.mods & RACK_MOD_MASK) == 0)) {
        return;
    }
    e.consume(this);
    drag_pos = e.pos;
}

void SLWidget::draw(const DrawArgs& args) {
    auto vg = args.vg;
    auto image_handle = getRamp()->getHandle(vg);
    if (image_handle) {
        nvgBeginPath(vg);
        NVGpaint imgPaint = nvgImagePattern(vg, 0.f, 0.f, box.size.x, box.size.y, 0.f, image_handle, 1.f);
        nvgRect(vg,0.f, 0.f, box.size.x, box.size.y);
        nvgFillPaint(vg, imgPaint);
        nvgFill(vg);
    } else {
        BoxRect(vg, 0.f, 0.f, box.size.x, box.size.y, RampGray(G_85));
    }
    auto x = sat * box.size.x;
    auto y = box.size.y - light * box.size.y;
    NVGcolor crosshair = RampGray(G_WHITE);
    crosshair.a = .5f;
    Line(vg, 0.f, y, box.size.x, y, crosshair);
    Line(vg, x, 0.f, x, box.size.y, crosshair);
    auto pix = getRamp()->getPic()->pixel(Vec(x,y));
    Circle(vg, x, y, 5.f, pix);
    OpenCircle(vg, x, y, 5.25, y <  box.size.y/3.f ? RampGray(G_05) : RampGray(G_95), 0.75f);
}

//
// ---- SyntaxSelector --------------------------
//

void SyntaxSelector::onButton(const ButtonEvent &e) {
    Base::onButton(e);
    if (!(e.action == GLFW_PRESS && (e.button == GLFW_MOUSE_BUTTON_LEFT) && (e.mods & RACK_MOD_MASK) == 0)) {
        return;
    }
    switch (syntax) {
        default: syntax = ColorSyntax::Hex; break;
        case ColorSyntax::Hex: syntax = ColorSyntax::RGB; break;
        case ColorSyntax::RGB: syntax = ColorSyntax::HSL; break;
        case ColorSyntax::HSL: syntax = ColorSyntax::Hex; break;
    }
    if (click_handler) click_handler(syntax);
}

void SyntaxSelector::draw(const DrawArgs &args) {
    auto vg = args.vg;

    BoxRect(vg, 0, 0, box.size.x, box.size.y, RampGray(G_50), .75f);

    auto font = GetPluginFontRegular();
    if (!FontOk(font)) return;

    const char * text;
    switch (syntax) {
        default: text = "???"; break;
        case ColorSyntax::Hex: text = "HEX"; break;
        case ColorSyntax::RGB: text = "RGB"; break;
        case ColorSyntax::HSL: text = "HSL"; break;
    }

    nvgFontFaceId(vg, font->handle);
    nvgFontSize(vg, 9.f);
    nvgTextAlign(vg, NVG_ALIGN_CENTER | NVG_ALIGN_TOP);
    nvgFillColor(vg, RampGray(G_75));
    nvgText(vg, box.size.x*.5, 2.5f, text, nullptr);
}

}