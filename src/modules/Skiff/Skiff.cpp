#include <rack.hpp>
using namespace ::rack;
#include "services/json-help.hpp"
#include "services/open-file.hpp"
#include "services/rack-help.hpp"
#include "services/theme-module.hpp"
#include "services/svg-query.hpp"
#include "widgets/action-button.hpp"
#include "widgets/components.hpp"
#include "widgets/hamburger.hpp"
#include "widgets/screws.hpp"
#include "widgets/text-button.hpp"
#include "skiff-help.hpp"
#include "cloak.hpp"

using namespace svg_theme_2;
using namespace widgetry;
using namespace svg_query;

namespace pachde {

struct SkiffUi;

struct Skiff : ThemeModule
{
    using Base = ThemeModule;

    std::string custom_rail_file = asset::user("");
    int rail_item{-1};

    bool unscrewed{false};
    bool nojacks{false};
    bool calm{false};
    bool derailed{false};
    bool depaneled{false};
    bool fancy{false};
    bool shouting{true};

    bool running{false};
    std::string theme_name;
    SkiffUi* ui;

    json_t* dataToJson() override
    {
        json_t *root = Base::dataToJson();
        set_json(root, "rail-file", custom_rail_file);
        set_json_int(root, "alt-rail", rail_item);
        set_json(root, "unscrewed", unscrewed);
        set_json(root, "no-jacks", nojacks);
        set_json(root, "calm", calm);
        set_json(root, "derailed", derailed);
        set_json(root, "depaneled", depaneled);
        set_json(root, "fancy", fancy);
        set_json(root, "theme", theme_name);
        set_json(root, "shouting", shouting);
        return root;
    }

    void dataFromJson(json_t* root) override
    {
        Base::dataFromJson(root);
        custom_rail_file = get_json_string(root, "rail-file", custom_rail_file);
        rail_item = get_json_int(root, "alt-rail", rail_item);
        unscrewed = get_json_bool(root, "unscrewed", unscrewed);
        nojacks   = get_json_bool(root, "no-jacks", nojacks);
        calm      = get_json_bool(root, "calm", calm);
        derailed  = get_json_bool(root, "derailed", derailed);
        depaneled = get_json_bool(root, "depaneled", depaneled);
        fancy     = get_json_bool(root, "fancy", fancy);
        shouting  = get_json_bool(root, "shouting", shouting);
        apply_settings();
    };

    void apply_settings();

    void process(const ProcessArgs& args) override {
        running = true;
    }
};

struct SkiffSvg
{
    static std::string background() {
        return asset::plugin(pluginInstance, "res/Skiff.svg");
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

    Skiff* my_module{nullptr};
    ThemeBase* theme_holder{nullptr};
    SvgCache my_svgs;
    RailMenu* ham{nullptr};

    bool request_custom_rail{false};
    bool other_skiff{false};
    int rail_item{-1};

    bool unscrewed{false};
    bool nojacks{false};
    bool calm{false};
    bool derailed{false};
    bool depaneled{false};
    bool fancy{false};

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

    SkiffUi(Skiff* module) : my_module(module) {
        setModule(module);
        if (my_module) {
            my_module->ui = this;
        }
        derailed = !is_rail_visible();
        fancy = nullptr != getBackgroundCloak();

        theme_holder = my_module ? my_module : new ThemeBase();
        theme_holder->setNotify(this);
        makeUi();
    }

    void onChangeTheme(ChangedItem item) override {
        if (ChangedItem::Theme == item) {
            auto svg_theme = getThemeCache().getTheme(ThemeName(theme_holder->getTheme()));
            my_svgs.changeTheme(svg_theme);
            if (ham) ham->applyTheme(svg_theme);
            applyChildrenTheme(this, svg_theme);
            sendDirty(this);
        }
    }

    TextButton* makeTextButton (
        std::map<std::string,::math::Rect>& bounds,
        const char* key,
        bool sticky,
        const char* title,
        const char* tip,
        std::shared_ptr<svg_theme_2::SvgTheme> svg_theme,
        std::function<void(bool,bool)> handler)
    {
        auto button = new TextButton;
        button->box = bounds[key];
        HOT_POSITION(key, HotPosKind::Box, button);
        button->set_sticky(sticky);
        button->set_text(title);
        if (module) {
            if (tip) button->describe(tip);
            button->set_handler(handler);
        }
        button->applyTheme(svg_theme);
        return button;
    }

    void makeUi() {
        assert(children.empty());

        auto svg_theme = getThemeCache().getTheme(ThemeName(theme_holder->getTheme()));

        auto panel = createSvgThemePanel<SkiffSvg>(&my_svgs, nullptr);
        auto layout = panel->svg;
        setPanel(panel);

        other_skiff = !is_singleton(my_module, this);
        if (other_skiff) return;

        ::svg_query::BoundsIndex bounds;
        svg_query::addBounds(layout, "k:", bounds, true);

        ham= Center(createWidget<RailMenu>(bounds["k:rail-menu"].getCenter()));
        HOT_POSITION("k:rail-menu", HotPosKind::Center, ham);
        ham->setUi(this);
        ham->describe("Alternate Rails");
        ham->applyTheme(svg_theme);
        addChild(ham);

        addChild(derail_button = makeTextButton(bounds,"k:unrail-btn", true, "", "Toggle visible rails", svg_theme,
            [this](bool ctrl, bool shift) { derail(); }));

        addChild(fancy_button = makeTextButton(bounds, "k:bg-btn", true, "", "Toggle Fancy background", svg_theme,
            [this](bool ctrl, bool shift) { fancy_background(); }));

        addChild(nopanel_button = makeTextButton(bounds, "k:no-panel", true, "", "Toggle visibile panels", svg_theme,
            [this](bool ctrl, bool shift){ no_panels(); }));

        addChild(calm_button = makeTextButton(bounds, "k:calm-btn", true, "", "Toggle calm knobs and ports", svg_theme,
            [this](bool ctrl, bool shift) { calm_rack(); }));

        addChild(unscrew_button = makeTextButton(bounds, "k:screw-btn", true, "", "Toggle visible screws", svg_theme,
            [this](bool ctrl, bool shift) { toggle_screws(); }));

        addChild(nojack_button = makeTextButton(bounds, "k:nojack-btn", true, "", "Toggle visible unused jacks", svg_theme,
            [this](bool ctrl, bool shift) { toggle_jacks(); }));

        addChild(pack_button = makeTextButton(bounds, "k:pack-btn", false, "", "Pack (selected) modules (F7)", svg_theme,
            [this](bool ctrl, bool shift) { pack_modules(); }));

        auto button = Center(createThemeSvgButton<SmallActionButton>(&my_svgs, bounds["k:from-patch-btn"].getCenter()));
        HOT_POSITION("k:from-patch-btn", HotPosKind::Center, button);
        if (module) {
            button->describe("Apply last saved skiff");
            button->set_handler([this](bool ctrl, bool shift) { from_patch(); });
        }
        addChild(button);

        shouting_labels(my_module ? my_module->shouting : true);
        my_svgs.changeTheme(svg_theme);
        sync_latch_state();
    }

    void shouting_labels(bool shouting) {
        if (shouting) {
            derail_button->set_text("DERAIL");
            fancy_button->set_text("FANCYBOX");
            nopanel_button->set_text("NOPANELS");
            calm_button->set_text("CALM");
            unscrew_button->set_text("UNSCREW");
            nojack_button->set_text("NOJACK");
            pack_button->set_text("PACK'EM");
        } else {
            derail_button->set_text("deRail");
            fancy_button->set_text("FancyBox");
            nopanel_button->set_text("noPanels");
            calm_button->set_text("Calm");
            unscrew_button->set_text("unScrew");
            nojack_button->set_text("noJack");
            pack_button->set_text("Pack'em");
        }
    }

    void from_patch() {
        if (!my_module) return;

        unscrewed = my_module->unscrewed;
        screw_visibility(APP->scene->rack, !unscrewed);

        nojacks = my_module->nojacks;
        port_visibility(APP->scene->rack, !nojacks);

        calm = !my_module->calm; // invert because calm_rack() toggles
        calm_rack();

        depaneled = my_module->depaneled;
        panel_visibility(nullptr, !depaneled);

        {
            fancy = my_module->fancy;
            auto cloak = getBackgroundCloak();
            if (fancy) {
                if (!cloak) {
                    ensureBackgroundCloak();
                }
            } else {
                if (cloak) cloak->requestDelete();
            }
        }

        {
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
                        std::string rack_rail_file = get_rack_rail_filename();
                        // get cached rail svg
                        auto railSvg = window::Svg::load(rack_rail_file);
                        if (system::isFile(my_module->custom_rail_file)) {
                            // don't crash on bad svg
                            try {
                                railSvg->loadFile(my_module->custom_rail_file);
                            } catch (Exception& e) {
                                WARN("%s", e.what());
                            }
                            if (!railSvg->handle) {
                                railSvg->loadFile(rack_rail_file);
                                my_module->custom_rail_file.clear();
                                my_module->rail_item = rail_item = -1;
                            }
                        } else {
                            // no file -- fall back to Rack rails
                            railSvg->loadFile(rack_rail_file);
                            my_module->custom_rail_file.clear();
                            my_module->rail_item = rail_item = -1;
                        }
                    }
                }
            }
        }

        auto rail = APP->scene->rack->getFirstDescendantOfType<RailWidget>();
        if (rail) {
            derailed = my_module->derailed;
            rail->setVisible(!derailed);
        }

        sync_latch_state();
    }

    void sync_latch_state() {
        if (!my_module) return;

        derail_button->latched = derailed;
        fancy_button->latched = fancy;
        nopanel_button->latched = depaneled;
        calm_button->latched = calm;
        unscrew_button->latched = unscrewed;
        nojack_button->latched = nojacks;

        sendDirty(this);
    }

    void no_panels() {
        if (!my_module) return;

        depaneled = !depaneled;
        my_module->depaneled = depaneled;
        panel_visibility(nullptr, !depaneled);
    }

    void fancy_background() {
        if (!my_module) return;

        fancy = !fancy;
        my_module->fancy = fancy;
        auto cloak = getBackgroundCloak();
        if (fancy) {
            if (!cloak) ensureBackgroundCloak();
        } else {
            if (cloak) cloak->requestDelete();
        }
    }

    bool is_alt_rail() {
        auto railSvg = window::Svg::load(get_rack_rail_filename());
        return is_marked_svg(railSvg);
    }

    void derail() {
        if (!my_module) return;

        derailed = !derailed;
        my_module->derailed = derailed;
        set_rail_visible(!derailed);
    }

    void calm_rack() {
        if (!my_module) return;
        calm = !calm;
        my_module->calm = calm;
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
    }

    void toggle_screws() {
        if (!my_module) return;
        unscrewed = !unscrewed;
        my_module->unscrewed = unscrewed;
        screw_visibility(APP->scene->rack, !unscrewed);
    }

    void toggle_jacks() {
        if (!my_module) return;
        nojacks = !nojacks;
        my_module->nojacks = nojacks;
        port_visibility(APP->scene->rack, !nojacks);
    }

    std::string get_rack_rail_filename() {
        return asset::system(
            (settings::uiTheme == "light") ? "res/ComponentLibrary/Rail-light.svg" :
            (settings::uiTheme == "hcdark") ? "res/ComponentLibrary/Rail-hcdark.svg" :
            "res/ComponentLibrary/Rail.svg");
    }

    void alt_rail(int n) {
        if (!my_module) return;
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
        if (!my_module) return;
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
        Base::step();
        if (!my_module) return;
        if (request_custom_rail) {
            request_custom_rail = false;
            custom_rail();
        }
        theme_holder->pollRackThemeChanged();
    }

    void onHoverKey(const HoverKeyEvent& e) override
    {
        if (!my_module) return;
        auto mods = e.mods & RACK_MOD_MASK;
        switch (e.key) {
#ifdef HOT_SVG
        case GLFW_KEY_F5: {
            if (e.action == GLFW_RELEASE && (0 == mods)) {
                e.consume(this);
                my_svgs.reloadAll();
                reloadThemeCache();
                onChangeTheme(ChangedItem::Theme);
                if (!other_skiff) {
                    auto panel = dynamic_cast<SvgThemePanel<SkiffSvg>*>(getPanel());
                    positionWidgets(pos_widgets, makeBounds(panel->svg, "k:", true));
                }
                sendDirty(this);
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

        menu->addChild(createMenuLabel<HamburgerTitle>("#d Skiff"));
        menu->addChild(createCheckMenuItem("Shouting labels", "",
            [=](){ return my_module->shouting; },
            [=](){
                my_module->shouting = !my_module->shouting;
                shouting_labels(my_module->shouting);
            }
        ));
        menu->addChild(createMenuLabel<FancyLabel>("theme"));
        AddThemeMenu(menu, this, theme_holder, false, false, false);
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
