#pragma once
#include "myplugin.hpp"
#include "services/colors.hpp"
#include "services/svg-theme-2.hpp"
#include "services/text.hpp"
#include "create-theme-widget.hpp"
using namespace ::rack;
using namespace ::svg_theme_2;

namespace pachde {

constexpr const float ONE_HP = 15.0f;
constexpr const float TWO_HP = 30.0f;
constexpr const float HALF_HP = 7.5f;

inline float HpToPix(float hp) { return hp * 15.0; }
inline float PixToHp(float pix) { return pix / 15.0; }
inline float ClampBipolar(float v) { return rack::math::clamp(v, -5.0f, 5.0f); }
inline float ClampUnipolar(float v) { return rack::math::clamp(v, 0.0f, 10.0f); }

void AddThemeMenu(rack::ui::Menu *menu, ThemeBase* it, bool isChangeColor, bool isChangeScrews, bool submenu = true);

struct FancyLabel : MenuLabel
{
    NVGcolor co_bg;
    NVGcolor co_text;
    NVGcolor co_rule{nvgHSL(200.f/360.f,.5,.4)};
    NVGalign align{NVG_ALIGN_CENTER};

    FancyLabel();

    void backgroundColor(PackedColor color) { co_bg = fromPacked(color); }
    void textColor(PackedColor color) { co_text = fromPacked(color); }
    void setTextAlignment(NVGalign a) { align = (NVGalign)(a & (NVG_ALIGN_LEFT|NVG_ALIGN_CENTER|NVG_ALIGN_RIGHT)); }
    void draw(const DrawArgs& args) override;
};

template<typename TSvg>
struct TKnob: rack::RoundKnob
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

    void loadSvg(ILoadSvg* loader) {
        setSvg(loader->loadSvg(TSvg::knob()));
        bg->setSvg(loader->loadSvg(TSvg::background()));
    }

    void applyTheme(std::shared_ptr<SvgTheme> theme) {
        applySvgTheme(sw->svg, theme);
        applySvgTheme(bg->svg, theme);
        if (fb) {
            fb->setDirty(true);
        }
    }
};

struct LargeKnobSvg {
    static std::string knob() { return asset::plugin(pluginInstance, "res/widget/LargeKnob.svg"); }
    static std::string background() { return asset::plugin(pluginInstance, "res/widget/LargeKnob-bg.svg"); }
};
using LargeKnob = TKnob<LargeKnobSvg>;

struct SmallKnobSvg {
    static std::string knob() { return asset::plugin(pluginInstance,"res/widget/SmallKnob.svg"); }
    static std::string background() { return asset::plugin(pluginInstance,"res/widget/SmallKnob-bg.svg"); }
};
using SmallKnob = TKnob<SmallKnobSvg>;

// template<typename TSvg>
// void reloadThemeKnob(std::shared_ptr<SvgTheme> theme) {
//     std::string path = TSvg::background();
//     auto bg = window::Svg::load(path);
//     bg->loadFile(path);
//     applySvgTheme(theme, bg);

//     path = TSvg::knob();
//     auto knob = window::Svg::load(path);
//     knob->loadFile(path);
//     applySvgTheme(theme, knob);
// }

template <class TParamWidget>
TParamWidget *createThemeSvgParam(ILoadSvg* loader, std::shared_ptr<SvgTheme> theme, math::Vec pos, engine::Module *module, int paramId)
{
    TParamWidget *o = new TParamWidget();
    o->loadSvg(loader);
    o->applyTheme(theme);
    o->box.pos = pos;
    o->app::ParamWidget::module = module;
    o->app::ParamWidget::paramId = paramId;
    o->initParamQuantity();
    return o;
}

template <class TParamWidget>
TParamWidget *createThemeSvgParam(SvgCache* loader, math::Vec pos, engine::Module *module, int paramId)
{
    TParamWidget *o = new TParamWidget();
    o->loadSvg(loader);
    o->box.pos = pos;
    o->app::ParamWidget::module = module;
    o->app::ParamWidget::paramId = paramId;
    o->initParamQuantity();
    return o;
}

// TSvg is any class/struct with a static
// `static std::string background()` method.
// The method should return the full path of the SVG.
template <typename TSvg>
struct SvgThemePanel : SvgPanel
{
    void loadSvg(ILoadSvg* loader) {
        setBackground(loader->loadSvg(TSvg::background()));
    }

    void applyTheme(std::shared_ptr<SvgTheme> theme) {
        applySvgTheme(svg, theme);
    }

    void updatePanel(std::shared_ptr<SvgTheme> theme) {
        svg->loadFile(TSvg::background());
        applySvgTheme(svg, theme);
        fb->dirty = true;
    }
};

template <class TSvg>
SvgThemePanel<TSvg>* createSvgThemePanel(ILoadSvg* loader, std::shared_ptr<SvgTheme> theme) {
	auto panel = new SvgThemePanel<TSvg>();
    panel->loadSvg(loader);
    if (theme) panel->applyTheme(theme);
    return panel;
}

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

} // namespace pachde