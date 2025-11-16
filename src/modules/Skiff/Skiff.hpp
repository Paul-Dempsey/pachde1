#pragma once
#include <rack.hpp>
using namespace ::rack;
#include "services/theme-module.hpp"
#include "services/svg-query.hpp"
using namespace svg_query;
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

    bool unscrewed{false};
    bool nojacks{false};
    bool calm{false};
    bool derailed{false};
    bool depaneled{false};
    bool fancy{false};
    bool shouting{true};

    std::string theme_name;
    RailThemeSetting rail_theme;

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
    bool other_skiff{false};

    RailMenu* ham{nullptr};
    TextButton* derail_button{nullptr};
    TextButton* fancy_button{nullptr};
    TextButton* nopanel_button{nullptr};
    TextButton* calm_button{nullptr};
    TextButton* unscrew_button{nullptr};
    TextButton* nojack_button{nullptr};
    TextButton* pack_button{nullptr};

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
    void sync_latch_state();
    void restore_rack();
    void from_module();
    void shouting_buttons(bool shouting);
    void no_panels(bool depanel);
    void set_unscrewed(bool unscrewed);
    void derail(bool derail);
    void fancy_background(bool fancy);
    void set_nojacks(bool nojacks);
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