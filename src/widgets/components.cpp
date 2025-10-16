#include "components.hpp"
#include "widgetry.hpp"
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
        widgetry::dirtyWidget(widget);
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
        widgetry::dirtyWidget(widget);
    }
}

void AddThemeMenu(rack::ui::Menu *menu, ITheme* it, bool isChangeColor, bool isChangeScrews)
{
    assert(it);
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

} // namespace pachde