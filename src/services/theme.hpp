#pragma once
#include <rack.hpp>
#include "./colors.hpp"
using namespace ::rack;
namespace pachde {

enum class Theme {
    Unset = 0,
    Light = 1,
    Dark = 2,
    HighContrast = 3
};

Theme ParseThemeName(const std::string& name);
const char * ThemeName(Theme choice);

enum class ThemeSetting {
    Light,
    Dark,
    HighContrast,
    FollowRackUi,
    FollowRackPreferDark
};

Theme getActualTheme(ThemeSetting choice);
const ThemeSetting DefaultThemeSetting{ThemeSetting::FollowRackPreferDark};
ThemeSetting ParseThemeSettingShorthand(std::string text);
ThemeSetting ThemeSettingFromJson(json_t * root);

NVGcolor PanelBackground(Theme theme);
NVGcolor ThemeTextColor(Theme theme);
NVGcolor OutputBackground(Theme theme);
NVGcolor LogoColor(Theme theme);
inline bool IsDarker(Theme theme) { return theme > Theme::Light; }
inline bool IsLighter(Theme theme) { return theme <= Theme::Light; }

struct ISetTheme {
    virtual void setTheme(Theme theme) = 0;
};

struct ISetColor {
    virtual void setMainColor(PackedColor color) = 0;
    virtual PackedColor getMainColor() = 0;
};

void sendChildrenTheme(Widget *widget, Theme theme);
void sendChildrenThemeColor(Widget *widget, Theme theme, PackedColor main);

// theme + main color for widgets
struct IBasicTheme: ISetColor
{
    ThemeSetting theme_setting{DefaultThemeSetting};
    Theme actual_theme;
    PackedColor main_color = colors::NoColor;

    virtual ~IBasicTheme() {}
    virtual Theme getTheme() { return actual_theme; }
    virtual void setThemeSetting(ThemeSetting new_theme) {
        theme_setting = new_theme;
        actual_theme = getActualTheme(theme_setting);
    }
    virtual ThemeSetting getThemeSetting() { return theme_setting; };
    virtual void setMainColor(PackedColor color) override { main_color = color; }
    virtual PackedColor getMainColor() override { return main_color; }
};

// theme + main color + screws
struct ITheme : IBasicTheme
{
    virtual ~ITheme() {}
    virtual void setScrews(bool showScrews) {}
    virtual bool hasScrews() { return true; }
};

enum class ChangedItem {
    Theme,
    MainColor,
    Screws
};

struct IThemeChange
{
    virtual void onChangeTheme(ChangedItem item) = 0;
};

struct ThemeBase: ITheme
{
    bool screws{true};
    IThemeChange * notify{nullptr};

    virtual ~ThemeBase() {}

    void reset();
    void randomize();

    // Rack sends no notification for changes in theme settings, so we must poll.
    // Returns true and notifies with ChangedItem::Theme if changed.
    bool pollRackThemeChanged() {
        bool changed{false};

        auto current = getActualTheme(theme_setting);
        changed = actual_theme != current;
        actual_theme = current;

        if (changed && notify) { notify->onChangeTheme(ChangedItem::Theme); }
        return changed;
    }

    bool isColorOverride() { return packed_color::isVisible(main_color); }
    void setNotify(IThemeChange *callback) {
        assert((nullptr == callback) || (nullptr == notify));
        notify = callback;
    }
    void setThemeSetting(ThemeSetting setting) override {
        theme_setting = setting;
        actual_theme = getActualTheme(theme_setting);
        if (notify) { notify->onChangeTheme(ChangedItem::Theme); }
    };
    void setScrews(bool showScrews) override {
        screws = showScrews;
        if (notify) { notify->onChangeTheme(ChangedItem::Screws); }
    };
    void setMainColor(PackedColor color) override {
        ITheme::setMainColor(color);
        if (notify) { notify->onChangeTheme(ChangedItem::MainColor); }
    }

    bool hasScrews() override { return screws; }

    virtual json_t* save(json_t* root);
    virtual void load(json_t* root);
};

inline Theme GetPreferredTheme(ITheme * itheme) {
    return  itheme ? itheme->actual_theme : getActualTheme(DefaultThemeSetting);
}
inline PackedColor GetPreferredColor(ITheme * itheme) {
    return  itheme ? itheme->getMainColor() : colors::NoColor;
}

inline ThemeSetting RandomThemeSetting() {
    return static_cast<ThemeSetting>(random::get<uint32_t>() % 5);
}

void RandomizeTheme(ITheme* itheme, bool opaque = true);

}
