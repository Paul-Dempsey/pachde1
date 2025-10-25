#include "components.hpp"
#include "widgetry.hpp"
namespace pachde {

void AddThemeMenu(rack::ui::Menu *menu, ThemeBase* it, bool isChangeColor, bool isChangeScrews)
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
            NVGcolor co_dot{nvgHSL(200.f/360.f, .5, .5)};

            ColorDotMenuItem* option;

            option = createMenuItem<ColorDotMenuItem>("Follow Rack UI thme", "",
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
        }));

    if (isChangeColor) {
        menu->addChild(createSubmenuItem("Panel color", "",
            [=](Menu *menu)
            {
                MenuTextField *editField = new MenuTextField();
                editField->box.size.x = 100;
                auto color = it->getMainColor();
                if (packed_color::visible(color)) {
                    char hex[10];
                    packed_color::hexFormat(color, sizeof(hex), hex);
                    editField->setText(hex);
                } else {
                    editField->setText("#<hex>");
                }
                editField->changeHandler = [=](std::string text) {
                    auto color = colors::G20;
                    if (!text.empty()) {
                        parseColor(color, colors::G20, text.c_str());
                    }
                    it->setMainColor(color);
                };
                menu->addChild(editField);
            }));
    }
}

} // namespace pachde