#pragma once
#include "plugin.hpp"
#include "colors.hpp"
using namespace ::rack;

namespace pachde {
constexpr const float ONE_HP = 15.0f;
constexpr const float TWO_HP = 30.0f;
constexpr const float HALF_HP = 7.5f;

void DrawLogo(NVGcontext * vg, float x, float y, NVGcolor fill, float scale = 1.0f);
void DrawScrewCap(NVGcontext * vg, float x, float y, Theme theme, NVGcolor color = COLOR_NONE);

inline float HpToPix(float hp) { return hp * 15.0; }
inline float PixToHp(float pix) { return pix / 15.0; }
inline float ClampBipolar(float v) { return rack::math::clamp(v, -5.0f, 5.0f); }
inline float ClampUnipolar(float v) { return rack::math::clamp(v, 0.0f, 10.0f); }

struct IChangeTheme
{
    virtual void setTheme(Theme theme) {};
    virtual void setColor(NVGcolor color) {};
    virtual void setScrews(bool showScrews) {};
};

struct ScrewCap : rack::TransparentWidget, public IChangeTheme {
    Theme theme = Theme::Unset;
    NVGcolor color = COLOR_NONE;

    ScrewCap(Theme theme) {
        setTheme(theme);
    }
    void draw(const DrawArgs &args) override {
        rack::TransparentWidget::draw(args);
        DrawScrewCap(args.vg, 0, 0, theme, color);
    }

    void setTheme(Theme t) override {
        theme = t;
    }
    void setColor(NVGcolor color) override {
        this->color = color;
    }
};

struct LogoWidget : rack::OpaqueWidget, public IChangeTheme {
    Theme theme = Theme::Unset;

    LogoWidget(Theme theme) {
        setTheme(theme);
        box.size.x = 15.0f;
        box.size.y = 15.0f;
    }
    void draw(const DrawArgs &args) override {
        rack::OpaqueWidget::draw(args);
        DrawLogo(args.vg, 0, 0, LogoColor(theme));
    }
    void setTheme(Theme t) override {
        theme = t;
    }
};

struct LogoOverlayWidget : rack::OpaqueWidget, public IChangeTheme {
    Theme theme = Theme::Unset;

    LogoOverlayWidget(Theme theme) {
        setTheme(theme);
        box.size.x = 15.0f;
        box.size.y = 15.0f;
    }
    void draw(const DrawArgs &args) override {
        rack::OpaqueWidget::draw(args);
        auto color = LogoColor(theme);
        if (theme != Theme::HighContrast) {
            color.a = 0.75f;
        }
        DrawLogo(args.vg, 0, 0, color);
    }

    void setTheme(Theme t) override {
        theme = t;
    }
};

struct InfoWidget : rack::SvgWidget, IChangeTheme {
    Theme theme = Theme::Unset;

    InfoWidget(Theme theme) {
        setTheme(theme);
    }

    void setTheme(Theme t) override {
        if (t == theme && svg) return;
        theme = t;
        setSvg(Svg::load(asset::plugin(pluginInstance, IsLighter(t)
                ? "res/InfoBright.svg"
                : "res/InfoDark.svg")));
    }
};

struct BluePort: rack::SvgPort, public IChangeTheme {
    Theme theme = Theme::Unset;

    BluePort(Theme t) {
        setTheme(t);
    }

    void setTheme(Theme t) override {
        if (t == theme && sw) return;
        theme = t;
        setSvg(Svg::load(asset::plugin(pluginInstance,
            IsLighter(t)
                ? "res/Port.svg"
                : "res/PortDark.svg")));
        if (fb) {
            fb->setDirty(true);
        }
    }
};

struct SmallKnob: rack::RoundKnob, public IChangeTheme {
    Theme theme = Theme::Unset;

    SmallKnob(Theme t) {
        setTheme(t);
    }

    void setTheme(Theme t) override {
        if (t == theme && bg && bg->svg) return;
        theme = t;
        bool light = IsLighter(t);
        setSvg(Svg::load(asset::plugin(pluginInstance,
            light ? "res/SmallKnob.svg" : "res/SmallKnobDark.svg")));
        bg->setSvg(Svg::load(asset::plugin(pluginInstance,
            light ? "res/SmallKnob-bg.svg" : "res/SmallKnobDark-bg.svg")));
        if (fb) {
            fb->setDirty(true);
        }
    }
};

struct PushButtonBase: rack::SvgSwitch {
    CircularShadow* orphan_shadow = nullptr;

    void noShadow() {
        if (orphan_shadow) return;
        orphan_shadow = this->shadow;
        if (orphan_shadow) {
            this->fb->removeChild(orphan_shadow);
        }
    }

    ~PushButtonBase() {
         if (orphan_shadow) {
            delete orphan_shadow;
        }
    }

    void center(Vec pos)
    {
        this->box.pos = pos.minus(this->box.size.div(2));
    }

    std::function<void(void)> clickHandler;
    // set click handler
    // btn->onClick([this, module]() {
    //     this->doSomething(module);
    // });
    void onClick(std::function<void(void)> callback)
    {
        clickHandler = callback;
    }

    void onDragEnd(const DragEndEvent & e) override {
        rack::SvgSwitch::onDragEnd(e);
        if (e.button != GLFW_MOUSE_BUTTON_LEFT)
            return;
        if (clickHandler) {
            clickHandler();
        }
    }
};

// struct PushButton: PushButtonBase, IChangeTheme {
//     Theme theme = Theme::Unset;

//     PushButton(Theme t) {
//         noShadow();
//         setTheme(t);
//     }

//     void setTheme(Theme t) override {
//         if (t != theme || frames.empty()) {
//             theme = t;
//             frames.clear();
//             if (IsLighter(t)) {
//                 addFrame(Svg::load(asset::plugin(pluginInstance,"res/SmallPushButton_Up.svg")));
//                 addFrame(Svg::load(asset::plugin(pluginInstance,"res/SmallPushButton_Down.svg")));
//             } else {
//                 addFrame(Svg::load(asset::plugin(pluginInstance,"res/SmallPushButtonDark_Up.svg")));
//                 addFrame(Svg::load(asset::plugin(pluginInstance,"res/SmallPushButtonDark_Down.svg")));
//             }
//             if (fb) {
//                 fb->setDirty(true);
//             }
//         }
//     }
// };


struct PicButton: OpaqueWidget, IChangeTheme {
    NVGcolor line, face;
    bool pressed = false;
    std::function<void(void)> clickHandler;

    PicButton(Theme t);

    void setTheme(Theme t) override;
    void draw(const DrawArgs &args) override;
    void onButton(const event::Button& e) override;
    void onDragEnd(const DragEndEvent & e) override;

    void center(Vec pos);
    void onClick(std::function<void(void)> callback);
};

struct PLayPauseButton: PushButtonBase, IChangeTheme {
    Theme theme = Theme::Unset;

    PLayPauseButton(Theme t) {
        noShadow();
        setTheme(t);
    }

    void setTheme(Theme t) override {
        if (t != theme || frames.empty()) {
            theme = t;
            bool light = IsLighter(t);
            frames.clear();
            addFrame(Svg::load(asset::plugin(pluginInstance, light
                ? "res/PLayPauseButton_Up.svg"
                : "res/PLayPauseButtonDark_Up.svg")));
            addFrame(Svg::load(asset::plugin(pluginInstance, light
                ? "res/PLayPauseButton_Down.svg"
                : "res/PLayPauseButtonDark_Down.svg")));
            if (fb) {
                fb->setDirty(true);
            }
        }
    }
};

struct Switch : rack::Switch, IChangeTheme {
    Theme theme = Theme::Unset;
    int value = 0;
    int units = 2;
    NVGcolor background, frame, thumb, thumb_top, thumb_bottom;

    Switch(Theme theme);

	void initParamQuantity() override;
    void draw(const DrawArgs &args) override;
    void onChange(const ChangeEvent& e) override;
    void setTheme(Theme t) override;
};

void SetChildrenTheme(Widget * widget, Theme theme, bool top = true);
void SetChildrenThemeColor(Widget * widget, NVGcolor color, bool top = true);

struct ThemeModule : Module, IChangeTheme
{
    bool dirty = false;
    bool show_screws = true;
    Theme theme = Theme::Unset;
    // a non-transparent panel color overrides the theme
    NVGcolor panel_color = COLOR_NONE;

    Theme getTheme() { return ConcreteTheme(theme); }
    NVGcolor getColor() { return panel_color; }
    bool isColorOverride() { return isColorVisible(panel_color); }
    void setTheme(Theme t) override
    {
        theme = t;
        dirty = true;
    }
    void setColor(NVGcolor color) override
    {
        panel_color = color;
        dirty = true;
    }
    void setScrews(bool showScrews) override {
        show_screws = showScrews;
    }
    bool hasScrews() { return show_screws; }

    void onReset() override;
    json_t *dataToJson() override;
    void dataFromJson(json_t *rootJ) override;
    void addThemeMenu(Menu *menu, IChangeTheme *change, bool isChangeColor = false, bool isChangeScrews = false);
};


inline Theme ModuleTheme(ThemeModule* module)
{
    return module ? module->getTheme() : DefaultTheme;
}

inline NVGcolor ModuleColor(ThemeModule* module)
{
    return module ? module->panel_color : COLOR_NONE;
}
inline bool ModuleColorOverride(ThemeModule* module)
{
    return module && module->isColorOverride();
}
inline bool ModuleHasScrews(ThemeModule* module)
{
    return module ? module->hasScrews() : true;
}

struct ThemePanel : Widget
{
    ThemeModule* module;
    ThemePanel(ThemeModule* module)
    {
        this->module = module;
    }
    Theme getTheme() { return ModuleTheme(module); }
    NVGcolor getColor() { return ModuleColor(module); }
    void setTheme(Theme theme) {
        if (module) {
            module->setTheme(theme);
        }
    }
    void setColor(NVGcolor color) {
        if (module) {
            module->setColor(color);
        }
    }
    void draw(const DrawArgs &args) override;
};

void AddScrewCaps(Widget *widget, Theme theme, NVGcolor color);
void RemoveScrewCaps(Widget* widget);
void SetScrewColors(Widget* widget, NVGcolor color);

struct ControlRateTrigger
{
    float rate_ms;
    int steps;
    int trigger = -1;

    ControlRateTrigger(float rate = 2.5f)
    {
        configure(rate);
        assert(trigger >= 1);
        reset();
    }

    void configure(float rate) {
        assert(rate >= 0.0);
        rate_ms = rate;
        onSampleRateChanged();
    }

    // after reset, fires on next step
    void reset() { steps = trigger; }

    void onSampleRateChanged()
    {
        trigger = APP->engine->getSampleRate() * (rate_ms / 1000.0f);
    }

    bool process()
    {
        // rate of 0 means sample rate
        if (rate_ms <= 0.0) return true;

        ++steps;
        if (steps >= trigger)
        {
            steps = 0;
            return true;
        }
        return false;
    }
};


} // namespace pachde