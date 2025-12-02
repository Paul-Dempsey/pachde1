#include "PanelTone.hpp"
#include "IHaveColor.hpp"
#include "services/json-help.hpp"
namespace pachde {

PanelTone::PanelTone()
{
    config(Params::NUM_PARAMS, Inputs::NUM_INPUTS, Outputs::NUM_OUTPUTS, Lights::NUM_LIGHTS);
    configSwitch(P_OVERLAY_POSITION, 0.f, 1.f, 0.f, "Overlay position", {
        "Panel",
        "Widgets"
    });
    configSwitch(P_CONFIG_INPUT, 0.f, 1.f, 0.f, "Configure input", {
        "Trigger",
        "Continuous"
    });
    dp4(configParam(Params::P_FADE_TIME, 0.f, 10.f, 1.f, "Fade time", "sec"));
    configInput(Inputs::IN_FADE, "Go trigger");
}

void PanelTone::fade_in()
{
    data.on = true;
    fader.fade_in(getParam(P_FADE_TIME).getValue());
}

void PanelTone::fade_out()
{
    fader.fade_out(getParam(P_FADE_TIME).getValue());
    if (fader.fading == Fading::Zero) {
        data.on = false;
        if (ui) ui->onFadeOutComplete();
    }
}

const char * appliesToJsonValue(AppliesTo apply) {
    switch (apply) {
    case AppliesTo::All:              return "A";
    case AppliesTo::Selected:         return "S";
    case AppliesTo::Row:              return "RW";
    case AppliesTo::RowLeft:          return "RL";
    case AppliesTo::ContinuousLeft:   return "CL";
    case AppliesTo::RowRight:         return "RR";
    case AppliesTo::ContinuousRight:  return "CR";
    case AppliesTo::Left:             return "L";
    case AppliesTo::Right:            return "R";
    default: assert(false); break;
    }
    return "?";
}

AppliesTo parseAppliesTo(const char * text) {
    char first = *text;
    char second = text[1];
    switch (first) {
    default: assert(false); break;
    case 'A': return AppliesTo::All;
    case 'S': return AppliesTo::Selected;
    case 'R':
        switch (second) {
        case 'W': return AppliesTo::Row;
        case 'L': return AppliesTo::ContinuousLeft;
        case 'R': return AppliesTo::ContinuousRight;
        default: assert(false); break;
        }
        break;
    case 'C':
        switch (second) {
        case 'L': return AppliesTo::Left;
        case 'R': return AppliesTo::Right;
        default: assert(false); break;
        }
        break;
    case 'L': return AppliesTo::Left;
    }
    return AppliesTo::All;
}

InputKind PanelTone::get_input_kind(){
    if (getInput(IN_FADE).isConnected()) {
        return (getParam(P_CONFIG_INPUT).getValue() < .5)
            ? InputKind::Trigger
            : InputKind::Continuous;
    }
    return InputKind::None;
}

json_t *PanelTone::dataToJson()
{
    json_t *root = json_object();
    json_object_set_new(root, "Overlay", data.toJson());
    if (apply_to == AppliesTo::Selected) {
        if (module_ids.size() > 0) {
            auto jar = json_array();
            if (jar) {
                for (auto id: module_ids) {
                    json_t* j = json_integer(id);
                    if (j) {
                        json_array_append_new(jar, j);
                    }
                }
                json_object_set_new(root, "selected-modules", jar);
            }
            set_json(root, "apply-to", appliesToJsonValue(apply_to));
        } else {
            // revert to default;
            set_json(root, "apply-to", appliesToJsonValue(AppliesTo::All));
        }
    } else {
        set_json(root, "apply-to", appliesToJsonValue(apply_to));
    }
    set_json(root, "apply-to-me", apply_to_me);
    return root;
}

void PanelTone::dataFromJson(json_t *root)
{
    json_t *j = json_object_get(root, "Overlay");
    if (j) data.fromJson(j);
    apply_to_me = get_json_bool(root, "apply-to-me", apply_to_me);
    apply_to = parseAppliesTo(get_json_cstring(root, "apply-to", appliesToJsonValue(apply_to)));
    if (AppliesTo::Selected == apply_to) {
        auto jar = json_object_get(root, "selected-modules");
        if (jar) {
            json_t* jp;
            size_t index;
            json_array_foreach(jar, index, jp) {
                int64_t id = json_integer_value(jp);
                module_ids.push_back(id);
            }
        }
    }
}

bool PanelTone::fetch_expander_color(Expander expander)
{
    if (expander.module && (expander.module->model == modelCopper || expander.module->model == modelCopperMini)) {
        auto copper = dynamic_cast<IHaveColor*>(expander.module);
        if (copper && copper->colorExtenderEnabled()) {
            copper_color = copper->getColor(1);
            return true;
        }
    }
    return false;
}

void PanelTone::process(const ProcessArgs &args)
{
    Base::process(args);

    if (coppertone && data.on) {
        if (fetch_expander_color(getRightExpander())) {
            copper_connected = true;
        } else if (fetch_expander_color(getLeftExpander())) {
            copper_connected = true;
        } else {
            copper_connected = false;
        }
    }

    if (ui && (0 == ((getId() + args.frame) % 90))) {
        OverlayPosition pos = (getParam(P_OVERLAY_POSITION).getValue() > .5f)
            ? OverlayPosition::OnTop
            : OverlayPosition::OnPanel;
        if (pos != data.position) {
            data.position = pos;
            ui->set_overlay_position(pos);
        }

        float v = getParam(P_CONFIG_INPUT).getValue();
        if (last_input_config != v) {
            last_input_config = v;
            getInputInfo(IN_FADE)->name = v < .5f ? "Go trigger" : "Fade amount";
        }
    }

    if (InputKind::Trigger == get_input_kind()) {
        float v = getInput(IN_FADE).getVoltage();
        if (fade_trigger.process(v, 0.1f, 5.f)) {
            if (data.on) {
                fade_out();
            } else {
                if (ui) {
                    ui->onStartFadeIn();
                } else {
                    fade_in();
                }
            }
        }
    }

    switch (fader.fading) {
    case Fading::In:
        fader.step_fade();
        break;

    case Fading::Out:
        fader.step_fade();
        if (Fading::Zero == fader.fading) {
            data.on = false;
            if (ui) ui->onFadeOutComplete();
        }
        break;

    default:
        break;
    }
    getLight(L_ON).setBrightness(fader.fade);
}

// ---- UI ------------------------------------------------
void safeDeparentOverlay(PanelOverlay* overlay) {
    auto parent = overlay->getParent();
    if (parent) {
        if (parent->children.cend() != std::find(parent->children.cbegin(), parent->children.cend(), overlay)) {
            parent->removeChild(overlay);
        }
    }
}

struct PanelToneMenu : Hamburger
{
    PanelToneUi* ui{nullptr};
    void setUi(PanelToneUi* w) { ui = w; }
    void appendContextMenu(ui::Menu* menu) override;
};

struct PanelToneSvg {
    static std::string background() { return asset::plugin(pluginInstance, "res/PanelTone.svg"); }
};

PanelToneUi::PanelToneUi(PanelTone* module) : my_module(module)
{
    setModule(module);
    if (my_module) {
        my_module->ui = this;
        theme_holder = my_module;
        data = &my_module->data;
    } else {
        theme_holder = new ThemeBase();
        data = new OverlayData;
    }
    theme_holder->setNotify(this);
    auto theme = theme_holder->getTheme();
    auto svg_theme = getThemeCache().getTheme(ThemeName(theme));
    auto panel = createSvgThemePanel<PanelToneSvg>(&my_svgs, nullptr);
    auto layout = panel->svg;
    setPanel(panel);

    BoundsIndex bounds = makeBounds(layout, "k:", true);

    auto panel_menu = createWidgetCentered<PanelToneMenu>(bounds["k:menu"].getCenter());
    HOT_POSITION("k:menu", HotPosKind::Center, panel_menu);
    panel_menu->setUi(this);
    panel_menu->applyTheme(svg_theme);
    addChild(panel_menu);

    pos_switch = createParam<widgetry::Switch>(Vec(), my_module, PanelTone::P_OVERLAY_POSITION);
    HOT_POSITION("k:pos-switch", HotPosKind::Box, pos_switch);
    pos_switch->box = bounds["k:pos-switch"];
    pos_switch->applyTheme(svg_theme);
    addChild(pos_switch);

    {
        auto palette = Center(createThemeSvgButton<Palette1ActionButton>(&my_svgs, bounds["k:pick-panel"].getCenter()));
        HOT_POSITION("k:pick-panel", HotPosKind::Center, palette);
        palette->describe("Panel overlay color");
        palette->set_handler([=](bool,bool) {
            auto picker = new ColorPickerMenu();
            picker->set_color(data->color);
            picker->set_on_new_color([=](PackedColor color) { set_overlay_color(color); });
            auto menu = createMenu();
            menu->addChild(picker);
        });
        addChild(palette);
    }
    {
        auto knob = Center(createThemeSvgParam<TinyKnob>(&my_svgs, bounds["k:fade"].getCenter(), my_module, PanelTone::P_FADE_TIME));
        HOT_POSITION("k:fade", HotPosKind::Center, knob);
        knob->step_increment_by = .1f;
        addChild(knob);
    }
    {
        auto light = createLightCentered<SmallLight<BlueLight>>(bounds["k:on-light"].getCenter(), my_module, PanelTone::L_ON);
        HOT_POSITION("k:on-light", HotPosKind::Center, light);
        addChild(light);
    }

    on_button = Center(createThemeSvgButton<SmallActionButton>(&my_svgs, bounds["k:go-btn"].getCenter()));
    HOT_POSITION("k:go-btn", HotPosKind::Center, on_button);
    on_button->describe("Toggle toned panels (F2)");
    on_button->set_sticky(true);
    on_button->latched = data->on;
    on_button->set_handler([=](bool,bool){
        toggle_panels();
    });
    addChild(on_button);

    {
        auto port = createInputCentered<ThemedPJ301MPort>(bounds["k:fade-trigger"].getCenter(), my_module, PanelTone::IN_FADE);
        HOT_POSITION("k:fade-trigger", HotPosKind::Center, port);
        addChild(port);
    }

    in_config_switch = createParam<widgetry::Switch>(Vec(), my_module, PanelTone::P_CONFIG_INPUT);
    HOT_POSITION("k:cv-switch", HotPosKind::Box, in_config_switch);
    in_config_switch->box = bounds["k:cv-switch"];
    in_config_switch->applyTheme(svg_theme);
    addChild(in_config_switch);

    my_svgs.changeTheme(svg_theme);
}

void PanelToneUi::onDestroyPanelOverlay(PanelOverlay *removed)
{
    if (in_destroy) return;
    auto it = std::find(overlays.begin(), overlays.end(), removed);
    if (it != overlays.end()) {
        overlays.erase(it);
    }
}

void PanelToneUi::onFadeOutComplete() {
    remove_pending = true;
}

void PanelToneUi::onStartFadeIn() {
    add_pending = true;
}

void PanelToneUi::fade_in_overlays() {
    if (my_module) my_module->fade_in();
}

void PanelToneUi::fade_out_overlays() {
    if (my_module) my_module->fade_out();
}

void PanelToneUi::fade_overlays() {
    if (!my_module) return;
    //if (!data->on) return;
    float fade = my_module->fader.fade;
    switch (my_module->fader.fading) {
    default:
    case Fading::Zero: break;

    case Fading::In:
    case Fading::Out:
        for (auto overlay: overlays) {
            if (!is_my_overlay(overlay)) continue;
            overlay->fade = fade;
        }
        break;

    case Fading::Ready:
        if (InputKind::Continuous == my_module->get_input_kind()) {
            fade = my_module->getInput(PanelTone::IN_FADE).getVoltage() * .1f;
            for (auto overlay: overlays) {
                if (!is_my_overlay(overlay)) continue;
                overlay->fade = fade;
            }
        }
        break;
    }
}

void PanelToneUi::set_overlay_position(OverlayPosition pos)
{
    if (pos == data->position) return;
    data->position = pos;
    for (auto overlay: overlays) {
        if (!is_my_overlay(overlay)) continue;
        auto mw = overlay->getParent();
        if (mw) {
            mw->removeChild(overlay);
            add_layered_child(mw, overlay, pos);
        }
    }
}

void PanelToneUi::set_overlay_color(PackedColor color)
{
    data->color = color;
    broadcast_overlay_color(color);
}

void PanelToneUi::broadcast_overlay_color(PackedColor color)
{
    if (color != last_broadcast_color) {
        last_broadcast_color = color;
        for (auto overlay: overlays) {
            if (!is_my_overlay(overlay)) continue;
            overlay->data.color = color;
        }
    }
}

void PanelToneUi::remove_overlays()
{
    in_destroy = true;
    for (auto overlay: overlays) {
        if (!is_my_overlay(overlay)) continue;
        safeDeparentOverlay(overlay);
        delete overlay;
    }
    in_destroy = false;
    overlays.clear();
}

std::vector<ModuleWidget *> PanelToneUi::get_applicable_module_widgets()
{
    std::vector<ModuleWidget*> module_widgets;
    if ((AppliesTo::Selected == my_module->apply_to) && !my_module->module_ids.empty()) {
        std::vector<int64_t>& ids{my_module->module_ids};
        for (auto mw: APP->scene->rack->getModules()) {
            int64_t id = mw->getModule()->getId();
            if (ids.cend() != std::find(ids.cbegin(), ids.cend(), id)) {
                module_widgets.push_back(mw);
            }
        }
    } else {
        module_widgets = getModuleWidgets(this, my_module->apply_to);
        if (AppliesTo::Selected == my_module->apply_to) {
            for (auto mw: module_widgets) {
                my_module->module_ids.push_back(mw->getModule()->getId());
            }
        } else {
            my_module->module_ids.clear();
        }
    }
    return module_widgets;
}

void PanelToneUi::add_overlays(const std::vector<ModuleWidget*>& module_widgets)
{
    if (!my_module) return;
    for (auto mw: module_widgets) {
        if (!my_module->apply_to_me && (mw == this)) continue;
        auto overlay = mw->getFirstDescendantOfType<PanelOverlay>();
        if (overlay) {
            safeDeparentOverlay(overlay);
            if (overlay->host) { overlay->host->onDestroyPanelOverlay(overlay); }
            overlay->data.init(data);
            overlay->host = this;
        } else {
            overlay = new PanelOverlay(this);
        }
        add_layered_child(mw, overlay, data->position);
        auto it = std::find(overlays.begin(), overlays.end(), overlay);
        if (it == overlays.end()) {
            overlays.push_back(overlay);
        }
        dirtyWidget(mw);
    }
}

void PanelToneUi::toggle_panels()
{
    if (!my_module) return;

    if (!data->on) {
        data->on = true;
        add_overlays(get_applicable_module_widgets());
        fade_in_overlays();
    } else {
        fade_out_overlays();
    }
}

void PanelToneUi::set_applies_to(AppliesTo apply)
{
    if (!my_module) return;
    my_module->apply_to = apply;
    remove_overlays();
    my_module->module_ids.clear();
    if (data->on) {
        add_overlays(get_applicable_module_widgets());
        fade_in_overlays();
    } else {
        get_applicable_module_widgets();
    }
}

void PanelToneUi::onChangeTheme(ChangedItem item)
{
    if (ChangedItem::Theme != item) return;
    auto theme = theme_holder->getTheme();
    auto svg_theme = getThemeCache().getTheme(ThemeName(theme));
    my_svgs.changeTheme(svg_theme);
    applyChildrenTheme(this, svg_theme); // any IThemed widgets
    sendDirty(this);
}

void PanelToneUi::onHoverKey(const HoverKeyEvent &e)
{
    if (!my_module) return;
    auto mods = e.mods & RACK_MOD_MASK;
    switch (e.key) {
        case GLFW_KEY_F2: {
            if (e.action == GLFW_PRESS && (0 == mods)) {
                toggle_panels();
            }
        } break;
    #ifdef HOT_SVG
    case GLFW_KEY_F5: {
        if (e.action == GLFW_RELEASE && (0 == mods)) {
            e.consume(this);
            reloadThemeCache();
            my_svgs.reloadAll();
            onChangeTheme(ChangedItem::Theme);
            auto panel = dynamic_cast<SvgThemePanel<PanelToneSvg>*>(getPanel());
            positionWidgets(pos_widgets, makeBounds(panel->svg, "k:", true));
            sendDirty(this);
        }
    } break;
    #endif
    }
    Base::onHoverKey(e);
}

void PanelToneUi::appendContextMenu(Menu *menu)
{
    if (!module) return;
    menu->addChild(createMenuLabel<HamburgerTitle>("#d PanelTone"));
    menu->addChild(createMenuLabel<FancyLabel>("theme"));
    AddThemeMenu(menu, this, theme_holder, false, false, false);
}

void PanelToneUi::step() {
    Base::step();
    if (!my_module) return;
    if (data->on != on_button->latched) {
        on_button->latched = data->on;
    }

    bool is_copper_toning = (my_module->coppertone && data->on && my_module->copper_connected);
    if (is_copper_toning) {
        auto co = toPacked(my_module->copper_color);
        broadcast_overlay_color(co);
        coppertoning = true;
    } else if (coppertoning) {
        coppertoning = false;
        broadcast_overlay_color(data->color);
    }
    fade_overlays();

    if (remove_pending) {
        remove_pending = false;
        remove_overlays();
    }
    if (add_pending) {
        add_pending = false;
        data->on = true;
        add_overlays(get_applicable_module_widgets());
        fade_in_overlays();
    }
}

void PanelToneUi::toggle_applies_to_me() {
    if (!my_module) return;
    my_module->apply_to_me = !my_module->apply_to_me;
    if (data->on) {
        auto overlay = getPanelOverlay(this);
        if (!is_my_overlay(overlay)) {
            overlay = nullptr;
        }
        if (overlay && !my_module->apply_to_me) {
            safeDeparentOverlay(overlay);
            delete overlay;
            return;
        }
        //  if here, check if applicable
        if (my_module->apply_to_me) {
            switch (my_module->apply_to) {
                case AppliesTo::All:
                case AppliesTo::Row:
                    if (overlay) return;
                    break;

                case AppliesTo::Selected: {
                    get_applicable_module_widgets();
                    std::vector<int64_t>& ids{my_module->module_ids};
                    bool applies_to_me = ids.cend() != std::find(ids.cbegin(), ids.cbegin(), my_module->getId());
                    if (overlay && !applies_to_me) {
                        safeDeparentOverlay(overlay);
                        delete overlay;
                        return;
                    }
                } break;

                default:
                    if (overlay) {
                        safeDeparentOverlay(overlay);
                        delete overlay;
                    }
                    return;
                    break;
            }
            // if here, then adding a new overlay
            if (overlay) return;
            auto overlay = new PanelOverlay(this);
            add_layered_child(this, overlay, data->position);
            overlays.push_back(overlay);
        }
    }
}
void PanelToneMenu::appendContextMenu(ui::Menu* menu) {
    if (!ui->module) return;
    menu->addChild(createMenuLabel<HamburgerTitle>("PanelTone"));
    menu->addChild(createCheckMenuItem("Use Copper color", "",
        [=](){ return ui->my_module->coppertone; },
        [=](){ ui->my_module->coppertone = !ui->my_module->coppertone; }
    ));
    menu->addChild(createCheckMenuItem("Tone this PanelTone", "",
        [=](){ return ui->my_module->apply_to_me; },
        [=](){ ui->toggle_applies_to_me(); }));

    menu->addChild(createMenuLabel<FancyLabel>("Apply To"));

    menu->addChild(createCheckMenuItem("All", "",
        [=](){ return AppliesTo::All == ui->my_module->apply_to; },
        [=](){ ui->set_applies_to(AppliesTo::All); }));
    menu->addChild(createCheckMenuItem("Selected modules", "",
        [=](){ return AppliesTo::Selected == ui->my_module->apply_to; },
        [=](){ ui->set_applies_to(AppliesTo::Selected); },
        !APP->scene->rack->hasSelection()
    ));
    menu->addChild(createCheckMenuItem("Modules in this row", "",
        [=](){ return AppliesTo::Row == ui->my_module->apply_to; },
        [=](){ ui->set_applies_to(AppliesTo::Row); }));
    menu->addChild(createCheckMenuItem("Modules to the left", "",
        [=](){ return AppliesTo::RowLeft == ui->my_module->apply_to; },
        [=](){ ui->set_applies_to(AppliesTo::RowLeft); }));
    menu->addChild(createCheckMenuItem("Continuous modules to the left", "",
        [=](){ return AppliesTo::ContinuousLeft == ui->my_module->apply_to; },
        [=](){ ui->set_applies_to(AppliesTo::ContinuousLeft); }));
    menu->addChild(createCheckMenuItem("Modules to the right", "",
        [=](){ return AppliesTo::RowRight == ui->my_module->apply_to; },
        [=](){ ui->set_applies_to(AppliesTo::RowRight); }));
    menu->addChild(createCheckMenuItem("Continuous modules to the right", "",
        [=](){ return AppliesTo::ContinuousRight == ui->my_module->apply_to; },
        [=](){ ui->set_applies_to(AppliesTo::ContinuousRight); }));
    menu->addChild(createCheckMenuItem("Module to the left", "",
        [=](){ return AppliesTo::Left == ui->my_module->apply_to; },
        [=](){ ui->set_applies_to(AppliesTo::Left); }));
    menu->addChild(createCheckMenuItem("Module to the right", "",
        [=](){ return AppliesTo::Right == ui->my_module->apply_to; },
        [=](){ ui->set_applies_to(AppliesTo::Right); }));
}

}

Model* modelPanelTone = createModel<pachde::PanelTone, pachde::PanelToneUi>("pachde-paneltone");
