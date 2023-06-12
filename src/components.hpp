#pragma once
#include "plugin.hpp"
#include "colors.hpp"
#include "theme.hpp"
#include "themehelpers.hpp"

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

struct ScrewCap : rack::TransparentWidget, ThemeBase
{
    NVGcolor color = COLOR_NONE;

    ScrewCap(Theme theme) {
        box.size.x = box.size.y = 15.f;
        setTheme(theme);
    }

    void draw(const DrawArgs &args) override {
        rack::TransparentWidget::draw(args);
        DrawScrewCap(args.vg, 0, 0, getTheme(), color);
    }
};

struct LogoWidget : rack::OpaqueWidget, ThemeLite {
    LogoWidget(Theme theme) {
        setTheme(theme);
        box.size.x = box.size.y = 15.0f;
    }
    void draw(const DrawArgs &args) override {
        rack::OpaqueWidget::draw(args);
        DrawLogo(args.vg, 0, 0, LogoColor(theme));
    }
};

struct LogoOverlayWidget : rack::OpaqueWidget, ThemeLite {
    LogoOverlayWidget(Theme theme) {
        setTheme(theme);
        box.size.x = box.size.y = 15.0f;
    }
    void draw(const DrawArgs &args) override {
        rack::OpaqueWidget::draw(args);
        auto color = LogoColor(theme);
        if (theme != Theme::HighContrast) {
            color.a = 0.75f;
        }
        DrawLogo(args.vg, 0, 0, color);
    }
};

struct InfoWidget : rack::SvgWidget, ThemeLite {
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

struct BluePort: rack::SvgPort, ThemeLite {
    BluePort(Theme theme) {
        setTheme(theme);
    }

    void setTheme(Theme theme) override {
        if (theme == getTheme() && sw) return;
        ThemeLite::setTheme(theme);
        setSvg(Svg::load(asset::plugin(pluginInstance,
            IsLighter(theme)
                ? "res/Port.svg"
                : "res/PortDark.svg")));
        if (fb) {
            fb->setDirty(true);
        }
    }
};

struct SmallKnob: rack::RoundKnob, ThemeLite {
    SmallKnob(Theme theme) {
        setTheme(theme);
    }

    void setTheme(Theme theme) override {
        if (theme == getTheme() && bg && bg->svg) return;
        ThemeLite::setTheme(theme);
        bool light = IsLighter(theme);
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

struct PicButton: OpaqueWidget, ThemeLite {
    NVGcolor line, face;
    bool pressed = false;
    std::function<void(void)> clickHandler;

    PicButton(Theme theme);

    void setTheme(Theme theme) override;
    void draw(const DrawArgs &args) override;
    void onButton(const event::Button& e) override;
    void onDragEnd(const DragEndEvent & e) override;

    void center(Vec pos) {
        box.pos = pos.minus(box.size.div(2));
    }
    void onClick(std::function<void(void)> callback) {
        clickHandler = callback;
    }
};

struct PLayPauseButton: PushButtonBase, ThemeLite {

    PLayPauseButton(Theme theme) {
        noShadow();
        setTheme(theme);
    }

    void setTheme(Theme theme) override {
        if (theme != getTheme() || frames.empty()) {
            ThemeLite::setTheme(theme);
            bool light = IsLighter(theme);
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

struct Switch : rack::Switch, ThemeLite {
    int value = 0;
    int units = 2;
    NVGcolor background, frame, thumb, thumb_top, thumb_bottom;

    Switch(Theme theme);

	void initParamQuantity() override;
    void draw(const DrawArgs &args) override;
    void onChange(const ChangeEvent& e) override;
    void setTheme(Theme theme) override;
};

void SetChildrenTheme(Widget * widget, Theme theme, bool top = true);
void SetChildrenThemeColor(Widget * widget, NVGcolor color, bool top = true);
void AddThemeMenu(rack::ui::Menu* menu, ITheme* change, bool isChangeColor = false, bool isChangeScrews = false);

struct ThemeModule : Module, ThemeBase
{
    ThemeModule() {
        setScrews(true);
    }
    json_t* dataToJson() override { return save(json_object()); }
    void dataFromJson(json_t* root) override { load(root); }
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
    ITheme* theme_holder;
    ThemePanel(ITheme* it) : theme_holder(it) {}

    Theme getTheme() { return theme_holder->getTheme(); }
    NVGcolor getColor() { return theme_holder->getPanelColor(); }
    void draw(const DrawArgs &args) override;
};

// textfield as menu item, originally adapted from SubmarineFree
struct EventParamField : ui::TextField {
    std::function<void(std::string)> changeHandler;
    std::function<void(std::string)> commitHandler;
    void step() override {
        // Keep selected
        APP->event->setSelectedWidget(this);
        TextField::step();
    }
    void setText(std::string text) {
        this->text = text;
        selectAll();
    }

    void onChange(const ChangeEvent& e) override {
        ui::TextField::onChange(e);
        if (changeHandler) { 
            changeHandler(text);
        }
    }

    void onSelectKey(const event::SelectKey &e) override {
        if (e.action == GLFW_PRESS && (e.key == GLFW_KEY_ENTER || e.key == GLFW_KEY_KP_ENTER)) {
            if (commitHandler) {
                commitHandler(text);
            }
            ui::MenuOverlay *overlay = getAncestorOfType<ui::MenuOverlay>();
            overlay->requestDelete();
            e.consume(this);
        }
        if (!e.getTarget())
            TextField::onSelectKey(e);
    }
};

void AddScrewCaps(Widget *widget, Theme theme, NVGcolor color);
void RemoveScrewCaps(Widget* widget);
void SetScrewColors(Widget* widget, NVGcolor color);

} // namespace pachde