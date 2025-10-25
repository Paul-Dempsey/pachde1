#include "screws.hpp"

namespace widgetry {

void SetScrewColors(Widget* widget, PackedColor color, WhichScrew which) {
    if (!widget || widget->children.empty()) return;
    for (Widget * child: widget->children) {
        auto screw = dynamic_cast<ScrewCap*>(child);
        if (screw && isApplicable(screw->which, which)) {
            screw->setMainColor(color);
        }
    }
}

WhichScrew GetScrewPosition(const ScrewCap* screw)
{
    if (isUnknown(screw->which)) {
        // assume relative to parent
        bool left = screw->box.pos.x <= 2.5f * ONE_HP;
        bool top = screw->box.pos.y <= ONE_HP;
        if (left) {
            return top ? WhichScrew::TL : WhichScrew::BL;
        } else {
            return top ? WhichScrew::TR : WhichScrew::BR;
        }
    }
    return screw->which;
}

WhichScrew SetScrewPosition(ScrewCap* screw, WhichScrew which)
{
    return screw->which = isUnknown(which)
        ? GetScrewPosition(screw)
        : which;
}

void AddScrewCaps(Widget *widget, Theme theme, PackedColor color, ScrewAlign align, WhichScrew which)
{
    // top left
    if (which & WhichScrew::TL) {
        auto screw = new ScrewCap(WhichScrew::TL, align);
        screw->setTheme(theme);
        screw->setMainColor(color);
        widget->addChild(screw);
    }

    //top right
    if (which & WhichScrew::TR) {
        auto screw = new ScrewCap(WhichScrew::TR, align);
        screw->setTheme(theme);
        screw->setMainColor(color);
        widget->addChild(screw);
    }

    //bottom left
    if (which & WhichScrew::BL) {
        auto screw = new ScrewCap(WhichScrew::BL, align);
        screw->setTheme(theme);
        screw->setMainColor(color);
        widget->addChild(screw);
    }

    // bottom right
    if (which & WhichScrew::BR) {
        auto screw = new ScrewCap(WhichScrew::BR, align);
        screw->setTheme(theme);
        screw->setMainColor(color);
        widget->addChild(screw);
    }
}

void RemoveScrewCaps(Widget* widget, WhichScrew which)
{
    if (!widget || widget->children.empty()) return;

    std::vector<Widget*> remove;
    for (Widget * child: widget->children) {
        auto screw = dynamic_cast<const ScrewCap*>(child);
        if (screw && isApplicable(GetScrewPosition(screw), which)) {
            remove.push_back(child);
        }
    }
    for (Widget * child: remove) {
        widget->removeChild(child);
        delete child;
    }
}

void DrawScrewCap(NVGcontext * vg, float x, float y, Theme theme, PackedColor color)
{
    NVGcolor a,b,c;
    switch (theme) {
        default:
        case Theme::Unset:
        case Theme::Light:
            a = RampGray(G_60);
            b = RampGray(G_80);
            c = RampGray(G_95);
            break;
        case Theme::Dark:
            a = RampGray(G_05);
            b = RampGray(G_35);
            c = RampGray(G_10);
            break;
        case Theme::HighContrast:
            a = RampGray(G_05);
            b = RampGray(G_35);
            c = RampGray(G_10);
            break;
    }
    if (packed_color::isVisible(color)) {
        c = fromPacked(color);
    }
    nvgBeginPath(vg);
    nvgFillColor(vg, a);
    nvgCircle(vg, 7.5,7.5, 6.2);
    nvgFill(vg);

    nvgBeginPath(vg);
    nvgFillColor(vg, b);
    nvgCircle(vg, 7.5,7.5, 5.7);
    nvgFill(vg);

    nvgBeginPath(vg);
    nvgFillColor(vg, c);
    nvgCircle(vg, 7.5,7.5, 5.2);
    nvgFill(vg);

    nvgBeginPath(vg);
    // light screen to darkened
    auto gradient = nvgLinearGradient(vg, 7.5, 2.3, 7.5, 12.7,
        nvgRGBAf(0.9f,0.9f,0.9f,0.2f),
        nvgRGBAf(0.0,0.0,0.0,0.3f));
    nvgFillPaint(vg, gradient);
    nvgCircle(vg, 7.5,7.5, 5.2);
    nvgFill(vg);
}

}