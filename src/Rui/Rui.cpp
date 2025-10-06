#include <rack.hpp>
using namespace ::rack;
#include "../components.hpp"
#include "../services/json-help.hpp"
#include "../widgets/action-button.hpp"
#include "../widgets/hamburger.hpp"
#include "../services/rack-help.hpp"
using namespace widgetry;

namespace pachde {

struct Rui : ThemeModule
{
    using Base = ThemeModule;

    bool single{true};
    bool running{false};
    bool stopped{false};
    bool fluff{true};

    float initialCableOpacity;
    float initialCableTension;
    float initialBright;
    float initialBloom;

    enum Params {
        CableOpacity,
        CableTension,
        Bright,
        Bloom,

        ModCableOpacity,
        ModCableTension,
        ModBright,
        ModBloom,

        NUM_PARAMS
    };
    enum Inputs {
        InCableOpacity,
        InCableTension,
        InBright,
        InBloom,
        NUM_INPUTS
    };
    enum Outputs {
        OutVolt,
        NUM_OUTPUTS
    };
    enum Lights {
        NUM_LIGHTS
    };

    Rui() {
        config(Params::NUM_PARAMS, Inputs::NUM_INPUTS, Outputs::NUM_OUTPUTS, Lights::NUM_LIGHTS);

        initialCableOpacity = ::rack::settings::cableOpacity;
        initialCableTension = ::rack::settings::cableTension;
        initialBright = ::rack::settings::rackBrightness;
        initialBloom = ::rack::settings::haloBrightness;

        dp4(configParam(Params::CableOpacity, 0, 100, 37.1f, "Cable opacity", "%"))->setValue(initialCableOpacity * 100.f);
        dp4(configParam(Params::CableTension, 0, 100, 50, "Cable tension", "%"))->setValue(initialCableTension * 100.f);
        dp4(configParam(Params::Bright, 0, 100, 100, "Room brightness", "%"))->setValue(initialBright * 100.f);
        dp4(configParam(Params::Bloom, 0, 100, 25, "Light bloom", "%"))->setValue(initialBloom * 100.f);

        dp4(configParam(Params::ModCableOpacity, 0, 100, 25, "Cable opacity amount", "%"));
        dp4(configParam(Params::ModCableTension, 0, 100, 25, "Cable tension amount", "%"));
        dp4(configParam(Params::ModBright, 0, 100, 25, "Bright amount", "%"));
        dp4(configParam(Params::ModBloom, 0, 100, 25, "Bloom amount", "%"));

        getParam(Params::ModCableOpacity).setValue(0);
        getParam(Params::ModCableTension).setValue(0);
        getParam(Params::ModBright).setValue(0);
        getParam(Params::ModBloom).setValue(0);

        configInput(InCableOpacity, "Cable opacity");
        configInput(InCableTension, "Cable tension");
        configInput(InBright, "Bright");
        configInput(InBloom, "Bloom");
    }

    ~Rui() {
        restore_initial_values(false);
    }

    // void paramsFromJson(json_t* root) override {
    //     if (running) {
    //         Base::paramsFromJson(root);
    //     }
    // }

    json_t* dataToJson() override {
        json_t* root = Base::dataToJson();
        set_json(root, "stopped", stopped);
        return root;
    }

    void dataFromJson(json_t* root) override {
        stopped = get_json_bool(root, "stopped", false);
    }

    const int PARAM_INTERVAL = 64;

    void restore_initial_values(bool params = true)
    {
        ::rack::settings::cableOpacity = initialCableOpacity;
        if (params) getParam(Params::CableOpacity).setValue(::rack::settings::cableOpacity);

        ::rack::settings::cableTension = initialCableTension;
        if (params) getParam(Params::CableTension).setValue(::rack::settings::cableTension);

        ::rack::settings::rackBrightness = initialBright;
        if (params) getParam(Params::Bright).setValue(::rack::settings::rackBrightness);

        ::rack::settings::haloBrightness = initialBloom;
        if (params) getParam(Params::Bloom).setValue(::rack::settings::haloBrightness);
        if (params) {
            getParam(Params::ModCableOpacity).setValue(0);
            getParam(Params::ModCableTension).setValue(0);
            getParam(Params::ModBright).setValue(0);
            getParam(Params::ModBloom).setValue(0);
        }
    }

    void process_param(int param_id, int mod_id, int input_id, float &setting)
    {
        auto input = getInput(input_id);
        auto param = getParam(param_id);
        auto trim  = getParam(mod_id);
        float v = param.getValue() * .01f;
        if (input.isConnected()) {
            float mod = trim.getValue() * .01f;
            v = clamp(v + (input.getVoltage() * .1f) * mod, 0.f, 1.f);
        }
        setting = v;
    }

    void processParams()
    {
        process_param(Params::CableOpacity, Params::ModCableOpacity, Inputs::InCableOpacity, ::rack::settings::cableOpacity);
        process_param(Params::CableTension, Params::ModCableTension, Inputs::InCableTension, ::rack::settings::cableTension);
        process_param(Params::Bright, Params::ModBright, Inputs::InBright, ::rack::settings::rackBrightness);
        process_param(Params::Bloom, Params::ModBloom, Inputs::InBloom, ::rack::settings::haloBrightness);
    }

    void process(const ProcessArgs& args) override
    {
        running = true;
        if (!single || stopped) return;
        if (0 == ((args.frame + getId()) % PARAM_INTERVAL)) {
            processParams();
        }
    }
};

struct RuiSvg
{
    static std::string background(Theme theme)
    {
        const char * asset = "res/Rui.svg";
        switch (theme) {
            case Theme::Dark: asset = "res/Rui-dark.svg"; break;
            case Theme::HighContrast: asset = "res/Rui-hc.svg"; break;
            case Theme::Unset:
            case Theme::Light:
            default: break;
        }
        return asset::plugin(pluginInstance, asset);
    }
};

struct RuiUi : ModuleWidget, IThemeChange
{
    using Base = ModuleWidget;
    Rui* my_module;
    ThemeBase* theme_holder{nullptr};
#ifdef HOT_SVG
    std::map<const char *, Widget*> positioned_widgets;
#endif
    bool single{true};
    PlayButton* play_button{nullptr};

    virtual ~RuiUi()
    {
        if (theme_holder && !module) {
            delete theme_holder;
        }
    }

    RuiUi(Rui* module) : my_module(module)
    {
        setModule(module);
        theme_holder = module ? module : new ThemeBase();
        theme_holder->setNotify(this);
        applyTheme(GetPreferredTheme(theme_holder));
    }

#ifdef HOT_SVG
#define HOT_POSITION(name,widget) positioned_widgets[name] = widget
#else
#define HOT_POSITION(name,widget) void
#endif

    void show_fluff(bool visible)
    {
        if (my_module) my_module->fluff = visible;
        auto svg = window::Svg::load(RuiSvg::background(Theme::Light));
        if (visible) {
            svg_query::showElements(svg, "fluff-");
        } else {
            svg_query::hideElements(svg, "fluff-");
        }
        svg = window::Svg::load(RuiSvg::background(Theme::Dark));
        if (visible) {
            svg_query::showElements(svg, "fluff-");
        } else {
            svg_query::hideElements(svg, "fluff-");
        }
        svg = window::Svg::load(RuiSvg::background(Theme::HighContrast));
        if (visible) {
            svg_query::showElements(svg, "fluff-");
        } else {
            svg_query::hideElements(svg, "fluff-");
        }
        onDirty(DirtyEvent{});
    }

    void makeUi(Theme theme)
    {
        assert(children.empty());
        setPanel(createSvgThemePanel<RuiSvg>(theme));
        single = is_singleton(my_module, this);
        if (my_module) my_module->single = single;
        if (!single) return;

        RoundBlackKnob* knob;
        Trimpot* pot;
        ThemedPJ301MPort* port;

        auto layout = window::Svg::load(RuiSvg::background(Theme::Light));
        std::map<std::string, ::math::Rect> bounds;
        svg_query::boundsIndex(layout, "k:", bounds, true);

        knob = createParamCentered<RoundBlackKnob>(bounds["k:cableop"].getCenter(), my_module, Rui::Params::CableOpacity);
        HOT_POSITION("k:cableop", knob);
        pot = createParamCentered<Trimpot>(bounds["k:trim-cableop"].getCenter(), my_module, Rui::Params::ModCableOpacity);
        HOT_POSITION("k:trim-cableop", pot);
        port = createInputCentered<ThemedPJ301MPort>(bounds["k:port-cableop"].getCenter(), my_module, Rui::Inputs::InCableOpacity);
        HOT_POSITION("k:port-cableop", port);
        addChild(knob);
        addChild(pot);
        addChild(port);

        knob = createParamCentered<RoundBlackKnob>(bounds["k:cableten"].getCenter(), my_module, Rui::Params::CableTension);
        HOT_POSITION("k:cableten", knob);
        pot = createParamCentered<Trimpot>(bounds["k:trim-cableten"].getCenter(), my_module, Rui::Params::ModCableTension);
        HOT_POSITION("k:trim-cableten", pot);
        port = createInputCentered<ThemedPJ301MPort>(bounds["k:port-cableten"].getCenter(), my_module, Rui::Inputs::InCableTension);
        HOT_POSITION("k:port-cableten", port);
        addChild(knob);
        addChild(pot);
        addChild(port);

        knob = createParamCentered<RoundBlackKnob>(bounds["k:bright"].getCenter(), my_module, Rui::Params::Bright);
        HOT_POSITION("k:bright", knob);
        pot = createParamCentered<Trimpot>(bounds["k:trim-bright"].getCenter(), my_module, Rui::Params::ModBright);
        HOT_POSITION("k:trim-bright", pot);
        port = createInputCentered<ThemedPJ301MPort>(bounds["k:port-bright"].getCenter(), my_module, Rui::Inputs::InBright);
        HOT_POSITION("k:port-bright", port);
        addChild(knob);
        addChild(pot);
        addChild(port);

        knob = createParamCentered<RoundBlackKnob>(bounds["k:bloom"].getCenter(), my_module, Rui::Params::Bloom);
        HOT_POSITION("k:bloom", knob);
        pot = createParamCentered<Trimpot>(bounds["k:trim-bloom"].getCenter(), my_module, Rui::Params::ModBloom);
        HOT_POSITION("k:trim-bloom", pot);
        port = createInputCentered<ThemedPJ301MPort>(bounds["k:port-bloom"].getCenter(), my_module, Rui::Inputs::InBloom);
        HOT_POSITION("k:port-bloom", port);
        addChild(knob);
        addChild(pot);
        addChild(port);

        play_button = createThemeWidgetCentered<PlayButton>(theme, bounds["k:pause"].getCenter());
        play_button->set_sticky(true);
        play_button->latched = my_module->stopped;
        play_button->describe(my_module->stopped ? "Play (F2)" : "Pause (F2)");
        HOT_POSITION("k:pause", play_button);
        if (module) {
            play_button->setHandler([this](bool ctrl, bool shift) {
                my_module->stopped = !my_module->stopped;
                play_button->describe(my_module->stopped ? "Play (F2)" : "Pause (F2)");
            });
        }
        addChild(play_button);
    }

    void toggle_play_pause() {
        if (!my_module || !play_button) return;
        ActionEvent e;
        e.context = new EventContext;
        play_button->onAction(e);
    }

    void applyTheme(Theme new_theme) {
        if (children.empty()) {
            makeUi(new_theme);
        } else {
            if (Theme::Light == new_theme) {
                auto layout = window::Svg::load(RuiSvg::background(Theme::Light));
                svg_query::hideElements(layout, "k:");
            }
            SetChildrenTheme(this, new_theme);
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

    void step() override {
        Base::step();
        theme_holder->pollRackDarkChanged();
    }

    void onHoverKey(const HoverKeyEvent& e) override
    {
        auto mods = e.mods & RACK_MOD_MASK;
        switch (e.key) {

        case GLFW_KEY_F2: {
            if (e.action == GLFW_PRESS && (0 == mods)) {
                toggle_play_pause();
            }
        } break;
#ifdef HOT_SVG
        case GLFW_KEY_F5: {
            if (e.action == GLFW_PRESS && (0 == mods)) {
                e.consume(this);
                auto panel = dynamic_cast<SvgThemePanel<RuiSvg>*>(getPanel());
                if (panel) {
                    auto layout = window::Svg::load(RuiSvg::background(Theme::Light));
                    std::map<std::string, ::math::Rect> bounds;
                    svg_query::boundsIndex(layout, "k:", bounds, true);
                    for (auto kv: positioned_widgets) {
                        kv.second->box.pos = bounds[kv.first].getCenter();
                        Center(kv.second);
                    }
                    panel->updatePanel(theme_holder->getTheme());
                    if (my_module) show_fluff(my_module->fluff);
                }
            }
        } break;
#endif
        }
        Base::onHoverKey(e);
    }
    void draw(const DrawArgs& args) override {
        Base::draw(args);
        if (my_module && !single) {
            draw_disabled_panel(this, GetPreferredTheme(theme_holder), args, 20, 20);
        }
    }

    void appendContextMenu(Menu* menu) override {
        if (!module) return;

        menu->addChild(createMenuLabel<HamburgerTitle>("Rui"));
        if (my_module) {
            menu->addChild(createMenuItem("Reset", "", [=](){
                my_module->reset();
            }));
            menu->addChild(createCheckMenuItem("Fluff", "",
                [=](){ return my_module->fluff; },
                [=](){ show_fluff(!my_module->fluff); }
            ));
        }
        AddThemeMenu(menu, theme_holder, false, false);
    }

};
}

Model *modelRui = createModel<pachde::Rui, pachde::RuiUi>("pachde-rui");
