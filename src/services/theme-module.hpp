#pragma once
#include <rack.hpp>
#include "theme.hpp"
using namespace ::rack;

namespace pachde {

struct ThemeModule : Module, ThemeBase
{
    void onReset(const ResetEvent& e) override { ThemeBase::reset(); }
    json_t* dataToJson() override { return save(json_object()); }
    void dataFromJson(json_t* root) override { load(root); }
};

inline Theme ModuleTheme(ThemeModule* module) {
    return GetPreferredTheme(module);
}

inline ThemeSetting ModuleThemeSetting(ThemeModule* module) {
    return module ? module->getThemeSetting() : DefaultThemeSetting;
}
inline PackedColor ModuleColor(ThemeModule* module) {
    return module ? module->getMainColor() : colors::NoColor;
}
inline bool ModuleColorOverride(ThemeModule* module) {
    return module && module->isColorOverride();
}
inline bool ModuleHasScrews(ThemeModule* module) {
    return module ? module->hasScrews() : true;
}

struct ThemePanel : Widget
{
    ITheme* theme_holder;
    explicit ThemePanel(ITheme* it) : theme_holder(it) {}

    Theme getTheme() { return GetPreferredTheme(theme_holder); }
    PackedColor getColor() { return theme_holder->getMainColor(); }
    void draw(const DrawArgs &args) override;
};

}