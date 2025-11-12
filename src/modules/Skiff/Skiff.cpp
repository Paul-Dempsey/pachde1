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

    std::string rail{"Rack"};
    std::string rail_folder = asset::userDir;

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
        set_json(root, "rail", rail);
        set_json(root, "rail-folder", rail_folder);
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
        rail      = get_json_string(root, "rail", rail);
        rail_folder = get_json_string(root, "rail-folder", rail_folder);
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

const std::set<std::string> known_rails{"Rack","Plain","Simple","No-hole","Pinstripe","Gradient","Blank"};

struct SkiffUi : ModuleWidget, IThemeChange
{
    using Base = ModuleWidget;

    Skiff* my_module{nullptr};
    ThemeBase* theme_holder{nullptr};
    SvgCache my_svgs;
    RailMenu* ham{nullptr};

    bool request_custom_rail{false};
    bool other_skiff{false};

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
            my_module->rail = alt_rail_name();
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

        alt_rail(my_module->rail);

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

    const char * alt_rail_name() {
        auto railSvg = window::Svg::load(get_rack_rail_filename());
        auto name = marker_name(railSvg);
        return (*name) ? name :"Rack";
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

    void alt_rail(const std::string& rail_name) {
        if (!my_module) return;
        auto rail = APP->scene->rack->getFirstDescendantOfType<RailWidget>();
        if (!rail) return;

        std::string rail_file = get_rack_rail_filename();
        std::shared_ptr<window::Svg> railSvg = window::Svg::load(rail_file);
        assert(railSvg);
        if (!railSvg) return;

        auto ext = system::getExtension(rail_name);
        if (ext.size()) { // custom
            try {
                railSvg->loadFile(rail_name);
                my_module->rail = rail_name;
                add_named_marker(railSvg, system::getStem(rail_name));
            } catch (Exception& e) {
                WARN("%s", e.msg.c_str());
                railSvg->loadFile(rail_file);
                my_module->rail = "Rack";
            }
        } else { // builtin
            if (0 == rail_name.compare("Rack")) {
                railSvg->loadFile(rail_file);
                my_module->rail = "Rack";
            } else {
                auto filename = asset::plugin(pluginInstance, format_string("res/rails/%s.svg", rail_name.c_str()));
                if (system::exists(filename)) {
                    railSvg->loadFile(filename);
                    my_module->rail = rail_name;
                } else {
                    assert(false);
                    railSvg->loadFile(rail_file);
                    my_module->rail = "Rack";
                }
            }
        }
        rail->onDirty(DirtyEvent{});
    }

    void pend_custom_rail() {
        if (!my_module) return;
        request_custom_rail = true;
    }

    void custom_rail() {
        if (!module) return;

        auto rail = APP->scene->rack->getFirstDescendantOfType<RailWidget>();
        if (!rail) return;

        std::string filename;
        std::string folder = my_module->rail_folder;
        auto ext = system::getExtension(my_module->rail);
        if (ext.size()) {
            filename = system::getFilename(my_module->rail);
        }
        if (openFileDialog(folder, "SVG Files (.svg):svg;Any (*):", filename, filename)) {
            my_module->rail_folder = system::getDirectory(filename);
            std::string rail_file = get_rack_rail_filename();
            std::shared_ptr<window::Svg> railSvg = window::Svg::load(rail_file);
            if (railSvg) {
                try {
                    railSvg->loadFile(filename);
                } catch (Exception& e) {
                    WARN("%s", e.msg.c_str());
                }
                if (railSvg->handle) {
                    std::string name = system::getStem(filename);
                    add_named_marker(railSvg, name);
                    my_module->rail = filename;
                } else {
                    railSvg->loadFile(rail_file);
                    my_module->rail = "Rack";
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

inline const char * menu_bullet(bool other, const std::string& name, const char *item) {
    if (other) return "";
    return (0 == name.compare(item)) ? "●": "";
}

inline bool known_rail(const char * name) {
    return known_rails.find(name) != known_rails.cend();
}

void RailMenu::appendContextMenu(ui::Menu* menu)
{
    if (!ui->module) return;
    auto rail_name = ui->alt_rail_name();
    bool custom = !known_rail(rail_name);

    menu->addChild(createMenuLabel<HamburgerTitle>("Alternate rails"));
    menu->addChild(createMenuItem("Rack rail", menu_bullet(custom, rail_name, "Rack"), [this](){ ui->alt_rail("Rack"); } ));
    menu->addChild(createMenuLabel<FancyLabel>(rail_name));
    menu->addChild(createMenuItem("Plain",     menu_bullet(custom, rail_name, "Plain"),     [this](){ ui->alt_rail("Plain"); } ));
    menu->addChild(createMenuItem("Simple",    menu_bullet(custom, rail_name, "Simple"),    [this](){ ui->alt_rail("Simple"); } ));
    menu->addChild(createMenuItem("No-hole",   menu_bullet(custom, rail_name, "No-hole"),   [this](){ ui->alt_rail("No-hole"); } ));
    menu->addChild(createMenuItem("Pinstripe", menu_bullet(custom, rail_name, "Pinstripe"), [this](){ ui->alt_rail("Pinstripe"); } ));
    menu->addChild(createMenuItem("Gradient",  menu_bullet(custom, rail_name, "Gradient"),  [this](){ ui->alt_rail("Gradient"); } ));
    menu->addChild(createMenuItem("Blank",     menu_bullet(custom, rail_name, "Blank"),     [this](){ ui->alt_rail("Blank"); } ));
    menu->addChild(createMenuItem("Custom rail SVG", custom ? "●": "", [this]() { ui->pend_custom_rail(); } ));
}

}
Model* modelSkiff = createModel<pachde::Skiff, pachde::SkiffUi>("pachde-skiff");
