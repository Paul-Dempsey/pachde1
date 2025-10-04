#pragma once
#include "plugin.hpp"
#include "colors.hpp"
#include "theme.hpp"
#include "create-theme-widget.hpp"

using namespace ::rack;

namespace pachde {

constexpr const float ONE_HP = 15.0f;
constexpr const float TWO_HP = 30.0f;
constexpr const float HALF_HP = 7.5f;

inline float HpToPix(float hp) { return hp * 15.0; }
inline float PixToHp(float pix) { return pix / 15.0; }
inline float ClampBipolar(float v) { return rack::math::clamp(v, -5.0f, 5.0f); }
inline float ClampUnipolar(float v) { return rack::math::clamp(v, 0.0f, 10.0f); }

template<typename T>
struct TKnob: rack::RoundKnob, IBasicTheme
{
    bool clickStepValue = true;
    float stepIncrementBy = 1.f;
    bool key_modified = false;
    bool key_modified2 = false;

    std::function<void(void)> clickHandler;
    void onClick(std::function<void(void)> callback)
    {
        clickHandler = callback;
    }

    void onHoverKey(const HoverKeyEvent& e) override {
        rack::RoundKnob::onHoverKey(e);
        key_modified = (e.mods & RACK_MOD_MASK) & RACK_MOD_CTRL;
        key_modified2 = (e.mods & RACK_MOD_MASK) & GLFW_MOD_SHIFT;
    }

    void onAction(const ActionEvent& e) override {
        rack::RoundKnob::onAction(e);
        if (clickHandler) {
            clickHandler();
        } else if (clickStepValue) {
            auto pq = getParamQuantity();
            if (pq) {
                float value = pq->getValue();
                if (!key_modified) {
                    auto lim = pq->getMaxValue();
                    if (value == lim) {
                        value = pq->getMinValue();
                    } else {
                        value = value + (key_modified2 ? stepIncrementBy * 10 : stepIncrementBy);
                        if (value > lim) {
                            value = lim;
                        }
                    }
                } else {
                    auto lim = pq->getMinValue();
                    if (value == lim) {
                        value = pq->getMaxValue();
                    } else {
                        value = value - (key_modified2 ? stepIncrementBy * 10 : stepIncrementBy);
                        if (value < pq->getMinValue()) {
                            value = lim;
                        }
                    }
                }
                pq->setValue(value);
            }
        }
        if (fb) {
            fb->setDirty(true);
        }
    }

    void setTheme(Theme theme) override {
        if (theme == getTheme() && bg && bg->svg) return;
        IBasicTheme::setTheme(theme);
        setSvg(Svg::load(asset::plugin(pluginInstance, T::knob(theme))));
        bg->setSvg(Svg::load(asset::plugin(pluginInstance, T::background(theme))));
        if (fb) {
            fb->setDirty(true);
        }
    }
};

struct LargeKnobSvg {
    static std::string knob(Theme theme) {
        switch (theme) {
            default:
            case Theme::Unset:
            case Theme::Light:
                return "res/widget/LargeKnob.svg";
            case Theme::Dark:
                return "res/widget/LargeKnobDark.svg";
            case Theme::HighContrast:
                return "res/widget/LargeKnobHighContrast.svg";
        }

    }
    static std::string background(Theme theme) {
        switch (theme) {
            default:
            case Theme::Unset:
            case Theme::Light:
                return "res/widget/LargeKnob-bg.svg";
            case Theme::Dark:
            case Theme::HighContrast:
                return "res/widget/LargeKnobDark-bg.svg";
        }
    }
};
using LargeKnob = TKnob<LargeKnobSvg>;

struct SmallKnobSvg {
    static std::string knob(Theme theme) {
        switch (theme) {
            default:
            case Theme::Unset:
            case Theme::Light:
                return "res/widget/SmallKnob.svg";
            case Theme::Dark:
                return "res/widget/SmallKnobDark.svg";
            case Theme::HighContrast:
                return "res/widget/SmallKnobHighContrast.svg";
        }
    }
    static std::string background(Theme theme) {
        switch (theme) {
            default:
            case Theme::Unset:
            case Theme::Light:
                return "res/widget/SmallKnob-bg.svg";
            case Theme::Dark:
            case Theme::HighContrast:
                return "res/widget/SmallKnobDark-bg.svg";
        }
    }
};
using SmallKnob = TKnob<SmallKnobSvg>;

// TSvgProvider is any class/struct with a static
// `static std::string background(Theme theme)` method.
// The method should return the full path of the SVG.
//
// For example:
//
// ```cpp
// struct CopperSvg {
//     static std::string background(Theme theme)
//     {
//         const char * asset;
//         switch (theme) {
//         default:
//         case Theme::Unset:
//         case Theme::Light:
//             asset = "res/Copper.svg";
//             break;
//         case Theme::Dark:
//             asset = "res/CopperDark.svg";
//             break;
//         case Theme::HighContrast:
//             asset = "res/CopperHighContrast.svg";
//             break;
//         }
//         return asset::plugin(pluginInstance, asset);
//     }
// };
// ```
template <typename TSvgProvider>
struct SvgThemePanel : SvgPanel, IBasicTheme
{
    void setTheme(Theme theme) override
    {
        SvgPanel::setBackground(window::Svg::load(TSvgProvider::background(theme)));
    }
    void updatePanel(Theme theme) {
        svg->loadFile(TSvgProvider::background(theme));
        fb->dirty = true;
    }
};

template <class TSvgProvider>
inline SvgThemePanel<TSvgProvider>* createSvgThemePanel(Theme theme) {
	auto tp = new SvgThemePanel<TSvgProvider>();
    tp->setTheme(theme);
    return tp;
}

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

void SetChildrenTheme(Widget * widget, Theme theme, bool top = true);
void SetChildrenThemeColor(Widget * widget, NVGcolor color, bool top = true);

void AddThemeMenu(rack::ui::Menu* menu, ITheme* itheme, bool isChangeColor = false, bool isChangeScrews = false);

struct ThemeModule : Module, ThemeBase
{
    void onReset(const ResetEvent& e) override
    {
        ThemeBase::reset();
    }

    json_t* dataToJson() override { return save(json_object()); }
    void dataFromJson(json_t* root) override { load(root); }
};

inline Theme ModuleTheme(ThemeModule* module)
{
    return GetPreferredTheme(module);
}
inline NVGcolor ModuleColor(ThemeModule* module)
{
    return module ? module->getMainColor() : COLOR_NONE;
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
    explicit ThemePanel(ITheme* it) : theme_holder(it) {}

    Theme getTheme() { return GetPreferredTheme(theme_holder); }
    NVGcolor getColor() { return theme_holder->getMainColor(); }
    void draw(const DrawArgs &args) override;
};

// textfield as menu item, originally adapted from SubmarineFree
struct MenuTextField : ui::TextField {
    std::function<void(std::string)> changeHandler;
    std::function<void(std::string)> commitHandler;
    void step() override {
        // Keep selected
        APP->event->setSelectedWidget(this);
        TextField::step();
    }
    void setText(const std::string& text) {
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

struct PickerTextField : ui::TextField
{
    std::function<void(std::string)> changeHandler;
    std::function<void(std::string)> commitHandler;
    void step() override {
        // Keep selected
        APP->event->setSelectedWidget(this);
        TextField::step();
    }
    void setText(const std::string& text) {
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
            e.consume(this);
        }
        if (!e.getTarget())
            TextField::onSelectKey(e);
    }
};

} // namespace pachde