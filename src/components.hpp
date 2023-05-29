#pragma once
#include "plugin.hpp"

inline float HpToPix(float hp) { return hp * 15.0; }
inline float PixToHp(float pix) { return pix / 15.0; }
inline float ClampBipolar(float v) { return clamp(v, -5.0f, 5.0f); }
inline float ClampUnipolar(float v) { return clamp(v, 0.0f, 10.0f); }

struct IChangeTheme
{
    virtual void setTheme(Theme theme) {};
};


struct ScrewCap : app::SvgScrew, IChangeTheme {
    enum Brightness { More, Less };
    Theme theme = Theme::Unset;
    Brightness bright;

    ScrewCap(Theme theme, Brightness lightness) 
        : app::SvgScrew(), bright(lightness) {
        setTheme(theme);
    }

    void setTheme(Theme t) override {
        assert(Theme::Unset != t);
        if (t == theme && sw && sw->svg) return;
        theme = t;
        setSvg(Svg::load(asset::plugin(pluginInstance, 
            IsLighter(t)
                ? (bright == Brightness::More 
                    ? "res/ScrewCap.svg"
                    : "res/ScrewCapMed.svg")
                : "res/ScrewCapDark.svg")));
        if (fb) {
            fb->setDirty(true);
        }
    }
};

struct LogoWidget : rack::SvgWidget, IChangeTheme {
    Theme theme = Theme::Unset;

    LogoWidget(Theme theme)
        : rack::SvgWidget() {
        setTheme(theme);
    }

    void setTheme(Theme t) override {
        assert(Theme::Unset != t);
        if (t == theme && svg) return;
        theme = t;
        setSvg(Svg::load(asset::plugin(pluginInstance, 
            IsLighter(t)
                ? "res/Logo.svg"
                : "res/LogoBright.svg")));
    }
};

struct LogoOverlayWidget : rack::SvgWidget, IChangeTheme {
    Theme theme = Theme::Unset;

    LogoOverlayWidget(Theme theme)
        : rack::SvgWidget() {
        setTheme(theme);
    }

    void setTheme(Theme t) override {
        assert(Theme::Unset != t);
        if (t == theme && svg) return;
        theme = t;
        setSvg(Svg::load(asset::plugin(pluginInstance, 
            IsLighter(t) 
                ? "res/LogoOverlay.svg"
                : "res/LogoBrightOverlay.svg")));
    }
};

struct InfoWidget : rack::SvgWidget, IChangeTheme {
    Theme theme = Theme::Unset;

    InfoWidget(Theme theme)
        : rack::SvgWidget() {
        setTheme(theme);
    }

    void setTheme(Theme t) override {
        assert(Theme::Unset != t);
        if (t == theme && svg) return;
        theme = t;
        setSvg(Svg::load(asset::plugin(pluginInstance, IsLighter(t)
                ? "res/InfoBright.svg"
                : "res/InfoDark.svg")));
    }
};

struct NullWidget : rack::SvgWidget, IChangeTheme
{
    Theme theme = Theme::Unset;

    NullWidget(Theme t)
        : rack::SvgWidget() {
        setTheme(t);
    }

    void setTheme(Theme t) override {
        assert(Theme::Unset != t);
        if (t == theme && svg) return;
        theme = t;
        setSvg(Svg::load(asset::plugin(pluginInstance,
            IsLighter(t)
                ? "res/NullBright.svg"
                : "res/NullDark.svg")));
    }
};

struct BluePort: SvgPort, IChangeTheme {
    Theme theme = Theme::Unset;

    BluePort(Theme t)
        : SvgPort() {
        setTheme(t);
    }

    void setTheme(Theme t) override {
        assert(Theme::Unset != t);
        if (t == theme && sw) return;
        theme = t;
        setSvg(Svg::load(asset::plugin(pluginInstance,
            IsLighter(t)
                ? "res/Port.svg"
                : "res/PortDark.svg")));
    }
};


struct SmallKnob: RoundKnob, IChangeTheme {
    Theme theme = Theme::Unset;

    SmallKnob(Theme t) {
        setTheme(t);
    }

    void setTheme(Theme t) override {
        assert(Theme::Unset != t);
        if (t == theme && bg && bg->svg) return;
        theme = t;
        bool light = IsLighter(t);
        setSvg(Svg::load(asset::plugin(pluginInstance,
            light ? "res/SmallKnob.svg" : "res/SmallKnobDark.svg")));
        bg->setSvg(Svg::load(asset::plugin(pluginInstance,
            light ? "res/SmallKnob-bg.svg" : "res/SmallKnobDark-bg.svg")));
        fb->setDirty(true);
    }
};

void SetThemeChildren(Widget * widget, Theme theme, bool top = true);

struct ThemeModule : Module
{
    bool dirty = false;
    Theme theme = Theme::Unset;

    void setTheme(Theme t)
    {
        theme = t;
        dirty = true;
    }

    // get valid theme
    Theme getTheme() { return ConcreteTheme(theme); }

    void onReset() override;
    json_t *dataToJson() override;
    void dataFromJson(json_t *rootJ) override;
    void addThemeMenu(Menu *menu, IChangeTheme *change);
};


inline Theme ModuleTheme(ThemeModule *module)
{
    return module ? module->getTheme() : DefaultTheme;
}

struct ThemePanel : Widget
{
    Theme theme;

    void draw(const DrawArgs &args) override;
};

void CreateScrews(ModuleWidget *me, Theme theme, ScrewCap::Brightness bright);

struct ControlRateTrigger
{
    float rate_ms;
    int steps;
    int trigger;

    ControlRateTrigger(float rate = 2.5f) : rate_ms(rate)
    {
        assert(rate > 0.0);
        onSampleRateChanged();
        reset();
        assert(trigger >= 1);
    }

    // after reset, fires on next step
    void reset() { steps = trigger; }

    void onSampleRateChanged()
    {
        // modulate every ~2.5ms regardless of engine sample rate.
        trigger = APP->engine->getSampleRate() * (rate_ms / 1000.0f);
    }

    bool process()
    {
        ++steps;
        if (steps >= trigger)
        {
            steps = 0;
            return true;
        }
        return false;
    }
};