#include "components.hpp"

namespace pachde {

void ThemePanel::draw(const DrawArgs &args)
{
    auto color = theme_holder->getPanelColor();
    if (isColorTransparent(color))
    {
        color = PanelBackground(theme_holder->getTheme());
    }

    nvgBeginPath(args.vg);
    nvgRect(args.vg, 0.0, 0.0, box.size.x, box.size.y);
    nvgFillColor(args.vg, color);
    nvgFill(args.vg);

    Widget::draw(args);
}

void SetChildrenTheme(Widget * widget, Theme theme, bool top)
{
    for (Widget* child : widget->children) {
        auto change = dynamic_cast<ITheme*>(child);
        if (change) {
            change->setTheme(theme);
        }
        if (!child->children.empty()) {
            SetChildrenTheme(child, theme, false);
        }
    }
    if (top) {
        widget::EventContext cDirty;
        widget::Widget::DirtyEvent eDirty;
        eDirty.context = &cDirty;
        widget->onDirty(eDirty);
    }
}

void SetChildrenThemeColor(Widget * widget, NVGcolor color, bool top)
{
    for (Widget* child : widget->children) {
        auto change = dynamic_cast<ITheme*>(child);
        if (change) {
            change->setPanelColor(color);
        }
        if (!child->children.empty()) {
            SetChildrenThemeColor(child, color, false);
        }
    }
    if (top) {
        widget::EventContext cDirty;
        widget::Widget::DirtyEvent eDirty;
        eDirty.context = &cDirty;
        widget->onDirty(eDirty);
    }
}

void SetScrewColors(Widget* widget, NVGcolor color) {
    if (!widget || widget->children.empty()) return;
    for (Widget * child: widget->children) {
        auto screw = dynamic_cast<ScrewCap*>(child);
        if (screw) {
            screw->setPanelColor(color);
        }
    }    
}

void AddScrewCaps(Widget *widget, Theme theme, NVGcolor color)
{
    bool set_color = isColorVisible(color);
    auto screw = new ScrewCap(theme);
    screw->box.pos = Vec(RACK_GRID_WIDTH, 0);
    if (set_color) {
        screw->setPanelColor(color);
    }
    widget->addChild(screw);

    screw = new ScrewCap(theme);
    screw->box.pos = Vec(widget->box.size.x - RACK_GRID_WIDTH * 2, 0);
    if (set_color) {
        screw->setPanelColor(color);
    }
    widget->addChild(screw);

    screw = new ScrewCap(theme);
    screw->box.pos = Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH);
    if (set_color) {
        screw->setPanelColor(color);
    }
    widget->addChild(screw);

    screw = new ScrewCap(theme);
    screw->box.pos = Vec(widget->box.size.x - RACK_GRID_WIDTH * 2, RACK_GRID_HEIGHT - RACK_GRID_WIDTH);
    if (set_color) {
        screw->setPanelColor(color);
    }
    widget->addChild(screw);
}

void RemoveScrewCaps(Widget* widget)
{
    if (!widget || widget->children.empty()) return;

    std::vector<Widget*> remove;
    for (Widget * child: widget->children) {
        auto screw = dynamic_cast<ScrewCap*>(child);
        if (screw) {
            remove.push_back(child);
        }
    }
    for (Widget * child: remove) {
        widget->removeChild(child);
        delete child;
    }
}

void AddThemeMenu(rack::ui::Menu *menu, ITheme* it, bool isChangeColor, bool isChangeScrews) {
    assert(it);
    menu->addChild(new MenuSeparator);
    if (isChangeScrews) {
        menu->addChild(createCheckMenuItem("Screws", "",
            [=]() { return it->hasScrews(); }, 
            [=]() {
                    bool screws = it->hasScrews();
                    it->setScrews(!screws);
                }
            ));
    }
    menu->addChild(createSubmenuItem("Theme", "",
        [=](Menu *menu)
        {
            menu->addChild(createCheckMenuItem(
                "Light", "",
                [=]() { return it->getTheme() == Theme::Light; },
                [=]() { it->setTheme(Theme::Light); }
                ));
            menu->addChild(createCheckMenuItem(
                "Dark", "",
                [=]() { return it->getTheme() == Theme::Dark; },
                [=]() { it->setTheme(Theme::Dark); }
                ));
            menu->addChild(createCheckMenuItem(
                "High contrast", "",
                [=]() { return it->getTheme() == Theme::HighContrast; },
                [=]() { it->setTheme(Theme::HighContrast); }
                ));
        }));

    if (isChangeColor) {
        menu->addChild(createSubmenuItem("Panel color", "",
            [=](Menu *menu)
            {
                EventParamField *editField = new EventParamField();
                editField->box.size.x = 100;
                auto color = it->getPanelColor();
                if (isColorVisible(color)) {
                    editField->setText(rack::color::toHexString(color));
                } else {
                    editField->setText("[#<hex>]");
                }
                editField->changeHandler = [=](std::string text) {
                    auto color = COLOR_NONE;
                    if (!text.empty() && text[0] == '#') {
                        color = rack::color::fromHexString(text);
                    }
                    it->setPanelColor(color);
                };
                menu->addChild(editField);
            }));
    }
}

void DrawScrewCap(NVGcontext * vg, float x, float y, Theme theme, NVGcolor color)
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
    if (isColorVisible(color)) {
        c = color;
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

// draw #d logo
void DrawLogo(NVGcontext * vg, float x, float y, NVGcolor fill, float scale)
{
    bool scaling = scale != 1.0f;
    nvgSave(vg);
    nvgTranslate(vg, x, y);
    if (scaling) {
        nvgScale(vg, scale, scale);
    }
    nvgFillColor(vg, fill);

    // #
    nvgBeginPath(vg);
    nvgMoveTo(vg, 3.023, 5.039);
    nvgLineTo(vg, 3.916, 3.543);
    nvgLineTo(vg, 5.003, 3.834);
    nvgLineTo(vg, 4.104, 5.599);
    nvgLineTo(vg, 5.759, 6.042);
    nvgLineTo(vg, 6.658, 4.277);
    nvgLineTo(vg, 7.74, 4.567);
    nvgLineTo(vg, 6.846, 6.334);
    nvgLineTo(vg, 7.631, 6.544);
    nvgLineTo(vg, 7.34, 7.631);
    nvgLineTo(vg, 6.328, 7.36);
    nvgLineTo(vg, 5.666, 8.665);
    nvgLineTo(vg, 6.969, 9.014);
    nvgLineTo(vg, 6.678, 10.1);
    nvgLineTo(vg, 5.148, 9.691);
    nvgLineTo(vg, 4.255, 11.457);
    nvgLineTo(vg, 3.168, 11.166);
    nvgLineTo(vg, 4.062, 9.4);
    nvgLineTo(vg, 2.407, 8.956);
    nvgLineTo(vg, 1.513, 10.723);
    nvgLineTo(vg, 0.426, 10.431);
    nvgLineTo(vg, 1.32, 8.665);
    nvgLineTo(vg, 0.540, 8.456);
    nvgLineTo(vg, 0.831, 7.369);
    nvgLineTo(vg, 1.843, 7.64);
    nvgLineTo(vg, 2.5, 6.334);
    nvgLineTo(vg, 1.202, 5.986);
    nvgLineTo(vg, 1.493, 4.899);
    nvgClosePath(vg);
    nvgPathWinding(vg, NVGsolidity::NVG_HOLE);
    nvgMoveTo(vg, 5.242, 7.068);
    nvgLineTo(vg, 3.587, 6.625);
    nvgLineTo(vg, 2.924, 7.93);
    nvgLineTo(vg, 4.58, 8.374);
    nvgClosePath(vg);
    nvgFill(vg);

    //d
    nvgBeginPath(vg);
    nvgMoveTo(vg, 13.387,10.647);
    nvgLineTo(vg, 13.086,11.769);
    nvgLineTo(vg, 10.732,11.138);
    nvgLineTo(vg, 10.887,10.559);

    nvgQuadTo(vg, 10.064,11.035, 9.304,10.831);
    nvgQuadTo(vg, 8.394,10.587, 7.993,9.687);
    nvgQuadTo(vg, 7.593,8.787, 7.874,7.736);
    nvgQuadTo(vg, 8.160,6.669, 8.969,6.104);
    nvgQuadTo(vg, 9.78,5.534, 10.665,5.771);
    nvgQuadTo(vg, 11.516,6.0, 11.892,6.807);
    nvgLineTo(vg, 12.306,5.262);
    nvgLineTo(vg, 11.547,5.058);
    nvgLineTo(vg, 11.847,3.937);
    nvgLineTo(vg, 14.161,4.557);
    nvgLineTo(vg, 12.587,10.433);
    nvgClosePath(vg);

    nvgPathWinding(vg, NVGsolidity::NVG_HOLE);
    nvgMoveTo(vg, 11.402,8.697);
    nvgQuadTo(vg, 11.589,7.998, 11.367,7.62);
    nvgQuadTo(vg, 11.15,7.244, 10.767,7.141);
    nvgQuadTo(vg, 10.31,7.019, 9.962,7.308);
    nvgQuadTo(vg, 9.621,7.595, 9.466,8.173);
    nvgQuadTo(vg, 9.32,8.721, 9.465,9.165);
    nvgQuadTo(vg, 9.612,9.603, 10.08,9.729);
    nvgQuadTo(vg, 10.467,9.833, 10.84,9.609);
    nvgQuadTo(vg, 11.219,9.382, 11.402,8.697);
    nvgClosePath(vg);
    nvgFill(vg);

    nvgRestore(vg);
}


} // namespace pachde