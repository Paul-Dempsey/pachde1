#include "components.hpp"

namespace pachde {

void ThemePanel::draw(const DrawArgs &args)
{
    Widget::draw(args);

    auto color = theme_holder->getMainColor();
    if (isColorTransparent(color))
    {
        color = PanelBackground(GetPreferredTheme(theme_holder));
    }
    auto vg = args.vg;

    nvgBeginPath(vg);
    nvgRect(vg, 0.0f, 0.0f, box.size.x, box.size.y);
    nvgFillColor(vg, color);
    nvgFill(vg);
}

void SetChildrenTheme(Widget * widget, Theme theme, bool top)
{
    for (Widget* child : widget->children) {
        if (auto itheme = dynamic_cast<IBasicTheme*>(child)) {
            itheme->setTheme(theme);
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
        if (auto itheme = dynamic_cast<IBasicTheme*>(child)) {
            itheme->setMainColor(color);
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

void SetScrewColors(Widget* widget, NVGcolor color, WhichScrew which) {
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

void AddScrewCaps(Widget *widget, Theme theme, NVGcolor color, ScrewAlign align, WhichScrew which)
{
    // top left
    if (which & WhichScrew::TL) {
        auto screw = new ScrewCap(theme, WhichScrew::TL, align);
        screw->setMainColor(color);
        widget->addChild(screw);
    }

    //top right
    if (which & WhichScrew::TR) {
        auto screw = new ScrewCap(theme, WhichScrew::TR, align);
        screw->setMainColor(color);
        widget->addChild(screw);
    }

    //bottom left
    if (which & WhichScrew::BL) {
        auto screw = new ScrewCap(theme, WhichScrew::BL, align);
        screw->setMainColor(color);
        widget->addChild(screw);
    }

    // bottom right
    if (which & WhichScrew::BR) {
        auto screw = new ScrewCap(theme, WhichScrew::BR, align);
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

void AddThemeMenu(rack::ui::Menu *menu, ITheme* it, bool isChangeColor, bool isChangeScrews)
{
    assert(it);
    menu->addChild(new MenuSeparator);
    if (isChangeScrews) {
        menu->addChild(createCheckMenuItem("Screws", "",
            [=]() { return it->hasScrews(); }, 
            [=]() { it->setScrews(!it->hasScrews()); }
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

            menu->addChild(createCheckMenuItem(
                "Follow Rack - Use dark panels", "",
                [=]() { return it->getFollowRack(); },
                [=]() { it->setFollowRack(!it->getFollowRack()); }
                ));
            menu->addChild(createSubmenuItem(
                "Dark panel", "",
                [=](Menu * menu) {
                    menu->addChild(createCheckMenuItem(
                        "Light", "",
                        [=]() { return it->getDarkTheme() == Theme::Light; },
                        [=]() { it->setDarkTheme(Theme::Light); }
                        ));
                    menu->addChild(createCheckMenuItem(
                        "Dark", "",
                        [=]() { return it->getDarkTheme() == Theme::Dark; },
                        [=]() { it->setDarkTheme(Theme::Dark); }
                        ));
                    menu->addChild(createCheckMenuItem(
                        "High contrast", "",
                        [=]() { return it->getDarkTheme() == Theme::HighContrast; },
                        [=]() { it->setDarkTheme(Theme::HighContrast); }
                        ));
                }));
        }));

    if (isChangeColor) {
        menu->addChild(createSubmenuItem("Panel color", "",
            [=](Menu *menu)
            {
                MenuTextField *editField = new MenuTextField();
                editField->box.size.x = 100;
                auto color = it->getMainColor();
                if (isColorVisible(color)) {
                    editField->setText(rack::color::toHexString(color));
                } else {
                    editField->setText("#<hex>");
                }
                editField->changeHandler = [=](std::string text) {
                    auto color = COLOR_NONE;
                    if (!text.empty() && text[0] == '#') {
                        color = rack::color::fromHexString(text);
                    }
                    it->setMainColor(color);
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
    nvgMoveTo(vg, 3.0225791, 5.3090902);
    nvgLineTo(vg, 3.9164512, 3.5427485);
    nvgLineTo(vg, 5.0031212, 3.8339209);
    nvgLineTo(vg, 4.1042182, 5.5989145);
    nvgLineTo(vg, 5.7593776, 6.0424131);
    nvgLineTo(vg, 6.6582806, 4.2774195);
    nvgLineTo(vg, 7.7399197, 4.5672439);
    nvgLineTo(vg, 6.8460476, 6.3335855);
    nvgLineTo(vg, 7.6308648, 6.5438766);
    nvgLineTo(vg, 7.3396924, 7.6305466);
    nvgLineTo(vg, 6.3284856, 7.3595946);
    nvgLineTo(vg, 5.6659891, 8.6649013);
    nvgLineTo(vg, 6.9689869, 9.0140385);
    nvgLineTo(vg, 6.6778145, 10.100708);
    nvgLineTo(vg, 5.1484271, 9.6909104);
    nvgLineTo(vg, 4.254555, 11.457252);
    nvgLineTo(vg, 3.1678851, 11.16608);
    nvgLineTo(vg, 4.0617572, 9.399738);
    nvgLineTo(vg, 2.4065978, 8.9562394);
    nvgLineTo(vg, 1.5127257, 10.722581);
    nvgLineTo(vg, 0.4260557, 10.431409);
    nvgLineTo(vg, 1.3199278, 8.665067);
    nvgLineTo(vg, 0.54014147, 8.4561239);
    nvgLineTo(vg, 0.83131382, 7.3694539);
    nvgLineTo(vg, 1.8425206, 7.640406);
    nvgLineTo(vg, 2.4999863, 6.3337512);
    nvgLineTo(vg, 1.2020194, 5.985962);
    nvgLineTo(vg, 1.4931917, 4.899292);
    nvgClosePath(vg);
    nvgPathWinding(vg, NVGsolidity::NVG_HOLE);
    nvgMoveTo(vg, 5.2418157, 7.0684222);
    nvgLineTo(vg, 3.5866563, 6.6249236);
    nvgLineTo(vg, 2.9241597, 7.9302303);
    nvgLineTo(vg, 4.5793191, 8.3737289);
    nvgClosePath(vg);
    nvgFill(vg);

    //d
    nvgBeginPath(vg);
    nvgMoveTo(vg, 13.386835,10.647075);
    nvgLineTo(vg, 13.086226,11.768961);
    nvgLineTo(vg, 10.731774,11.138087);
    nvgLineTo(vg, 10.886797,10.559536);

    nvgQuadTo(vg, 10.063919,11.034625, 9.3042558,10.831074);
    nvgQuadTo(vg, 8.3936666,10.587083, 7.9931509,9.6871288);
    nvgQuadTo(vg, 7.5926352,8.7871751, 7.8743714,7.7357213);
    nvgQuadTo(vg, 8.1601517,6.6691748, 8.9693819,6.1041558);
    nvgQuadTo(vg, 9.7799602,5.5341058, 10.665395,5.7713573);
    nvgQuadTo(vg, 11.515614,5.9991727, 11.89242,6.8065002);
    nvgLineTo(vg, 12.306262,5.2620202);
    nvgLineTo(vg, 11.546599,5.0584691);
    nvgLineTo(vg, 11.847208,3.936583);
    nvgLineTo(vg, 14.161412,4.5566722);
    nvgLineTo(vg, 12.586925,10.43274);
    nvgClosePath(vg);

    nvgPathWinding(vg, NVGsolidity::NVG_HOLE);
    nvgMoveTo(vg, 11.402005,8.6971241);
    nvgQuadTo(vg, 11.58938,7.9978318, 11.36706,7.6201287);
    nvgQuadTo(vg, 11.149771,7.2437736, 10.767424,7.1413241);
    nvgQuadTo(vg, 10.309614,7.0186543, 9.962361,7.3084457);
    nvgQuadTo(vg, 9.6214865,7.5945543, 9.4664642,8.1731054);
    nvgQuadTo(vg, 9.31953,8.7214713, 9.4649416,9.1648403);
    nvgQuadTo(vg, 9.612,9.603, 10.08,9.729);
    nvgQuadTo(vg, 10.467,9.833, 10.84,9.609);
    nvgQuadTo(vg, 11.219,9.382, 11.402,8.697);
    nvgClosePath(vg);
    nvgFill(vg);

    nvgRestore(vg);
}


} // namespace pachde