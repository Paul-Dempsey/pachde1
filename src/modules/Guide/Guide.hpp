#pragma once
#include <rack.hpp>
using namespace ::rack;
#include <stdint.h>
#include "widgets/action-button.hpp"
#include "widgets/components.hpp"
#include "widgets/color-picker.hpp"
#include "widgets/hamburger.hpp"
#include "services/json-help.hpp"
#include "services/rack-help.hpp"
#include "services/theme-module.hpp"
#include "panel-overlay-widget.hpp"

using namespace svg_query;
using namespace widgetry;

namespace pachde {
enum OverlayPosition { OnPanel, OnTop };

struct GuideLine {
    Vec origin{0.f};
    float angle{0.f};
    float width{7.5};
    PackedColor color{0x50e04bcf};
    float repeat{0.f};
    std::string name;
    //json_t * toJson();
    //void fromJson(json_t * root);
};

struct GuideData {
    OverlayPosition position{OverlayPosition::OnPanel};
    PackedColor co_overlay{packHsla(0.f, 0.f, 1.f, .35f)};
    std::vector<std::shared_ptr<GuideLine>> guides;
    //json_t * toJson();
    //void fromJson(json_t * root);
};

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
    enum Lights { NUM_LIGHTS };

    GuideData guide_data;

    Guide();

    json_t* dataToJson() override;
    void dataFromJson(json_t *root) override;

    void set_guide(const std::shared_ptr<GuideLine> guide);
    GuideUi* ui{nullptr};
    void onExpanderChange(const ExpanderChangeEvent& e) override;
    void process(const ProcessArgs& args) override;
};

struct GuideUi  : ModuleWidget, IThemeChange
{
    using Base = ModuleWidget;

    Guide* my_module{nullptr};
    ThemeBase* theme_holder{nullptr};
    SvgCache my_svgs;
#ifdef HOT_SVG
    std::map<const char *, Widget*> positioned_widgets;
#define HOT_POSITION(name,widget) positioned_widgets[name] = widget
#else
#define HOT_POSITION(name,widget)
#endif

    std::shared_ptr<GuideLine> guideline{nullptr};
    GuideData* guide_data{nullptr};

    Swatch* panel_swatch{nullptr};
    Swatch* guide_swatch{nullptr};
    TextInput* name_input{nullptr};
    Rect list_box;

    PanelGuide* panel_guide{nullptr};

    GuideUi(Guide* module);
    virtual ~GuideUi();

    void onExpanderChange(Module::Expander& expander);
    void onDestroyGuide() { panel_guide = nullptr; }

    void set_overlay_position(OverlayPosition pos);
    void set_panel_overlay_color(PackedColor panel);
    void set_guide_color(std::shared_ptr<GuideLine> guide, PackedColor co_guide);
    void add_guide(std::shared_ptr<GuideLine> guide);
    void remove_guide(std::shared_ptr<GuideLine> guide);

    void onChangeTheme(ChangedItem item) override;
    void onHoverKey(const HoverKeyEvent& e) override;
    void appendContextMenu(Menu* menu) override ;
    void step() override;
    void draw(const DrawArgs& args) override;
};

}
