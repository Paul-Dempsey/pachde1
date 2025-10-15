#include <rack.hpp>
using namespace ::rack;
#include "../components.hpp"
#include "../open_file.hpp"
#include "../widgets/action-button.hpp"
#include "../widgets/hamburger.hpp"
#include "../widgets/screws.hpp"
#include "../services/json-help.hpp"
#include "../services/rack-help.hpp"
#include "skiff-help.hpp"
#include "cloak.hpp"

using namespace widgetry;

namespace pachde {

struct SkiffUi;

struct Skiff : ThemeModule
{
    using Base = ThemeModule;

    std::string custom_rail_file = asset::user("");
    int rail_item{-1};
    bool screws{false};
    bool calm{false};
    bool derailed{false};
    bool running{false};

    SkiffUi* ui;

    json_t* dataToJson() override
    {
        auto root = Base::dataToJson();
        set_json(root, "rail-file", custom_rail_file);
        set_json_int(root, "alt-rail", rail_item);
        set_json(root, "screws", screws);
        set_json(root, "calm", calm);
        set_json(root, "derailed", derailed);
        return root;
    }

    void dataFromJson(json_t* root) override
    {
        Base::dataFromJson(root);
        custom_rail_file = get_json_string(root, "rail-file", custom_rail_file);
        rail_item = get_json_int(root, "alt-rail", rail_item);
        screws = get_json_bool(root, "screws", screws);
        calm = get_json_bool(root, "calm", calm);
        derailed = get_json_bool(root, "derailed", derailed);

        apply_settings();
    };

    void apply_settings();

    void process(const ProcessArgs& args) override {
        running = true;
    }
};

struct SkiffSvg
{
    static std::string background(Theme theme)
    {
        const char * asset = "res/Skiff.svg";
        switch (theme) {
        case Theme::Dark: asset = "res/Skiff-dark.svg"; break;
        case Theme::HighContrast: asset = "res/Skiff-hc.svg"; break;

        case Theme::Unset:
        case Theme::Light:
        default: break;
        }
        return asset::plugin(pluginInstance, asset);
    }
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

    Skiff* my_module;
    ThemeBase* theme_holder{nullptr};

    bool request_custom_rail{false};
    bool other_skiff{false};
    int rail_item{-1};
    bool screws{false};
    bool calm{false};
    bool derailed{false};

#ifdef HOT_SVG
    std::map<const char *, Widget*> positioned_widgets;
#endif

    virtual ~SkiffUi()
    {
        if (theme_holder && !module) {
            delete theme_holder;
        }
    }

    SkiffUi(Skiff* module) : my_module(module)
    {
        setModule(module);
        if (my_module) {
            my_module->ui = this;
        }

        derailed = is_rail_visible();
        theme_holder = module ? module : new ThemeBase();
        theme_holder->setNotify(this);
        applyTheme(GetPreferredTheme(theme_holder));
    }

#ifdef HOT_SVG
#define HOT_POSITION(name,widget) positioned_widgets[name] = widget
#else
#define HOT_POSITION(name,widget) void
#endif

    void makeUi(Theme theme) {
        assert(children.empty());
        setPanel(createSvgThemePanel<SkiffSvg>(theme));

        other_skiff = !is_singleton(my_module, this);
        if (other_skiff) return;

        SmallButton* button;
        auto layout = window::Svg::load(SkiffSvg::background(Theme::Light));

        std::map<std::string, ::math::Rect> bounds;
        svg_query::boundsIndex(layout, "k:", bounds, true);

        auto menu = createThemeWidgetCentered<RailMenu>(theme, bounds["k:rail-menu"].getCenter());
        HOT_POSITION("k:rail-menu", menu);
        menu->setUi(this);
        menu->describe("Change Rails");
        addChild(menu);

        button = createThemeWidgetCentered<SmallButton>(theme, bounds["k:unrail-btn"].getCenter());
        HOT_POSITION("k:unrail-btn", button);
        button->set_sticky(true);
        button->latched = derailed;
        if (module) {
            button->describe("Derail");
            button->setHandler([this](bool ctrl, bool shift) { derail(); });
        }
        addChild(button);


        button = createThemeWidgetCentered<SmallButton>(theme, bounds["k:bg-btn"].getCenter());
        HOT_POSITION("k:bg-btn", button);
        button->set_sticky(true);
        button->latched = derailed;
        if (module) {
            button->describe("Fancy background");
            button->setHandler([this](bool ctrl, bool shift) { fancy_background(); });
        }
        addChild(button);

        button = createThemeWidgetCentered<SmallButton>(theme, bounds["k:calm-btn"].getCenter());
        HOT_POSITION("k:calm-btn", button);
        if (module) {
            button->describe("Calm knobs and ports");
            button->setHandler([this](bool ctrl, bool shift) { calm_rack(); });
        }
        addChild(button);

        button = createThemeWidgetCentered<SmallButton>(theme, bounds["k:screw-btn"].getCenter());
        HOT_POSITION("k:screw-btn", button);
        button->set_sticky(true);
        if (module) {
            button->describe("Unscrew");
            button->setHandler([this](bool ctrl, bool shift) { toggle_screws(); });
        }
        addChild(button);

        button = createThemeWidgetCentered<SmallButton>(theme, bounds["k:pack-btn"].getCenter());
        HOT_POSITION("k:pack-btn", button);
        if (module) {
            button->describe("Pack modules (F7)");
            button->setHandler([](bool ctrl, bool shift) { pack_modules(); });
        }
        addChild(button);

        button = createThemeWidgetCentered<SmallButton>(theme, bounds["k:from-patch-btn"].getCenter());
        HOT_POSITION("k:from-patch-btn", button);
        if (module) {
            button->describe("Apply last saved skiff");
            button->setHandler([this](bool ctrl, bool shift) { from_patch(); });
        }
        addChild(button);
    }

    void applyTheme(Theme theme) {
        if (children.empty()) {
            makeUi(theme);
        } else {
            if (Theme::Light == theme) {
                auto layout = window::Svg::load(SkiffSvg::background(Theme::Light));
                svg_query::hideElements(layout, "k:");
            }
            SetChildrenTheme(this, theme);
        }
    }

    void onChangeTheme(ChangedItem item) override {
        switch (item) {
        case ChangedItem::Theme:
            applyTheme(GetPreferredTheme(theme_holder));
            break;
        case ChangedItem::DarkTheme:
            applyTheme(GetPreferredTheme(theme_holder));
            break;
        case ChangedItem::FollowDark:
            if (theme_holder->getFollowRack()) {
                applyTheme(GetPreferredTheme(theme_holder));
            }
            break;
        case ChangedItem::MainColor:
            break;
        case ChangedItem::Screws:
            break;
        }
    }

    void fancy_background()
    {
        if (!my_module) return;
        toggleCloakWidget();
    }

    void from_patch() {
        if (!my_module) return;

        screws = my_module->screws;
        screw_visibility(APP->scene->rack, screws);

        calm = !my_module->calm; // invert because calm_rack() toggles
        calm_rack();

        bool altered = is_alt_rail();
        rail_item = my_module->rail_item;
        if (rail_item < 0) {
            if (altered) {
                std::string rail_file = get_rack_rail_filename();
                std::shared_ptr<window::Svg> railSvg = window::Svg::load(rail_file);
                railSvg->loadFile(rail_file);
            }
        } else {
            if (!altered) {
                if (rail_item < 6) {
                    alt_rail(rail_item);
                } else {
                    std::string rail_file = get_rack_rail_filename();
                    std::shared_ptr<window::Svg> railSvg = window::Svg::load(rail_file);
                    if (system::isFile(my_module->custom_rail_file)) {
                        railSvg->loadFile(my_module->custom_rail_file);
                        if (!railSvg->handle) {
                            railSvg->loadFile(rail_file);
                            my_module->rail_item = rail_item = -1;
                        }
                    } else {
                        railSvg->loadFile(rail_file);
                        my_module->rail_item = rail_item = -1;
                    }
                }
            }
        }
        auto rail = APP->scene->rack->getFirstDescendantOfType<RailWidget>();
        if (rail) {
            rail->setVisible(my_module->derailed);
            derailed = my_module->derailed;
        }
    }

    bool is_alt_rail() {
        auto railSvg = window::Svg::load(get_rack_rail_filename());
        return is_marked_svg(railSvg);
    }

    void derail() {
        derailed = !derailed;
        set_rail_visible(derailed);
        if (my_module) {
            my_module->derailed = derailed;
        }
    }

    void calm_rack() {
        calm = !calm;
        if (calm) {
            replace_system_svg("res/ComponentLibrary/RoundBlackKnob_bg.svg", "res/calm/alt-RoundBlackKnob_bg.svg");
            replace_system_svg("res/ComponentLibrary/RoundBlackKnob.svg", "res/calm/alt-RoundBlackKnob.svg");
            replace_system_svg("res/ComponentLibrary/RoundBigBlackKnob_bg.svg", "res/calm/alt-RoundBigBlackKnob_bg.svg");
            replace_system_svg("res/ComponentLibrary/RoundBigBlackKnob.svg", "res/calm/alt-RoundBigBlackKnob.svg");
            replace_system_svg("res/ComponentLibrary/RoundHugeBlackKnob_bg.svg", "res/calm/alt-RoundHugeBlackKnob_bg.svg");
            replace_system_svg("res/ComponentLibrary/RoundHugeBlackKnob.svg", "res/calm/alt-RoundHugeBlackKnob.svg");
            replace_system_svg("res/ComponentLibrary/RoundLargeBlackKnob_bg.svg", "res/calm/alt-RoundLargeBlackKnob_bg.svg");
            replace_system_svg("res/ComponentLibrary/RoundLargeBlackKnob.svg", "res/calm/alt-RoundLargeBlackKnob.svg");
            replace_system_svg("res/ComponentLibrary/RoundSmallBlackKnob_bg.svg", "res/calm/alt-RoundSmallBlackKnob_bg.svg");
            replace_system_svg("res/ComponentLibrary/RoundSmallBlackKnob.svg", "res/calm/alt-RoundSmallBlackKnob.svg");
            replace_system_svg("res/ComponentLibrary/Trimpot_bg.svg", "res/calm/alt-Trimpot_bg.svg");
            replace_system_svg("res/ComponentLibrary/Trimpot.svg", "res/calm/alt-Trimpot.svg");
            // ports
            replace_system_svg("res/ComponentLibrary/CL1362.svg", "res/calm/alt-CL1362.svg");
            replace_system_svg("res/ComponentLibrary/PJ301M-dark.svg", "res/calm/alt-PJ301M-dark.svg");
            replace_system_svg("res/ComponentLibrary/PJ301M.svg", "res/calm/alt-PJ301M.svg");
            replace_system_svg("res/ComponentLibrary/PJ3410.svg", "res/calm/alt-PJ3410.svg");
        } else {
            original_system_svg("res/ComponentLibrary/RoundBlackKnob_bg.svg");
            original_system_svg("res/ComponentLibrary/RoundBlackKnob.svg");
            original_system_svg("res/ComponentLibrary/RoundBigBlackKnob_bg.svg");
            original_system_svg("res/ComponentLibrary/RoundBigBlackKnob.svg");
            original_system_svg("res/ComponentLibrary/RoundHugeBlackKnob_bg.svg");
            original_system_svg("res/ComponentLibrary/RoundHugeBlackKnob.svg");
            original_system_svg("res/ComponentLibrary/RoundLargeBlackKnob_bg.svg");
            original_system_svg("res/ComponentLibrary/RoundLargeBlackKnob.svg");
            original_system_svg("res/ComponentLibrary/RoundSmallBlackKnob_bg.svg");
            original_system_svg("res/ComponentLibrary/RoundSmallBlackKnob.svg");
            original_system_svg("res/ComponentLibrary/Trimpot_bg.svg");
            original_system_svg("res/ComponentLibrary/Trimpot.svg");
            // ports
            original_system_svg("res/ComponentLibrary/CL1362.svg");
            original_system_svg("res/ComponentLibrary/PJ301M-dark.svg");
            original_system_svg("res/ComponentLibrary/PJ301M.svg");
            original_system_svg("res/ComponentLibrary/PJ3410.svg");
        }
        APP->scene->onDirty(DirtyEvent{});
        if (my_module) {
            my_module->calm = calm;
        }
    }

    void toggle_screws() {
        screws = !screws;
        screw_visibility(APP->scene->rack, screws);
        if (my_module) {
            my_module->screws = screws;
        }
    }

    std::string get_rack_rail_filename() {
        return asset::system(
            (settings::uiTheme == "light") ? "res/ComponentLibrary/Rail-light.svg" :
            (settings::uiTheme == "hcdark") ? "res/ComponentLibrary/Rail-hcdark.svg" :
            "res/ComponentLibrary/Rail.svg");
    }

    void alt_rail(int n) {
        auto rail = APP->scene->rack->getFirstDescendantOfType<RailWidget>();
        if (!rail) return;

        std::string rail_file = get_rack_rail_filename();
        std::shared_ptr<window::Svg> railSvg = window::Svg::load(rail_file);
        assert(railSvg);
        if (!railSvg) return;

        rail_item = n;
        if (n >= 0 && n <= 5) {
            switch (n) {
            case 0: railSvg->loadFile(asset::plugin(pluginInstance, "res/rails/alt-rail-0.svg")); break;
            case 1: railSvg->loadFile(asset::plugin(pluginInstance, "res/rails/alt-rail-1.svg")); break;
            case 2: railSvg->loadFile(asset::plugin(pluginInstance, "res/rails/alt-rail-2.svg")); break;
            case 3: railSvg->loadFile(asset::plugin(pluginInstance, "res/rails/alt-rail-3.svg")); break;
            case 4: railSvg->loadFile(asset::plugin(pluginInstance, "res/rails/alt-rail-blank.svg")); break;
            case 5: railSvg->loadFile(asset::plugin(pluginInstance, "res/rails/alt-rail-grad.svg")); break;
            }
        }
        if (n < 0 || !railSvg->handle) {
            railSvg->loadFile(rail_file);
            rail_item = -1;
        }
        rail->onDirty(DirtyEvent{});
        if (my_module) {
            my_module->rail_item = rail_item;
        }
    }

    void pend_custom_rail() {
        request_custom_rail = true;
    }

    void custom_rail() {
        if (!module) return;

        auto rail = APP->scene->rack->getFirstDescendantOfType<RailWidget>();
        if (!rail) return;

        std::string filename = system::getFilename(my_module->custom_rail_file);
        std::string folder = system::getDirectory(my_module->custom_rail_file);
        if (openFileDialog(folder, "SVG Files (.svg):svg;Any (*):", filename, filename)) {
            my_module->custom_rail_file = filename;
            std::string rail_file = get_rack_rail_filename();
            std::shared_ptr<window::Svg> railSvg = window::Svg::load(rail_file);
            if (railSvg) {
                railSvg->loadFile(filename);
                rail_item = 6;
                if (!railSvg->handle) {
                    railSvg->loadFile(rail_file);
                    rail_item = -1;
                } else {
                    add_marker(railSvg);
                }
                rail->onDirty(DirtyEvent{});
            }
        }
    }

    void step() override {
        if (request_custom_rail) {
            request_custom_rail = false;
            custom_rail();
        }
        theme_holder->pollRackDarkChanged();
    }

    void onHoverKey(const HoverKeyEvent& e) override
    {
        auto mods = e.mods & RACK_MOD_MASK;
        switch (e.key) {

#ifdef HOT_SVG
        case GLFW_KEY_F5: {
            if (e.action == GLFW_RELEASE && (0 == mods)) {
                e.consume(this);
                auto panel = dynamic_cast<SvgThemePanel<SkiffSvg>*>(getPanel());
                if (panel) {
                    auto layout = window::Svg::load(SkiffSvg::background(Theme::Light));
                    std::map<std::string, ::math::Rect> bounds;
                    svg_query::boundsIndex(layout, "k:", bounds, true);
                    for (auto kv: positioned_widgets) {
                        kv.second->box.pos = bounds[kv.first].getCenter();
                        Center(kv.second);
                    }
                    panel->updatePanel(theme_holder->getTheme());
                }
            }
        } break;
#endif

        case GLFW_KEY_F6: {
            if (e.action == GLFW_PRESS && (0 == mods)) {
                e.consume(this);
                zoom_selected();
            }
        } break;
        case GLFW_KEY_F7: {
            if (e.action == GLFW_PRESS && (0 == mods)) {
                e.consume(this);
                pack_modules();
            }
        } break;
        }
        Base::onHoverKey(e);
    }

    void draw(const DrawArgs& args) override {
        Base::draw(args);
        if (my_module && other_skiff) {
            draw_disabled_panel(this, GetPreferredTheme(theme_holder), args, 20.f, 20.f);
        }
    }

    void appendContextMenu(Menu* menu) override {
        if (!module) return;

        menu->addChild(createMenuLabel<HamburgerTitle>("Skiff"));
        AddThemeMenu(menu, theme_holder, false, false);
    }

};

void Skiff::apply_settings()
{
    if (running && ui) {
        ui->from_patch();
    }
}

void RailMenu::appendContextMenu(ui::Menu* menu)
{
    if (!ui->module) return;
    menu->addChild(createMenuLabel<HamburgerTitle>("Alternate rails"));
    menu->addChild(createMenuItem("Rack rail", (ui->rail_item < 0) ? "●": "", [this](){ ui->alt_rail(-1); } ));
    menu->addChild(new MenuSeparator);
    menu->addChild(createMenuItem("Alt rail 1", (ui->rail_item == 0) ? "●": "", [this](){ ui->alt_rail(0); } ));
    menu->addChild(createMenuItem("Alt rail 2", (ui->rail_item == 1) ? "●": "", [this](){ ui->alt_rail(1); } ));
    menu->addChild(createMenuItem("Alt rail 3", (ui->rail_item == 2) ? "●": "", [this](){ ui->alt_rail(2); } ));
    menu->addChild(createMenuItem("Alt rail 4", (ui->rail_item == 3) ? "●": "", [this](){ ui->alt_rail(3); } ));
    menu->addChild(createMenuItem("Alt rail 5", (ui->rail_item == 4) ? "●": "", [this](){ ui->alt_rail(4); } ));
    menu->addChild(createMenuItem("Alt rail 6", (ui->rail_item == 5) ? "●": "", [this](){ ui->alt_rail(5); } ));
    menu->addChild(createMenuItem("Custom rail SVG", (ui->rail_item == 6) ? "●": "", [this]() { ui->pend_custom_rail(); } ));
}

}
Model* modelSkiff = createModel<pachde::Skiff, pachde::SkiffUi>("pachde-skiff");
