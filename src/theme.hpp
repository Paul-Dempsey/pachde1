#pragma once
#ifndef THEME_HPP_INCLUDED
#define THEME_HPP_INCLUDED
#include <rack.hpp>
#include "colors.hpp"

using namespace ::rack;
namespace pachde {

// theme + main color for widgets
struct IBasicTheme
{
    Theme theme = DefaultTheme;
    NVGcolor main_color = COLOR_NONE;

    virtual ~IBasicTheme() {}
    virtual void setTheme(Theme new_theme) { theme = new_theme; }
    virtual Theme getTheme() { return theme; };
    virtual void setMainColor(NVGcolor color) { main_color = color; }
    virtual NVGcolor getMainColor() { return main_color; }
};

// theme + main color + follow rack dark
struct ITheme : IBasicTheme
{
    virtual ~ITheme() {}
    virtual void setDarkTheme(Theme theme) {}
    virtual void setScrews(bool showScrews) {}
    virtual void setFollowRack(bool follow) {}

    virtual bool hasScrews() { return true; }
    virtual Theme getDarkTheme() { return Theme::Dark; }
    virtual bool getFollowRack() { return true; }
};

// a theme -- no screws
// struct ThemeLite: ITheme
// {
//     virtual ~ThemeLite() {}
//     Theme dark_theme = Theme::Dark;
//     bool follow_rack = true;

//     Theme getDarkTheme() override { return dark_theme; }
//     bool getFollowRack() override { return follow_rack; }
//     bool hasScrews() override { return false; }

//     void setDarkTheme(Theme theme) override { this->dark_theme = theme; };
//     void setFollowRack(bool follow) override { follow_rack = follow; }
// };

enum class ChangedItem : uint8_t {
    Theme,
    DarkTheme,
    FollowDark,
    MainColor,
    Screws
};

struct IThemeChange
{
    virtual void onChangeTheme(ChangedItem item) = 0;
};

struct ThemeBase: ITheme
{
    Theme dark_theme = Theme::Dark;
    bool screws = true;
    bool follow_rack = true;
    bool rack_dark = ::rack::settings::preferDarkPanels;
    IThemeChange * notify = nullptr;

    virtual ~ThemeBase() {}

    void reset();
    //void randomize();

    // Rack sends no notifcation for changes in this setting, so we must poll.
    // `pollRackDarkChanged` checks for changes in rack preferDarkPanels.
    // Returns true and notifies with ChangedItem::FollowDark if changed.
    bool pollRackDarkChanged() {
        bool new_rack_dark = ::rack::settings::preferDarkPanels;
        bool changed = rack_dark != new_rack_dark;
        rack_dark = new_rack_dark;
        if (changed && notify) { notify->onChangeTheme(ChangedItem::FollowDark); }
        return changed;
    }

    bool isColorOverride() { return isColorVisible(main_color); }
    void setNotify(IThemeChange *callback) {
        assert(nullptr == callback || nullptr == notify);
        notify = callback;
    }
    void setTheme(Theme theme) override {
        this->theme = theme;
        if (notify) { notify->onChangeTheme(ChangedItem::Theme); }
    };
    void setDarkTheme(Theme theme) override {
        dark_theme = theme;
        if (notify) { notify->onChangeTheme(ChangedItem::DarkTheme); }
    }
    void setScrews(bool showScrews) override {
        screws = showScrews;
        if (notify) { notify->onChangeTheme(ChangedItem::Screws); }
    };
    void setFollowRack(bool follow) override {
        follow_rack = follow;
        if (notify) { notify->onChangeTheme(ChangedItem::FollowDark); }
    }
    void setMainColor(NVGcolor color) override {
        ITheme::setMainColor(color);
        if (notify) { notify->onChangeTheme(ChangedItem::MainColor); }
    }

    Theme getTheme() override { return ConcreteTheme(theme); };
    bool hasScrews() override { return screws; }
    bool getFollowRack() override { return follow_rack; }
    Theme getDarkTheme() override { return dark_theme; }

    virtual json_t* save(json_t* root);
    virtual void load(json_t* root);
};

inline Theme GetPreferredTheme(ITheme * itheme) {
    if (!itheme) return ::rack::settings::preferDarkPanels ? Theme::Dark : DefaultTheme;
    return (itheme->getFollowRack() && ::rack::settings::preferDarkPanels)
        ? itheme->getDarkTheme()
        : itheme->getTheme();
}
void DarkToJson(ITheme * itheme, json_t* root);
void DarkFromJson(ITheme * itheme, json_t* root);

inline Theme RandomTheme() {
    //return static_cast<Theme>(clamp(std::floor(random::uniform() * 3), 1.f, 3.f));
    return static_cast<Theme>(1 + random::get<uint32_t>() % 3);
}

void RandomizeTheme(ITheme* itheme, bool opaque = true);

}
#endif