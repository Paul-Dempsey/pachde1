#include "Skiff.hpp"
#include "skiff-help.hpp"
#include "services/open-file.hpp"
#include "services/rack-help.hpp"
#include "services/svg-theme-load.hpp"
#include "widgets/components.hpp"
#include "widgets/screws.hpp"
#include "widgets/symbol.hpp"

namespace pachde {

struct InfoWidget : OpaqueWidget {
    using Base = OpaqueWidget;

    QuestionSymbol* question{nullptr};
    TipWidget* tipper{nullptr};

    InfoWidget() {
        question = new QuestionSymbol();
        tipper = new TipWidget();
        addChild(question);
        addChild(tipper);
    }

    void describe(const std::string & tip) { tipper->describe(tip); }

    void loadSvg(ILoadSvg* loader) {
        question->loadSvg(loader);
        box.size = question->box.size;
        tipper->box.size = box.size;
    }

    void applyTheme(std::shared_ptr<SvgTheme> theme) {
        question->applyTheme(theme);
    }

    void update(std::shared_ptr<SvgTheme> theme) {
        question->update(theme);
        box.size = question->box.size;
        tipper->box.size = box.size;
    }

};

OptionMenuEntry* make_rail_item(SkiffUi *ui, bool other, const std::string& current, const char *item) {
    auto entry = new OptionMenuEntry(createMenuItem(item, "", [ui, item](){ ui->set_alt_rail(item); }));
    entry->selected = !other && (0 == current.compare(item));
    return entry;
}

OptionMenuEntry* make_rail_theme_item(SkiffUi *ui, const char * name, RailThemeSetting setting) {
    auto entry = new OptionMenuEntry(createMenuItem(name, "", [ui, setting](){ ui->set_rail_theme(setting); }));
    entry->selected = (ui->my_module->rail_theme == setting);
    return entry;
}

void RailMenu::appendContextMenu(ui::Menu* menu)
{
    if (!ui->module) return;
    auto rail_name = alt_rail_name();
    bool custom = !known_rail(rail_name);

    menu->addChild(createMenuLabel<HamburgerTitle>("Alternate rails"));
    menu->addChild(make_rail_item(ui, custom, rail_name, "Rack"));
    menu->addChild(new MenuSeparator);
    menu->addChild(make_rail_item(ui, custom, rail_name, "Plain"));
    menu->addChild(make_rail_item(ui, custom, rail_name, "Simple"));
    menu->addChild(make_rail_item(ui, custom, rail_name, "No-hole"));
    menu->addChild(make_rail_item(ui, custom, rail_name, "Blank"));
    menu->addChild(make_rail_item(ui, custom, rail_name, "Gradient"));
    menu->addChild(make_rail_item(ui, custom, rail_name, "Pinstripe"));
    menu->addChild(make_rail_item(ui, custom, rail_name, "Sine"));
//    menu->addChild(createMenuLabel<FancyLabel>(rail_name));
    auto option = new OptionMenuEntry(createMenuItem("Custom rail SVG", "", [=](){ ui->pend_custom_rail(); }));
    option->selected = custom;
    menu->addChild(option);

    menu->addChild(createMenuLabel<HamburgerTitle>("Rail Theme"));
    menu->addChild(make_rail_theme_item(ui, "None", RailThemeSetting::None));
    menu->addChild(make_rail_theme_item(ui, "Follow Rack UI theme", RailThemeSetting::FollowRackUi));
    menu->addChild(make_rail_theme_item(ui, "Follow Rack prefer dark panels", RailThemeSetting::FollowRackPreferDark));
    menu->addChild(make_rail_theme_item(ui, "Light", RailThemeSetting::Light));
    menu->addChild(make_rail_theme_item(ui, "Dark", RailThemeSetting::Dark));
    menu->addChild(make_rail_theme_item(ui, "High Contrast", RailThemeSetting::HighContrast));
}

struct SkiffSvg {
    static std::string background() { return asset::plugin(pluginInstance, "res/Skiff.svg"); }
};

SkiffUi::SkiffUi(Skiff* module) : my_module(module) {
    setModule(module);
    if (my_module) {
        my_module->ui = this;
        my_module->other_skiff = !is_singleton(my_module);
    }
    theme_holder = my_module ? my_module : new ThemeBase();
    theme_holder->setNotify(this);
    load_rail_themes();
    auto svg_theme = getThemeCache().getTheme(ThemeName(theme_holder->getTheme()));

    auto panel = createSvgThemePanel<SkiffSvg>(&my_svgs, nullptr);
    auto layout = panel->svg;
    setPanel(panel);

    if (my_module && my_module->other_skiff) return;

    ::svg_query::BoundsIndex bounds;
    svg_query::addBounds(layout, "k:", bounds, true);

    ham = Center(createWidget<RailMenu>(bounds["k:rail-menu"].getCenter()));
    HOT_POSITION("k:rail-menu", HotPosKind::Center, ham);
    ham->setUi(this);
    ham->describe("Alternate Rails");
    ham->applyTheme(svg_theme);
    addChild(ham);

    addChild(derail_button = makeTextButton(bounds,"k:unrail-btn", true, "", "Toggle visible rails", svg_theme,
        [this](bool ctrl, bool shift) { if(!my_module) return; derail(!my_module->derailed); }));

    addChild(nopanel_button = makeTextButton(bounds, "k:no-panel", true, "", "Toggle visibile panels", svg_theme,
        [this](bool ctrl, bool shift){ if(!my_module) return; no_panels(!my_module->depaneled); }));

    addChild(calm_button = makeTextButton(bounds, "k:calm-btn", true, "", "Toggle calm knobs and ports", svg_theme,
        [this](bool ctrl, bool shift) { if(!my_module) return; set_calm_rack(!my_module->calm); }));

    addChild(unscrew_button = makeTextButton(bounds, "k:screw-btn", true, "", "Toggle visible screws", svg_theme,
        [this](bool ctrl, bool shift) { if(!my_module) return; set_unscrewed(!my_module->unscrewed); }));

    addChild(nojack_button = makeTextButton(bounds, "k:nojack-btn", true, "", "Toggle visible unused jacks", svg_theme,
        [this](bool ctrl, bool shift) { if(!my_module) return; set_nojacks(!my_module->nojacks); }));

    addChild(dark_ages_button = makeTextButton(bounds, "k:darkages-btn", true, "", "Toggle visible lights", svg_theme,
        [this](bool ctrl, bool shift) { if(!my_module) return; set_dark_ages(!my_module->dark_ages); }));

    addChild(pack_button = makeTextButton(bounds, "k:pack-btn", false, "", "Pack modules (F7)", svg_theme,
        [](bool ctrl, bool shift) { pack_modules(); }));

    auto qmark = createWidget<InfoWidget>(bounds["k:q-mark"].getCenter());
    HOT_POSITION("k:q-mark", HotPosKind::Center, qmark);
    qmark->loadSvg(&my_svgs);
    qmark->describe("Tip:\nF6 = Zoom selected modules\nF7 = Pack (selected) modules");
    addChild(Center(qmark));

    auto button = Center(createThemeSvgButton<SmallActionButton>(&my_svgs, bounds["k:restore"].getCenter()));
    HOT_POSITION("k:restore", HotPosKind::Center, button);
    if (module) {
        button->describe("Restore normal Rack");
        button->set_handler([this](bool, bool) { restore_rack(); });
    }
    addChild(button);

    shouting_buttons(my_module ? my_module->shouting : true);
    my_svgs.changeTheme(svg_theme);
    from_module();
}

TextButton* SkiffUi::makeTextButton (
    std::map<std::string,::math::Rect>& bounds,
    const char* key,
    bool sticky,
    const char* title,
    const char* tip,
    std::shared_ptr<svg_theme::SvgTheme> svg_theme,
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

void SkiffUi::shouting_buttons(bool shouting) {
    if (shouting) {
        derail_button->set_text("DERAIL");
        nopanel_button->set_text("NOPANELS");
        calm_button->set_text("CALM");
        unscrew_button->set_text("UNSCREW");
        nojack_button->set_text("NOJACK");
        dark_ages_button->set_text("DARKNESS");
        pack_button->set_text("PACK'EM");
    } else {
        derail_button->set_text("deRail");
        nopanel_button->set_text("noPanels");
        calm_button->set_text("Calm");
        unscrew_button->set_text("unScrew");
        nojack_button->set_text("noJack");
        dark_ages_button->set_text("darkness");
        pack_button->set_text("Pack'em");
    }
}

void SkiffUi::onChangeTheme(ChangedItem item) {
    if (ChangedItem::Theme == item) {
        auto svg_theme = getThemeCache().getTheme(ThemeName(theme_holder->getTheme()));
        my_svgs.changeTheme(svg_theme);
        //if (ham) ham->applyTheme(svg_theme);
        applyChildrenTheme(this, svg_theme);
        sendDirty(this);
    }
}

void SkiffUi::sync_latch_state() {
    if (!my_module) return;

    derail_button->latched  = my_module->derailed;
    nopanel_button->latched = my_module->depaneled;
    calm_button->latched    = my_module->calm;
    unscrew_button->latched = my_module->unscrewed;
    nojack_button->latched  = my_module->nojacks;
    dark_ages_button->latched  = my_module->dark_ages;
}

void SkiffUi::restore_rack() {
    if (!my_module) return;
    my_module->set_defaults();
    from_module();
}

void SkiffUi::from_module() {
    if (!my_module) return;

    screw_visibility(APP->scene->rack, !my_module->unscrewed);
    port_visibility(APP->scene->rack, !my_module->nojacks);
    calm_rack(my_module->calm);
    panel_visibility(nullptr, !my_module->depaneled);
    light_visibility(APP->scene->rack, !my_module->dark_ages);
    set_alt_rail(my_module->rail);

    auto rail = APP->scene->rack->getFirstDescendantOfType<RailWidget>();
    if (rail) {
        rail->setVisible(!my_module->derailed);
    }
}

void SkiffUi::no_panels(bool depanel) {
    if (!my_module) return;

    my_module->depaneled = depanel;
    panel_visibility(nullptr, !depanel);
    APP->history->push(new BinaryAction(my_module->getId(), BinaryActionId::Depanel, depanel));
}

void SkiffUi::set_unscrewed(bool unscrewed) {
    if (!my_module) return;
    my_module->unscrewed = unscrewed;
    screw_visibility(APP->scene->rack, !unscrewed);
    APP->history->push(new BinaryAction(my_module->getId(), BinaryActionId::Unscrew, unscrewed));
}

void SkiffUi::derail(bool derail) {
    if (!my_module) return;

    my_module->derailed = derail;
    set_rail_visible(!derail);
    APP->history->push(new BinaryAction(my_module->getId(), BinaryActionId::Derail, derail));
}

void SkiffUi::set_nojacks(bool nojacks) {
    if (!my_module) return;
    my_module->nojacks = nojacks;
    port_visibility(APP->scene->rack, !nojacks);
    APP->history->push(new BinaryAction(my_module->getId(), BinaryActionId::Nojack, nojacks));
}

void SkiffUi::set_dark_ages(bool dark)
{
    if (!my_module) return;
    my_module->dark_ages = dark;
    light_visibility(APP->scene->rack, !dark);
    APP->history->push(new BinaryAction(my_module->getId(), BinaryActionId::Darkness, dark));
}

void SkiffUi::set_calm_rack(bool calm) {
    if (!my_module) return;
    my_module->calm = calm;
    calm_rack(calm);
    APP->history->push(new BinaryAction(my_module->getId(), BinaryActionId::Calm, calm));
}

void SkiffUi::recover_rack_rail() {
    auto rail_file = get_rack_rail_filename();
    auto railSvg = window::Svg::load(rail_file);
    railSvg->loadFile(rail_file);
    my_module->rail = "Rack";
}

std::shared_ptr<window::Svg> SkiffUi::set_rail_svg(RailWidget* rail, const std::string& filename) {
    std::string rail_file = get_rack_rail_filename();
    std::shared_ptr<window::Svg> railSvg = window::Svg::load(rail_file);
    if (railSvg) {
        try {
            railSvg->loadFile(filename);
            auto sw = rail->getFirstDescendantOfType<SvgWidget>();
            if (sw) {
                sw->setSvg(railSvg);
                auto fb = dynamic_cast<::rack::widget::FramebufferWidget*>(sw->getParent());
                if (fb) fb->setDirty(true);
            }
        } catch (Exception& e) {
            WARN("%s", e.msg.c_str());
        }
    }
    return railSvg;
}

void SkiffUi::set_alt_rail(const std::string& rail_name) {
    if (!my_module || my_module->other_skiff) return;
    auto rail = APP->scene->rack->getFirstDescendantOfType<RailWidget>();
    if (!rail) return;

    APP->history->push(new RailAction(my_module->getId(), my_module->rail, rail_name));

    auto ext = system::getExtension(rail_name);
    if (ext.size()) { // custom
        auto railSvg = set_rail_svg(rail, rail_name);
        if (railSvg->handle) {
            my_module->rail = rail_name;
            if (my_module->rail_theme != RailThemeSetting::None) {
                auto svg_theme = get_rail_theme(rail_theme_name(my_module->rail_theme));
                if (svg_theme) applySvgTheme(railSvg, svg_theme);
            }
        } else {
            railSvg->loadFile(get_rack_rail_filename());
            my_module->rail = "Rack";
        }
    } else { // builtin
        if (rail_name.empty() || (0 == rail_name.compare("Rack"))) {
            recover_rack_rail();
        } else {
            auto filename = asset::plugin(pluginInstance, format_string("res/rails/%s.svg", rail_name.c_str()));
            if (system::exists(filename)) {
                auto railSvg = set_rail_svg(rail, filename);
                if (railSvg && railSvg->handle) {
                    my_module->rail = rail_name;
                    if (my_module->rail_theme != RailThemeSetting::None) {
                        auto svg_theme = get_rail_theme(rail_theme_name(my_module->rail_theme));
                        if (svg_theme) applySvgTheme(railSvg, svg_theme);
                    }
                } else {
                    recover_rack_rail();
                }
            } else {
                assert(false);
                recover_rack_rail();
            }
        }
    }
    rail->onDirty(DirtyEvent{});
}

void SkiffUi::pend_custom_rail() {
    if (!my_module) return;
    request_custom_rail = true;
}

void SkiffUi::custom_rail() {
    if (!module) return;

    auto rail = APP->scene->rack->getFirstDescendantOfType<RailWidget>();
    if (!rail) return;

    std::string filename;
    std::string folder = my_module->rail_folder;
    auto ext = system::getExtension(my_module->rail);
    if (ext.size()) {
        filename = my_module->rail;
    }
    if (openFileDialog(folder, "SVG Files (.svg):svg;Any (*):", filename, filename)) {
        my_module->rail_folder = system::getDirectory(filename);
        set_alt_rail(filename);
    }
}

void SkiffUi::set_rail_theme(RailThemeSetting theme) {
    my_module->rail_theme = theme;
    set_alt_rail(my_module->rail);
}

void SkiffUi::onHoverKey(const HoverKeyEvent &e)
{
    if (!my_module) return;
    auto mods = e.mods & RACK_MOD_MASK;
    switch (e.key) {
#ifdef HOT_SVG
    case GLFW_KEY_F5: {
        if (e.action == GLFW_RELEASE && (0 == mods)) {
            e.consume(this);
            reloadThemeCache();
            my_svgs.reloadAll();
            load_rail_themes();
            set_rail_theme(my_module->rail_theme);
            onChangeTheme(ChangedItem::Theme);
            if (!my_module->other_skiff) {
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

void SkiffUi::step() {
    Base::step();
    if (!my_module) return;
    if (request_custom_rail) {
        request_custom_rail = false;
        custom_rail();
    }
   theme_holder->pollRackThemeChanged();
   sync_latch_state();
}

void SkiffUi::draw(const DrawArgs& args) {
    Base::draw(args);
    if (my_module && my_module->other_skiff) {
        draw_disabled_panel(this, GetPreferredTheme(theme_holder), args, 20.f, 20.f);
        return;
    }
    // if (my_cloak) {
    //     Circle(args.vg, 12, 192, 4, nvgHSLAf(50.f/360.f, 1.f, .8f, 1.f));
    // }
 }

void SkiffUi::appendContextMenu(Menu* menu) {
    if (!module) return;

    menu->addChild(createMenuLabel<HamburgerTitle>("#d Skiff"));
    menu->addChild(createCheckMenuItem("Shouting buttons", "",
        [=](){ return my_module->shouting; },
        [=](){
            my_module->shouting = !my_module->shouting;
            shouting_buttons(my_module->shouting);
        }
    ));
    menu->addChild(createMenuLabel<FancyLabel>("theme"));
    AddThemeMenu(menu, this, theme_holder, false, false, false);
}

// ---- BinaryAction (undo/redo) ----------------

BinaryAction::BinaryAction(int64_t module_id, BinaryActionId id, bool done_state) :
    id(id),
    done_state(done_state)
{
    moduleId = module_id;
    switch (id) {
    case BinaryActionId::Derail:   name = "deRail"; break;
    case BinaryActionId::Depanel:  name = "dePanel"; break;
    case BinaryActionId::Calm:     name = "Calm"; break;
    case BinaryActionId::Unscrew:  name = "unScrew"; break;
    case BinaryActionId::Nojack:   name = "noJacks"; break;
    case BinaryActionId::Darkness: name = "Darkness"; break;
    case BinaryActionId::Shouting: name = "shouting buttons"; break;
    }
}

void BinaryAction::set_desired_state(bool state) {
    auto skiff_module = dynamic_cast<Skiff*>(APP->engine->getModule(moduleId));

    switch (id) {
    case BinaryActionId::Derail:
        if (skiff_module) { skiff_module->derailed = state; }
        set_rail_visible(!state);
        break;
    case BinaryActionId::Depanel:
        if (skiff_module) { skiff_module->depaneled = state; }
        panel_visibility(nullptr, !state);
        break;
    case BinaryActionId::Calm:
        if (skiff_module) { skiff_module->calm = state; }
        calm_rack(state);
        break;
    case BinaryActionId::Unscrew:
        if (skiff_module) { skiff_module->unscrewed = state; }
        screw_visibility(APP->scene->rack, !state);
        break;
    case BinaryActionId::Nojack:
        if (skiff_module) { skiff_module->nojacks = state; }
        port_visibility(APP->scene->rack, !state);
        break;
    case BinaryActionId::Darkness:
        if (skiff_module) { skiff_module->dark_ages = state; }
        light_visibility(APP->scene->rack, !state);
        break;
    case BinaryActionId::Shouting:
        if (skiff_module->ui) {
            skiff_module->ui->shouting_buttons(state);
        }
        break;
    }
}

void RailAction::undo() {
    auto skiff_module = dynamic_cast<Skiff*>(APP->engine->getModule(moduleId));
    if (skiff_module && skiff_module->ui) {
        skiff_module->ui->set_alt_rail(prev_rail);
    }
}

void RailAction::redo() {
    auto skiff_module = dynamic_cast<Skiff*>(APP->engine->getModule(moduleId));
    if (skiff_module && skiff_module->ui) {
        skiff_module->ui->set_alt_rail(next_rail);
    }
}


}