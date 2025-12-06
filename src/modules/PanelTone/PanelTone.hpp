#pragma once
#include <rack.hpp>
using namespace ::rack;
#include "services/theme-module.hpp"
#include "services/rack-help.hpp"
#include "widgets/action-button.hpp"
#include "widgets/color-picker.hpp"
#include "widgets/hamburger.hpp"
#include "widgets/switch.hpp"
#include "panel-overlay.hpp"
#include "fader.hpp"

using namespace svg_query;
using namespace widgetry;

namespace pachde {

enum class InputKind { None, Trigger, Continuous };
struct PanelTone : ThemeModule
{
    using Base = ThemeModule;
    enum Params {
        P_OVERLAY_POSITION,
        P_CONFIG_INPUT,
        P_FADE_TIME,
        NUM_PARAMS
    };
    enum Inputs {
        IN_FADE,
        NUM_INPUTS
    };
    enum Outputs { NUM_OUTPUTS };
    enum Lights {
        L_ON,
        NUM_LIGHTS
    };
    OverlayData data;
    AppliesTo apply_to{AppliesTo::All};
    bool apply_to_me{false};
    rack::dsp::SchmittTrigger fade_trigger;
    Fader fader;
    float last_input_config{0.f};
    bool coppertone{false};
    bool copper_connected{false};
    NVGcolor copper_color{COLOR_NONE};
    std::vector<int64_t> module_ids;
    PanelToneUi* ui{nullptr};

    PanelTone();
    void fade_in();
    void fade_out();

    InputKind get_input_kind();
    json_t* dataToJson() override;
    void dataFromJson(json_t* root) override;
    void onReset(const ResetEvent& e) override;
    bool fetch_expander_color(Expander expander);
    void process(const ProcessArgs& args) override;
};

struct PanelToneUi : ModuleWidget, IThemeChange
{
    using Base = ModuleWidget;

    PanelTone* my_module{nullptr};
    ThemeBase* theme_holder{nullptr};
    SvgCache my_svgs;
#ifdef HOT_SVG
    PositionIndex pos_widgets;
#define HOT_POSITION(name,kind,widget) addPosition(pos_widgets, name, kind, widget)
#else
#define HOT_POSITION(name, kind, widget)
#endif

    OverlayData* data{nullptr};

    widgetry::Switch* pos_switch{nullptr};
    widgetry::Switch* in_config_switch{nullptr};
    SmallActionButton* on_button{nullptr};

    std::vector<PanelOverlay*> overlays;
    bool in_destroy{false};
    bool remove_pending{false};
    bool add_pending{false};
    bool coppertoning{false};
    PackedColor last_broadcast_color{colors::NoColor};

    PanelToneUi(PanelTone* module);
    virtual ~PanelToneUi();

    void onDestroyPanelOverlay(PanelOverlay* removed);
    void onStartFadeIn();
    void onFadeOutComplete();
    bool is_my_overlay(PanelOverlay* overlay) { return overlay->host == this; }
    void fade_in_overlays();
    void fade_out_overlays();
    void fade_overlays();
    void set_overlay_position(OverlayPosition pos);
    void set_overlay_color(PackedColor color);
    void broadcast_overlay_color(PackedColor color);
    void add_overlays(const std::vector<ModuleWidget*>& module_widgets);
    void gather_orphaned_overlays();
    void remove_overlays();
    std::vector<ModuleWidget*> get_applicable_module_widgets();
    void toggle_panels();
    void toggle_applies_to_me();
    void set_applies_to(AppliesTo apply);
    void onChangeTheme(ChangedItem item) override;

    void onHoverKey(const HoverKeyEvent& e) override;
    void appendContextMenu(Menu* menu) override ;
    void step() override;

};
}