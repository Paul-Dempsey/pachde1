#pragma once
#include <rack.hpp>
using namespace ::rack;
#include <stdint.h>
#include "widgets/action-button.hpp"
#include "widgets/components.hpp"
#include "widgets/color-picker.hpp"
#include "widgets/hamburger.hpp"
#include "widgets/switch.hpp"
#include "services/json-help.hpp"
#include "services/rack-help.hpp"
#include "services/theme-module.hpp"
#include "panel-guides.hpp"
#include "guide-data.hpp"
#include "guide-list.hpp"

using namespace svg_query;
using namespace widgetry;

namespace pachde {
struct GuideUi;

struct Guide: ThemeModule {
    using Base = ThemeModule;

    enum Params {
        P_X,
        P_Y,
        P_ANGLE,
        P_W,
        P_R,
        P_OVERLAY_POSITION,
        NUM_PARAMS
    };
    enum Inputs { NUM_INPUTS };
    enum Outputs { NUM_OUTPUTS };
    enum Lights {
        L_CONNECTED,
        NUM_LIGHTS
    };

    GuideData guide_data;
    std::string guide_folder;
    std::string guide_file;
    GuideUi* ui{nullptr};

    Guide();

    json_t* dataToJson() override;
    void dataFromJson(json_t *root) override;

    void set_guide(const std::shared_ptr<GuideLine> guide);
    void onExpanderChange(const ExpanderChangeEvent& e) override;
    void process(const ProcessArgs& args) override;
};

// ---- UI ------------------------------------------------

struct GuideUi  : ModuleWidget, IThemeChange
{
    using Base = ModuleWidget;

    Guide* my_module{nullptr};
    ThemeBase* theme_holder{nullptr};
    SvgCache my_svgs;
#ifdef HOT_SVG
    PositionIndex pos_widgets;
#define HOT_POSITION(name, kind, widget) addPosition(pos_widgets, name, kind, widget)
#else
#define HOT_POSITION(name, kind, widget)
#endif

    std::shared_ptr<GuideLine> guideline{nullptr};
    GuideData* guide_data{nullptr};

    Swatch* panel_swatch{nullptr};
    SolidSwatch* panel_solid{nullptr};
    Swatch* guide_swatch{nullptr};
    SolidSwatch* guide_solid{nullptr};
    TextInput* name_input{nullptr};
    GuideList* guide_list{nullptr};
    widgetry::Switch* pos_switch{nullptr};
    widgetry::Switch* angle_switch{nullptr};
    PanelGuides* panel_guides{nullptr};

    GuideUi(Guide* module);
    virtual ~GuideUi();

    void onExpanderChange(Module::Expander& expander);
    void onDestroyGuide() { panel_guides = nullptr; }

    void set_overlay_position(OverlayPosition pos);
    void set_panel_overlay_color(PackedColor panel);
    void set_guide_color(std::shared_ptr<GuideLine> guide, PackedColor co_guide);
    void add_guide(std::shared_ptr<GuideLine> guide);
    void remove_guide(std::shared_ptr<GuideLine> guide);
    void set_guide(std::shared_ptr<GuideLine> guide);
    void load_guide_file(std::string path);
    void save_guides();
    void open_guides();
    void onChangeTheme(ChangedItem item) override;
#ifdef HOT_SVG
    void onHoverKey(const HoverKeyEvent& e) override;
#endif
    void appendContextMenu(Menu* menu) override ;
    void step() override;
};

}
