#pragma once
#include "plugin.hpp"
#include "colors.hpp"
#include "theme.hpp"
#include "theme_helpers.hpp"
#include "play_pause.hpp"

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

enum ScrewAlign : uint8_t {
    TL_INSET = 0x01,
    TR_INSET = 0x02,
    BL_INSET = 0x04,
    BR_INSET = 0x08,
    UNKNOWN_ALIGNMENT = 0x80,
    SCREWS_OUTSIDE      = 0,
    SCREWS_INSET        = TL_INSET|TR_INSET|BL_INSET|BR_INSET,
    TOP_SCREWS_INSET    = TL_INSET|TR_INSET,
    BOTTOM_SCREWS_INSET = BL_INSET|BR_INSET,
};

enum WhichScrew : uint8_t {
    TL = 0x01,
    TR = 0x02,
    BL = 0x04,
    BR = 0x08,
    UNKNOWN_WHICH = 0x80,
    TOP_SCREWS      = TL|TR,
    BOTTOM_SCREWS   = BL|BR,
    UP_SCREWS       = TR|BL,  // [/]
    DOWN_SCREWS     = TL|BR,  // [\]
    ALL_SCREWS      = TL|TR|BL|BR,
    RIGHT_SCREWS    = TR|BR,
    LEFT_SCREWS     = TL|BL,
};

inline bool isUnknown(WhichScrew which) { return which & WhichScrew::UNKNOWN_WHICH; }
inline bool isApplicable(WhichScrew which_screw, WhichScrew mask) { return mask & which_screw; }
inline float tl_screw_inset(ScrewAlign align) { return ONE_HP * static_cast<bool>(align & ScrewAlign::TL_INSET); }
inline float tr_screw_inset(ScrewAlign align) { return ONE_HP * static_cast<bool>(align & ScrewAlign::TR_INSET); }
inline float bl_screw_inset(ScrewAlign align) { return ONE_HP * static_cast<bool>(align & ScrewAlign::BL_INSET); }
inline float br_screw_inset(ScrewAlign align) { return ONE_HP * static_cast<bool>(align & ScrewAlign::BR_INSET); }

void AddScrewCaps(Widget *widget, Theme theme, NVGcolor color, ScrewAlign positions = ScrewAlign::SCREWS_INSET, WhichScrew which = WhichScrew::ALL_SCREWS);
void RemoveScrewCaps(Widget* widget, WhichScrew which = WhichScrew::ALL_SCREWS);
void SetScrewColors(Widget* widget, NVGcolor color, WhichScrew which = WhichScrew::ALL_SCREWS);

struct ScrewCap : rack::TransparentWidget, IBasicTheme
{
    WhichScrew which;
    ScrewAlign align;

    ScrewCap (Theme theme, WhichScrew position, ScrewAlign alignment)
    : which(position), align(alignment)
    {
        setTheme(theme);
        box.size.x = box.size.y = 15.f;
    }

    void draw(const DrawArgs &args) override {
        rack::TransparentWidget::draw(args);
        DrawScrewCap(args.vg, 0, 0, getTheme(), getMainColor());
    }

    void step() override {
        rack::TransparentWidget::step();
        if (align & ScrewAlign::UNKNOWN_ALIGNMENT) return;
        if (parent) {
            switch (which) {
            default: break;
            case WhichScrew::TL:
                box.pos = Vec(tl_screw_inset(align), 0);
                break;
            case WhichScrew::TR:
                box.pos = Vec(parent->box.size.x - ONE_HP - tr_screw_inset(align), 0);
                break;
            case WhichScrew::BL:
                box.pos = Vec(bl_screw_inset(align), RACK_GRID_HEIGHT - ONE_HP);
                break;
            case WhichScrew::BR:
                box.pos = Vec(parent->box.size.x - ONE_HP - br_screw_inset(align), RACK_GRID_HEIGHT - ONE_HP);
                break;
            }
        }
    }
};

WhichScrew GetScrewPosition(const ScrewCap* screw);
WhichScrew SetScrewPosition(ScrewCap* screw, WhichScrew which);

struct LogoWidget : rack::OpaqueWidget, IBasicTheme
{
    LogoWidget() {
        box.size.x = box.size.y = 15.0f;
    }
    void draw(const DrawArgs &args) override {
        rack::OpaqueWidget::draw(args);
        DrawLogo(args.vg, 0, 0, LogoColor(theme));
    }
};

struct LogoOverlayWidget : rack::OpaqueWidget, IBasicTheme
{
    LogoOverlayWidget() {
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
                return "res/LargeKnob.svg";
            case Theme::Dark:
                return "res/LargeKnobDark.svg";
            case Theme::HighContrast:
                return "res/LargeKnobHighContrast.svg";
        }
        
    }
    static std::string background(Theme theme) {
        switch (theme) {
            default:
            case Theme::Unset:
            case Theme::Light:
                return "res/LargeKnob-bg.svg";
            case Theme::Dark:
            case Theme::HighContrast:
                return "res/LargeKnobDark-bg.svg";
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
                return "res/SmallKnob.svg";
            case Theme::Dark:
                return "res/SmallKnobDark.svg";
            case Theme::HighContrast:
                return "res/SmallKnobHighContrast.svg";
        }
    }
    static std::string background(Theme theme) {
        switch (theme) {
            default:
            case Theme::Unset:
            case Theme::Light:
                return "res/SmallKnob-bg.svg";
            case Theme::Dark:
            case Theme::HighContrast:
                return "res/SmallKnobDark-bg.svg";
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

struct Switch : rack::Switch, IBasicTheme {
    int value = 0;
    int units = 2;
    NVGcolor background, frame, thumb, thumb_top, thumb_bottom;

    Switch();

	void initParamQuantity() override;
    void draw(const DrawArgs &args) override;
    void onChange(const ChangeEvent& e) override;
    void setTheme(Theme theme) override;
};

void SetChildrenTheme(Widget * widget, Theme theme, bool top = true);
void SetChildrenThemeColor(Widget * widget, NVGcolor color, bool top = true);

class IDirty {
    bool _dirty = false;
public:
    bool dirty() { return _dirty; }
    void clean() { _dirty = false; }
    void touch() { _dirty = true; }
};
void AddThemeMenu(rack::ui::Menu* menu, ITheme* itheme, bool isChangeColor = false, bool isChangeScrews = false);

inline bool HaveScrewChildren(Widget* widget)
{
    return widget->children.end() !=
        std::find_if(widget->children.begin(), widget->children.end(),
            [](Widget* child) { return nullptr != dynamic_cast<ScrewCap*>(child); } );
}

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

struct PickerTextField : ui::TextField {
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