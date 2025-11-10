#include "PanelTone.hpp"
#include "services/json-help.hpp"
namespace pachde {

PanelTone::PanelTone()
{
    config(Params::NUM_PARAMS, Inputs::NUM_INPUTS, Outputs::NUM_OUTPUTS, Lights::NUM_LIGHTS);
    configSwitch(P_OVERLAY_POSITION, 0.f, 1.f, 0.f, "Overlay position", {
        "Panel",
        "Widgets"
    });
    dp4(configParam(Params::P_FADE_TIME, 0.f, 10.f, 1.f, "Fade time", "sec"));
    configInput(Inputs::IN_FADE_TRIGGER, "Fade in/out trigger");
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

json_t *PanelTone::dataToJson()
{
    json_t *root = json_object();
    json_object_set_new(root, "Overlay", data.toJson());
    set_json(root, "apply-to", appliesToJsonValue(apply_to));
    set_json(root, "apply-to-me", apply_to_me);
    return root;
}

void PanelTone::dataFromJson(json_t *root)
{
    json_t *j = json_object_get(root, "Overlay");
    if (j) data.fromJson(j);
    apply_to = parseAppliesTo(get_json_cstring(root, "apply-to", appliesToJsonValue(apply_to)));
    apply_to_me = get_json_bool(root, "apply-to-me", apply_to_me);
}

void PanelTone::process(const ProcessArgs &args)
{
    Base::process(args);

    if (ui && (0 == ((getId() + args.frame) % 90))) {
        OverlayPosition pos = (getParam(P_OVERLAY_POSITION).getValue() > .5f) ? OverlayPosition::OnTop : OverlayPosition::OnPanel;
        if (pos != data.position) {
            ui->set_overlay_position(pos);
        }
    }
    if (getInput(IN_FADE_TRIGGER).isConnected()) {
        auto v = getInput(IN_FADE_TRIGGER).getVoltage();
        if (fade_trigger.process(v, 0.1f, 5.f)) {
            data.on = !data.on;
            if (data.on) {
                fader.fade_in(getParam(P_FADE_TIME).getValue());
            } else {
                fader.fade_out(getParam(P_FADE_TIME).getValue());
            }
        }
    }

    switch (fader.fading) {
    case Fading::Zero: getLight(L_ON).setBrightness(0.f); break;
    case Fading::In:
    case Fading::Out:
        if (0 == ((getId() + args.frame) % 30)) {
            fader.step_fade();
            if (Fading::Zero == fader.fading) {
                if (ui) ui->onFadeOutComplete();
            }
            getLight(L_ON).setBrightness(fader.fade);
        }
        break;
    case Fading::Ready: getLight(L_ON).setBrightness(1.f); break;
    }
}

// ---- UI ------------------------------------------------
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
    auto svg_theme = getThemeCache().getTheme(ThemeName(theme_holder->getTheme()));
    auto panel = createSvgThemePanel<PanelToneSvg>(&my_svgs, nullptr);
    auto layout = panel->svg;
    setPanel(panel);

    BoundsIndex bounds = makeBounds(layout, "k:", true);

    auto panel_menu = createWidgetCentered<PanelToneMenu>(bounds["k:menu"].getCenter());
    HOT_POSITION("k:menu", HotPosKind::Center, panel_menu);
    panel_menu->setUi(this);
    panel_menu->applyTheme(svg_theme);
    addChild(panel_menu);

    pos_switch = createThemeParam<widgetry::Switch>(theme_holder->getTheme(), Vec(), my_module, PanelTone::P_OVERLAY_POSITION);
    HOT_POSITION("k:pos-switch", HotPosKind::Box, pos_switch);
    pos_switch->box = bounds["k:pos-switch"];
    pos_switch->setTheme(theme_holder->getTheme());
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
    on_button->describe("Toggle toned panels");
    on_button->set_sticky(true);
    on_button->latched = data->on;
    on_button->set_handler([=](bool,bool){
        toggle_panels();
    });
    addChild(on_button);

    {
        auto port = createInputCentered<ThemedPJ301MPort>(bounds["k:fade-trigger"].getCenter(), my_module, PanelTone::IN_FADE_TRIGGER);
        HOT_POSITION("k:fade-trigger", HotPosKind::Center, port);
        addChild(port);
    }
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

void PanelToneUi::fade_in_overlays() {
    if (!my_module) return;
    double interval = my_module->getParam(PanelTone::P_FADE_TIME).getValue();
    for (auto overlay: overlays) {
        overlay->fade_in(interval);
    }
    my_module->fade_in();

}

void PanelToneUi::fade_out_overlays() {
    if (!my_module) return;
    double interval = my_module->getParam(PanelTone::P_FADE_TIME).getValue();
    for (auto overlay: overlays) {
        overlay->fade_out(interval);
    }
    my_module->fade_out();
}

void PanelToneUi::set_overlay_position(OverlayPosition pos)
{
    if (pos == data->position) return;
    data->position = pos;
    for (auto overlay: overlays) {
        auto mw = overlay->getParent();
        mw->removeChild(overlay);
        add_layered_child(mw, overlay, pos);
    }
}

void PanelToneUi::set_overlay_color(PackedColor color)
{
    data->color = color;
    for (auto overlay: overlays) {
        overlay->data.color = color;
    }
}

void PanelToneUi::remove_overlays()
{
    in_destroy = true;
    for (auto overlay: overlays) {
        auto parent = overlay->getParent();
        if (parent) {
            parent->removeChild(overlay);
            dirtyWidget(parent);
        }
        delete overlay;
    }
    in_destroy = false;
    overlays.clear();
}

void PanelToneUi::add_overlays(const std::vector<ModuleWidget*>& module_widgets)
{
    if (!my_module) return;
    for (auto mw: module_widgets) {
        if (!my_module->apply_to_me && (mw == this)) continue;
        auto overlay = mw->getFirstDescendantOfType<PanelOverlay>();
        if (overlay) {
            mw->removeChild(overlay);
            overlay->data.init(data);
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
    data->on = !data->on;
    if (data->on) {
        add_overlays(getModuleWidgets(this, my_module->apply_to));
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
    if (data->on) {
        add_overlays(getModuleWidgets(this, apply));
        fade_in_overlays();
    }
}

void PanelToneUi::onChangeTheme(ChangedItem item)
{
    if (ChangedItem::Theme != item) return;
    auto theme = theme_holder->getTheme();
    auto svg_theme = getThemeCache().getTheme(ThemeName(theme));
    my_svgs.changeTheme(svg_theme);
    applyChildrenTheme(this, svg_theme); // any IThemed widgets
    pos_switch->setTheme(theme);
    sendDirty(this);
}

void PanelToneUi::onHoverKey(const HoverKeyEvent &e)
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
    //menu->addChild(createMenuLabel<FancyLabel>("theme"));
    AddThemeMenu(menu, this, theme_holder, false, false, false);
}

void PanelToneUi::step() {
    Base::step();
    if (data->on != on_button->latched) {
        on_button->latched = data->on;
    }
    if (remove_pending) {
        remove_pending = false;
        remove_overlays();
    }
}

void PanelToneUi::toggle_applies_to_me() {
    if (!my_module) return;
    my_module->apply_to_me = !my_module->apply_to_me;
    if (data->on) {
        auto overlay = getPanelOverlay(this);
        if (my_module->apply_to_me) {
            switch (my_module->apply_to) {
                case AppliesTo::All:
                case AppliesTo::Row:
                    break;

                case AppliesTo::Selected: {
                    auto sel = APP->scene->rack->getSelected();
                    auto it = std::find(sel.begin(), sel.end(), this);
                    if (it == sel.end()) {
                        if (overlay) {
                            auto parent = overlay->getParent();
                            if (parent) parent->removeChild(overlay);
                            delete overlay;
                        }
                        return;
                    }
                } break;

                default:
                    if (overlay) {
                        auto parent = overlay->getParent();
                        if (parent) parent->removeChild(overlay);
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

        } else {
            if (overlay) delete overlay;
        }
    }
}
void PanelToneMenu::appendContextMenu(ui::Menu* menu) {
    if (!ui->module) return;
    menu->addChild(createMenuLabel<HamburgerTitle>("PanelTone"));
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
