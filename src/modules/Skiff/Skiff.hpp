#pragma once
#include <rack.hpp>
using namespace ::rack;
#include "services/theme-module.hpp"
#include "services/svg-query.hpp"
using namespace svg_query;
#include "widgets/action-button.hpp"
#include "widgets/hamburger.hpp"
#include "widgets/text-button.hpp"
using namespace widgetry;
#include "rails.hpp"
#include "cloak.hpp"

namespace pachde {

struct SkiffUi;

struct Skiff : ThemeModule
{
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
        N_INPUTS
    };
    enum Outputs {
        N_OUTPUTS
    };
    enum Lights {
        L_FANCY,
        N_LIGHTS
    };

    std::string rail{"Rack"};
    std::string rail_folder = asset::userDir;
    bool other_skiff{false};
    bool derailed{false};
    bool depaneled{false};
    bool calm{false};
    bool unscrewed{false};
    bool nojacks{false};
    bool dark_ages{false};
    bool fancy{false};
    bool shouting{true};
    CloakData fancy_data;
    std::string theme_name;
    RailThemeSetting rail_theme;

    Skiff();
    SkiffUi* ui{nullptr};

    void set_defaults();
    json_t* dataToJson() override;
    void dataFromJson(json_t* root) override;
    void random_settings();

    void onRandomize(const RandomizeEvent& e) override;
    void process(const ProcessArgs& args) override;
};

struct RailMenu : Hamburger
{
    SkiffUi* ui{nullptr};
    void setUi(SkiffUi* w) { ui = w; }
    void appendContextMenu(ui::Menu* menu) override;
};

struct SkiffUi : ModuleWidget, IThemeChange, ICloakBackgroundClient
{
    using Base = ModuleWidget;

    Skiff* my_module{nullptr};
    ThemeBase* theme_holder{nullptr};
    SvgCache my_svgs;

    bool request_custom_rail{false};

    RailMenu* ham{nullptr};
    TextButton* derail_button{nullptr};
    TextButton* nopanel_button{nullptr};
    TextButton* calm_button{nullptr};
    TextButton* unscrew_button{nullptr};
    TextButton* nojack_button{nullptr};
    TextButton* dark_ages_button{nullptr};
    TextButton* pack_button{nullptr};
    TextButton* fancy_button{nullptr};
    GearActionButton* fancy_options{nullptr};

    CloakBackgroundWidget* my_cloak{nullptr};
    bool request_cloak{false};

    #ifdef HOT_SVG
    PositionIndex pos_widgets;
    #define HOT_POSITION(name,kind,widget) addPosition(pos_widgets, name, kind, widget)
    #else
    #define HOT_POSITION(name,kind,widget)
    #endif
    SkiffUi(Skiff* module);

    TextButton* makeTextButton (
        std::map<std::string,::math::Rect>& bounds,
        const char* key,
        bool sticky,
        const char* title,
        const char* tip,
        std::shared_ptr<svg_theme::SvgTheme> svg_theme,
        std::function<void(bool,bool)> handler);
    void onChangeTheme(ChangedItem item) override;
    void onDeleteCloak(CloakBackgroundWidget* cloak) override;
    void sync_latch_state();
    void restore_rack();
    void from_module();
    void shouting_buttons(bool shouting);
    void no_panels(bool depanel);
    void set_unscrewed(bool unscrewed);
    void derail(bool derail);
    void fancy_background(bool fancy);
    void set_nojacks(bool nojacks);
    void set_dark_ages(bool dark);
    void calm_rack(bool calm);
    void recover_rack_rail();
    std::shared_ptr<window::Svg> set_rail_svg(RailWidget* rail, const std::string& filename);
    void set_alt_rail(const std::string& rail_name);
    void pend_custom_rail();
    void custom_rail();
    void set_rail_theme(RailThemeSetting theme);
    void onHoverKey(const HoverKeyEvent& e) override;
    void step() override;
    void draw(const DrawArgs& args) override;
    void appendContextMenu(Menu* menu) override;

};

}