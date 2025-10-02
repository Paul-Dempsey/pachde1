#include <rack.hpp>
using namespace ::rack;
#include "../components.hpp"
#include "../open_file.hpp"
#include "../widgets/action-button.hpp"
#include "../widgets/hamburger.hpp"
#include "../widgets/screws.hpp"
#include "../services/json-help.hpp"
#include "skiff-help.hpp"

using namespace widgetry;

namespace pachde {

struct SkiffModule : ThemeModule
{
    using Base = ThemeModule;
    std::string custom_rail_file = asset::user("");

    json_t* dataToJson() override
    {
        auto root = json_object();
        set_json(root, "rail-file", custom_rail_file);
        return root;
    }

    void dataFromJson(json_t* root) override
    {
        custom_rail_file = get_json_string(root, "rail-file", custom_rail_file);
    };
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

struct SkiffUi;

struct RailMenu : Hamburger
{
    SkiffUi* ui{nullptr};
    void setUi(SkiffUi* w) { ui = w; }
    void appendContextMenu(ui::Menu* menu) override;
};

struct SkiffUi : ModuleWidget, IThemeChange
{
    using Base = ModuleWidget;

    SkiffModule* my_module;
    ThemeBase* theme_holder{nullptr};
    SmallButton* unscrew_button{nullptr};
    bool screws{true};
    bool request_custom_rail{false};
    bool other_skiff{false};
    bool calm{false};
    int rail_item{-1};

    virtual ~SkiffUi()
    {
        if (theme_holder && !module) {
            delete theme_holder;
        }
    }

    SkiffUi(SkiffModule* module) : my_module(module)
    {
        setModule(module);
        theme_holder = module ? module : new ThemeBase();
        theme_holder->setNotify(this);
        applyTheme(GetPreferredTheme(theme_holder));
    }

    bool is_singleton()
    {
        if (my_module) {
            auto mws = APP->scene->rack->getModules();
            if (mws.size() > 1) {
                for (auto mw: mws) {
                    if ((mw != this) && (mw->getModel() == my_module->getModel())) {
                        return false;
                    }
                }
            }
        }
        return true;
    }

    void makeUi(Theme theme) {
        assert(children.empty());
        setPanel(createSvgThemePanel<SkiffSvg>(theme));

        other_skiff = !is_singleton();
        if (other_skiff) return;

        SmallButton* button;
        auto layout = window::Svg::load(SkiffSvg::background(Theme::Light));
        std::map<std::string, ::math::Rect> bounds;
        svg_query::boundsIndex(layout, "k:", bounds, true);

        auto menu = createThemeWidgetCentered<RailMenu>(theme, bounds["k:rail-menu"].getCenter());
        menu->setUi(this);
        menu->describe("Change Rails");
        addChild(menu);

        button = createThemeWidgetCentered<SmallButton>(theme, bounds["k:unrail-btn"].getCenter());
        if (module) {
            button->describe("Derail (toggle)");
            button->setHandler([](bool ctrl, bool shift) { derail(); });
        }
        addChild(button);

        button = createThemeWidgetCentered<SmallButton>(theme, bounds["k:calm-btn"].getCenter());
        if (module) {
            button->describe("Calm knobs and ports");
            button->setHandler([this](bool ctrl, bool shift) { calm_rack(); });
        }
        addChild(button);

        unscrew_button = createThemeWidgetCentered<SmallButton>(theme, bounds["k:screw-btn"].getCenter());
        unscrew_button->set_sticky(true);
        if (module) {
            unscrew_button->describe("Unscrew (toggle)");
            unscrew_button->setHandler([this](bool ctrl, bool shift) { toggle_screws(); });
        }
        addChild(unscrew_button);

        button = createThemeWidgetCentered<SmallButton>(theme, bounds["k:pack-btn"].getCenter());
        if (module) {
            button->describe("Pack modules");
            button->setHandler([](bool ctrl, bool shift) { packModules(); });
        }
        addChild(button);
    }

    void applyTheme(Theme theme) {
        if (children.empty()) {
            makeUi(theme);
        } else {
            SetChildrenTheme(this, theme);
        }
    }

    void onChangeTheme(ChangedItem item) override {
        switch (item) {
        case ChangedItem::Theme:
            applyTheme(GetPreferredTheme(theme_holder));
            break;
        case ChangedItem::DarkTheme:
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

    void replace_svg(const char * rack, const char *alt)
    {
        window::Svg::load(asset::system(rack))->loadFile(asset::plugin(pluginInstance, alt));
    }

    void rack_svg(const char * rack)
    {
        auto f = asset::system(rack);
        window::Svg::load(f)->loadFile(f);
    }

    void calm_rack() {
        calm = !calm;
        if (calm) {
            replace_svg("res/ComponentLibrary/RoundBlackKnob_bg.svg", "res/calm/alt-RoundBlackKnob_bg.svg");
            replace_svg("res/ComponentLibrary/RoundBlackKnob.svg", "res/calm/alt-RoundBlackKnob.svg");
            replace_svg("res/ComponentLibrary/RoundBigBlackKnob_bg.svg", "res/calm/alt-RoundBigBlackKnob_bg.svg");
            replace_svg("res/ComponentLibrary/RoundBigBlackKnob.svg", "res/calm/alt-RoundBigBlackKnob.svg");
            replace_svg("res/ComponentLibrary/RoundHugeBlackKnob_bg.svg", "res/calm/alt-RoundHugeBlackKnob_bg.svg");
            replace_svg("res/ComponentLibrary/RoundHugeBlackKnob.svg", "res/calm/alt-RoundHugeBlackKnob.svg");
            replace_svg("res/ComponentLibrary/RoundLargeBlackKnob_bg.svg", "res/calm/alt-RoundLargeBlackKnob_bg.svg");
            replace_svg("res/ComponentLibrary/RoundLargeBlackKnob.svg", "res/calm/alt-RoundLargeBlackKnob.svg");
            replace_svg("res/ComponentLibrary/RoundSmallBlackKnob_bg.svg", "res/calm/alt-RoundSmallBlackKnob_bg.svg");
            replace_svg("res/ComponentLibrary/RoundSmallBlackKnob.svg", "res/calm/alt-RoundSmallBlackKnob.svg");
            replace_svg("res/ComponentLibrary/Trimpot_bg.svg", "res/calm/alt-Trimpot_bg.svg");
            replace_svg("res/ComponentLibrary/Trimpot.svg", "res/calm/alt-Trimpot.svg");
            // ports
            replace_svg("res/ComponentLibrary/CL1362.svg", "res/calm/alt-CL1362.svg");
            replace_svg("res/ComponentLibrary/PJ301M-dark.svg", "res/calm/alt-PJ301M-dark.svg");
            replace_svg("res/ComponentLibrary/PJ301M.svg", "res/calm/alt-PJ301M.svg");
            replace_svg("res/ComponentLibrary/PJ3410.svg", "res/calm/alt-PJ3410.svg");
        } else {
            rack_svg("res/ComponentLibrary/RoundBlackKnob_bg.svg");
            rack_svg("res/ComponentLibrary/RoundBlackKnob.svg");
            rack_svg("res/ComponentLibrary/RoundBigBlackKnob_bg.svg");
            rack_svg("res/ComponentLibrary/RoundBigBlackKnob.svg");
            rack_svg("res/ComponentLibrary/RoundHugeBlackKnob_bg.svg");
            rack_svg("res/ComponentLibrary/RoundHugeBlackKnob.svg");
            rack_svg("res/ComponentLibrary/RoundLargeBlackKnob_bg.svg");
            rack_svg("res/ComponentLibrary/RoundLargeBlackKnob.svg");
            rack_svg("res/ComponentLibrary/RoundSmallBlackKnob_bg.svg");
            rack_svg("res/ComponentLibrary/RoundSmallBlackKnob.svg");
            rack_svg("res/ComponentLibrary/Trimpot_bg.svg");
            rack_svg("res/ComponentLibrary/Trimpot.svg");
            // ports
            rack_svg("res/ComponentLibrary/CL1362.svg");
            rack_svg("res/ComponentLibrary/PJ301M-dark.svg");
            rack_svg("res/ComponentLibrary/PJ301M.svg");
            rack_svg("res/ComponentLibrary/PJ3410.svg");
        }
        APP->scene->onDirty(DirtyEvent{});
    }

    void toggle_screws() {
        screws = !screws;
        screw_visibility(APP->scene->rack, screws);
    }

    std::string get_rack_rail_filename() {
        return asset::system((settings::uiTheme == "light") ? "res/ComponentLibrary/Rail-light.svg" :
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

    void draw(const DrawArgs& args) override {
        Base::draw(args);
        if (my_module && other_skiff) {
            auto vg = args.vg;
            NVGcolor co1, co2;

            switch (theme_holder->getTheme()) {
            default:
            case Theme::Light: co1 = nvgRGB(0x80, 0x80, 0x80); co2 = nvgRGB(0x40, 0x40, 0x40); break;
            case Theme::Dark: co1 = nvgRGB(0x50, 0x50, 0x50);  co2 = nvgRGB(0x28, 0x28, 0x28); break;
            case Theme::HighContrast: co1 = nvgRGB(0,0,0); co2 = co1; break;
            }
            nvgBeginPath(vg);
            nvgRect(vg, 0.f, 20.f, 30.f, 360.f);
            auto p = nvgLinearGradient(vg, 0.f, 20.f, 30.f, 360.f, co1, co2);
            nvgFillPaint(vg, p);
            nvgFill(vg);

            nvgBeginPath(vg);
            nvgCircle(vg, 15.f, 118.5f, 10.f);
            nvgStrokeWidth(vg, 2.5f);
            nvgStrokeColor(vg, nvgRGB(0xff, 0x20, 0x20));
            nvgStroke(vg);

            nvgBeginPath(vg);
            nvgMoveTo(vg, 10.f,114.f);
            nvgLineTo(vg, 20.f,124.f);
            nvgMoveTo(vg, 20.f,114.f);
            nvgLineTo(vg, 10.f,124.f);
            nvgStrokeColor(vg, nvgRGB(0xff, 0x20, 0x20));
            nvgStroke(vg);
        }
    }

    void appendContextMenu(Menu* menu) override {
        if (!module) return;
        AddThemeMenu(menu, theme_holder, false, false);
    }

};

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
Model* modelSkiff = createModel<pachde::SkiffModule, pachde::SkiffUi>("pachde-skiff");
