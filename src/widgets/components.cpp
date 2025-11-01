#include "components.hpp"
#include "widgets/color-picker.hpp"
#include "widgets/widgetry.hpp"
using namespace widgetry;
namespace pachde {

void addThemeItems(rack::ui::Menu *menu, ModuleWidget* source, ThemeBase* it) {

    menu->addChild(createMenuItem("Send theme to all pachde1", "", [=](){
        broadcastThemeSetting(source, it->getThemeSetting());
    }));

    NVGcolor co_dot{nvgHSL(200.f/360.f, .5, .5)};
    ColorDotMenuItem* option;

    option = createMenuItem<ColorDotMenuItem>("Follow Rack UI theme", "",
        [it](){ it->setThemeSetting(ThemeSetting::FollowRackUi); }, false);
    option->color = it->getThemeSetting() == ThemeSetting::FollowRackUi ? co_dot : RampGray(G_45);
    menu->addChild(option);

    option = createMenuItem<ColorDotMenuItem>("Follow Rack prefer dark panels", "",
        [it](){ it->setThemeSetting(ThemeSetting::FollowRackPreferDark); }, false);
    option->color = it->getThemeSetting() == ThemeSetting::FollowRackPreferDark ? co_dot : RampGray(G_45);
    menu->addChild(option);

    option = createMenuItem<ColorDotMenuItem>("Light", "",
        [it](){ it->setThemeSetting(ThemeSetting::Light); }, false);
    option->color = it->getThemeSetting() == ThemeSetting::Light ? co_dot : RampGray(G_45);
    menu->addChild(option);

    option = createMenuItem<ColorDotMenuItem>("Dark", "",
        [it](){ it->setThemeSetting(ThemeSetting::Dark); }, false);
    option->color = it->getThemeSetting() == ThemeSetting::Dark ? co_dot : RampGray(G_45);
    menu->addChild(option);

    option = createMenuItem<ColorDotMenuItem>("High Contrast", "",
        [it](){ it->setThemeSetting(ThemeSetting::HighContrast); }, false);
    option->color = it->getThemeSetting() == ThemeSetting::HighContrast ? co_dot : RampGray(G_45);
    menu->addChild(option);
}

void AddThemeMenu(rack::ui::Menu *menu, ModuleWidget* source, ThemeBase* it, bool isChangeColor, bool isChangeScrews, bool submenu)
{
    assert(it);
    if (submenu) {
        menu->addChild(createSubmenuItem("Theme", "", [=](Menu *menu) { addThemeItems(menu, source, it); }));
    } else {
        addThemeItems(menu, source, it);
    }

    if (isChangeColor) {
        menu->addChild(createSubmenuItem("Color", "", [=](Menu* menu) {
            auto picker = new ColorPickerMenu();
            picker->set_color(it->getMainColor());
            picker->set_on_new_color([=](PackedColor color) {
                it->setMainColor(color);
            });
            menu->addChild(picker);
        }));
    }
    if (isChangeScrews) {
        menu->addChild(createCheckMenuItem("Screws", "",
            [=]() { return it->hasScrews(); },
            [=]() { it->setScrews(!it->hasScrews()); }
        ));
    }
}

FancyLabel::FancyLabel() {
    switch (getActualTheme(ThemeSetting::FollowRackUi)) {
    case Theme::Unset:
    case Theme::Light:
        co_bg = nvgRGB(0xf0, 0xf0, 0xf0);
        co_text = color::lerp(co_bg, nvgRGB(0x04, 0x04, 0x04), 0.6);
        break;
    case Theme::Dark:
        co_bg = nvgRGB(0x20, 0x20, 0x20);
        co_text = color::lerp(co_bg, nvgRGB(0xf0, 0xf0, 0xf0), 0.6);
        break;
    case Theme::HighContrast:
        co_bg = nvgRGB(0,0,0);
        co_text = color::lerp(co_bg, nvgRGB(0xff, 0xff, 0xff), 0.8); ;
        break;
    }
}

constexpr const float RULE_Y = 9.f;

void FancyLabel::draw(const DrawArgs& args) {
    auto vg = args.vg;
    auto font = GetPluginFontSemiBold();
    if (!FontOk(font)) return;
    FillRect(vg, 0, 0, box.size.x, box.size.y - 1.f, co_bg);
    SetTextStyle(vg, font, co_text, 16.f);
    nvgTextAlign(vg, NVG_ALIGN_BASELINE | align);
    float bounds[4];
    float x;
    switch (align) {
    case NVG_ALIGN_LEFT:   x = 3.5f; break;
    case NVG_ALIGN_CENTER: x = box.size.x*.5; break;
    case NVG_ALIGN_RIGHT:  x = box.size.x - 3.5; break;
    default: assert(false); break;
    }
    nvgText(vg, x, 14.f, text.c_str(), nullptr);
    if (co_rule.a > 0) {
        nvgTextBounds(vg, x, 14.f, text.c_str(), nullptr, bounds);
        if (bounds[0] - 5 > 3.5) {
            Line(vg, 3.5, RULE_Y, bounds[0] - 5, RULE_Y, co_rule, 1.f);
        } if (bounds[2] + 5 < box.size.x - 3.5) {
            Line(vg, bounds[2] + 5, RULE_Y, box.size.x - 3.5, RULE_Y, co_rule, 1.f);
        }
    }
}


} // namespace pachde