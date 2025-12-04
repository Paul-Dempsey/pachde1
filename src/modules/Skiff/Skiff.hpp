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

namespace pachde {

struct SkiffUi;

struct Skiff : ThemeModule
{
    using Base = ThemeModule;

    std::string rail{"Rack"};
    std::string rail_folder = asset::userDir;
    bool other_skiff{false};
    bool derailed{false};
    bool depaneled{false};
    bool calm{false};
    bool unscrewed{false};
    bool nojacks{false};
    bool dark_ages{false};
    bool shouting{true};
    std::string theme_name;
    RailThemeSetting rail_theme;

    Skiff();
    SkiffUi* ui{nullptr};

    void set_defaults();
    json_t* dataToJson() override;
    void dataFromJson(json_t* root) override;
    void random_settings();

    void onRandomize(const RandomizeEvent& e) override;
};


struct RailMenu : Hamburger
{
    SkiffUi* ui{nullptr};
    void setUi(SkiffUi* w) { ui = w; }
    void appendContextMenu(ui::Menu* menu) override;
};

struct SkiffUi : ModuleWidget, IThemeChange
{
    using Base = ModuleWidget;

    Skiff* my_module{nullptr};
    ThemeBase* theme_holder{nullptr};
    SvgCache my_svgs;

    bool request_custom_rail{false};
    bool no_history{false}; // suppress undo actions (e.g. during creation)
    RailMenu* ham{nullptr};
    TextButton* derail_button{nullptr};
    TextButton* nopanel_button{nullptr};
    TextButton* calm_button{nullptr};
    TextButton* unscrew_button{nullptr};
    TextButton* nojack_button{nullptr};
    TextButton* dark_ages_button{nullptr};
    TextButton* pack_button{nullptr};

    #ifdef HOT_SVG
    PositionIndex pos_widgets;
    #define HOT_POSITION(name,kind,widget) addPosition(pos_widgets, name, kind, widget)
    #else
    #define HOT_POSITION(name,kind,widget)
    #endif
    SkiffUi(Skiff* module);

    bool alive() { return my_module && !my_module->other_skiff; }

    TextButton* makeTextButton (
        std::map<std::string,::math::Rect>& bounds,
        const char* key,
        bool sticky,
        const char* title,
        const char* tip,
        std::shared_ptr<svg_theme::SvgTheme> svg_theme,
        std::function<void(bool,bool)> handler);
    void onChangeTheme(ChangedItem item) override;
    void sync_latch_state();
    void restore_rack();
    void from_module();
    void shouting_buttons(bool shouting);
    void no_panels(bool depanel);
    void set_unscrewed(bool unscrewed);
    void derail(bool derail);
    void set_nojacks(bool nojacks);
    void set_dark_ages(bool dark);
    void set_calm_rack(bool calm);
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

// ---- Undo/Redo -------------------------------

enum BinaryActionId {
    Derail, Depanel, Calm, Unscrew, Nojack, Darkness, Shouting
};

struct BinaryAction : ::rack::history::ModuleAction {
    BinaryActionId id;
    bool done_state;

    BinaryAction(int64_t module_id, BinaryActionId id, bool done_state);

    void set_desired_state(bool state);
    void undo() override { set_desired_state(!done_state); }
    void redo() override { set_desired_state(done_state); }
};

struct RailAction : ::rack::history::ModuleAction {
    std::string prev_rail;
    std::string next_rail;
    RailAction (int64_t module_id, const std::string& prev, const std::string& next) :
        prev_rail(prev), next_rail(next)
    {
        moduleId = module_id;
    }
    void undo() override;
    void redo() override;
};

}