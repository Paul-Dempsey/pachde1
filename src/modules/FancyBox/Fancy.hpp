#pragma once
#include <rack.hpp>
using namespace ::rack;
#include "services/theme-module.hpp"
#include "services/svg-query.hpp"
using namespace svg_query;
#include "widgets/action-button.hpp"
#include "widgets/label.hpp"
#include "widgets/text-button.hpp"
using namespace widgetry;
#include "cloak.hpp"
#include "fancy-swatch.hpp"

namespace pachde {

struct FancyUi;

struct Fancy : ThemeModule {
    using Base = ThemeModule;

    enum Params {
        P_FANCY_FILL_ON,
        P_FANCY_FILL_FADE,

        P_FANCY_LINEAR_ON,
        P_FANCY_LINEAR_START_FADE,
        P_FANCY_LINEAR_X1,
        P_FANCY_LINEAR_Y1,
        P_FANCY_LINEAR_END_FADE,
        P_FANCY_LINEAR_X2,
        P_FANCY_LINEAR_Y2,

        P_FANCY_RADIAL_ON,
        P_FANCY_RADIAL_INNER_FADE,
        P_FANCY_RADIAL_CX,
        P_FANCY_RADIAL_CY,
        P_FANCY_RADIAL_OUTER_FADE,
        P_FANCY_RADIAL_RADIUS,

        P_FANCY_BOX_ON,
        P_FANCY_BOX_INNER_FADE,
        P_FANCY_BOX_SHRINK_X,
        P_FANCY_BOX_SHRINK_Y,
        P_FANCY_BOX_OUTER_FADE,
        P_FANCY_BOX_RADIUS,
        P_FANCY_BOX_FEATHER,

        N_PARAMS
    };
    enum Inputs {
        IN_FANCY_FILL_H,
        IN_FANCY_FILL_S,
        IN_FANCY_FILL_L,
        IN_FANCY_FILL_A,
        IN_FANCY_FILL_FADE,
        IN_FANCY_LINEAR_START_H,
        IN_FANCY_LINEAR_START_S,
        IN_FANCY_LINEAR_START_L,
        IN_FANCY_LINEAR_START_A,
        IN_FANCY_LINEAR_START_FADE,
        IN_FANCY_LINEAR_X1,
        IN_FANCY_LINEAR_Y1,
        IN_FANCY_LINEAR_END_H,
        IN_FANCY_LINEAR_END_S,
        IN_FANCY_LINEAR_END_L,
        IN_FANCY_LINEAR_END_A,
        IN_FANCY_LINEAR_END_FADE,
        IN_FANCY_LINEAR_X2,
        IN_FANCY_LINEAR_Y2,
        IN_FANCY_RADIAL_INNER_H,
        IN_FANCY_RADIAL_INNER_S,
        IN_FANCY_RADIAL_INNER_L,
        IN_FANCY_RADIAL_INNER_A,
        IN_FANCY_RADIAL_INNER_FADE,
        IN_FANCY_RADIAL_CX,
        IN_FANCY_RADIAL_CY,
        IN_FANCY_RADIAL_OUTER_H,
        IN_FANCY_RADIAL_OUTER_S,
        IN_FANCY_RADIAL_OUTER_L,
        IN_FANCY_RADIAL_OUTER_A,
        IN_FANCY_RADIAL_OUTER_FADE,
        IN_FANCY_RADIAL_RADIUS,
        IN_FANCY_BOX_INNER_H,
        IN_FANCY_BOX_INNER_S,
        IN_FANCY_BOX_INNER_L,
        IN_FANCY_BOX_INNER_A,
        IN_FANCY_BOX_INNER_FADE,
        IN_FANCY_BOX_SHRINK_X,
        IN_FANCY_BOX_SHRINK_Y,
        IN_FANCY_BOX_OUTER_H,
        IN_FANCY_BOX_OUTER_S,
        IN_FANCY_BOX_OUTER_L,
        IN_FANCY_BOX_OUTER_A,
        IN_FANCY_BOX_OUTER_FADE,
        IN_FANCY_BOX_RADIUS,
        IN_FANCY_BOX_FEATHER,
        N_INPUTS
    };
    enum Outputs {
        N_OUTPUTS
    };
    enum Lights {
        L_FANCY,
        N_LIGHTS
    };
    bool other_fancy{false};
    bool fancy{false};
    bool show_ports{true};
    bool shouting{true};
    CloakData fancy_data;
    std::string theme_name;

    Fancy();
    FancyUi* ui{nullptr};

    json_t* dataToJson() override;
    void dataFromJson(json_t* root) override;
    void process(const ProcessArgs& args) override;
};

struct FancyUi : ModuleWidget, IThemeChange, ICloakBackgroundClient
{
    using Base = ModuleWidget;

    Fancy* my_module{nullptr};
    ThemeBase* theme_holder{nullptr};
    SvgCache my_svgs;

    LabelStyle* label_style{nullptr};
    LabelStyle* title_style{nullptr};
    LabelStyle* group_style{nullptr};
    FancySwatch* fill_swatch{nullptr};
    FancySwatch* lg_start_swatch{nullptr};
    FancySwatch* lg_end_swatch{nullptr};
    FancySwatch* rg_inner_swatch{nullptr};
    FancySwatch* rg_outer_swatch{nullptr};
    FancySwatch* bg_inner_swatch{nullptr};
    FancySwatch* bg_outer_swatch{nullptr};
    TextButton* fancy_button{nullptr};
    JackButton* jack_button{nullptr};
    std::vector<Widget*> removables;
    CloakBackgroundWidget* my_cloak{nullptr};
    bool request_cloak{false};

    #ifdef HOT_SVG
    PositionIndex pos_widgets;
    #define HOT_POSITION(name,kind,widget) addPosition(pos_widgets, name, kind, widget)
    #else
    #define HOT_POSITION(name,kind,widget)
    #endif

    FancyUi(Fancy* module);

    TextButton* makeTextButton (
        std::map<std::string,::math::Rect>& bounds,
        const char* key,
        bool sticky,
        const char* title,
        const char* tip,
        std::shared_ptr<svg_theme::SvgTheme> svg_theme,
        std::function<void(bool,bool)> handler
    );
    void add_knob(::svg_query::BoundsIndex& bounds, const char* key, int param);
    void add_label(::svg_query::BoundsIndex& bounds, const char* key, const char* text, LabelStyle* style, std::shared_ptr<svg_theme::SvgTheme> svg_theme, bool removable = false);
    void add_check(::svg_query::BoundsIndex& bounds, const char* key, int param, std::shared_ptr<svg_theme::SvgTheme> svg_theme);
    void add_input(::svg_query::BoundsIndex& bounds, const char* key, int id, PackedColor color);
    void add_ports(::svg_query::BoundsIndex& bounds, std::shared_ptr<svg_theme::SvgTheme> svg_theme);
    void remove_ports();
    bool show_ports() { return my_module ? my_module->show_ports : true; }
    void set_fill_color(PackedColor color);
    void set_lg_start_color(PackedColor color);
    void set_lg_end_color(PackedColor color);
    void set_rg_inner_color(PackedColor color);
    void set_rg_outer_color(PackedColor color);
    void set_bg_inner_color(PackedColor color);
    void set_bg_outer_color(PackedColor color);

    void onChangeTheme(ChangedItem item) override;
    void onDeleteCloak(CloakBackgroundWidget* cloak) override;
    void sync_latch_state();
    void shouting_buttons(bool shouting);
    void fancy_background(bool fancy);
    void from_module();

    void onHoverKey(const HoverKeyEvent& e) override;
    void step() override;
    void draw(const DrawArgs& args) override;
    void appendContextMenu(Menu* menu) override;

};












}