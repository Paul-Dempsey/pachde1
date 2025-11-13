#include "Guide.hpp"
#include "services/json-help.hpp"
#include "services/open-file.hpp"
#include "widgets/switch.hpp"
#include "guide-preset.hpp"

namespace pachde {

const char * file_dialog_filter = "Guides (.json):json;Any (*):*";

std::string TempName(const std::string& suffix) {
    return format_string("(%d-%d).%s",
        random::get<uint16_t>(),
        random::get<uint32_t>(),
        suffix.empty() ? ".tmp" : suffix.c_str()
        );
}

Guide::Guide() {
    config(Params::NUM_PARAMS, Inputs::NUM_INPUTS, Outputs::NUM_OUTPUTS, Lights::NUM_LIGHTS);
    dp3(configParam(P_X, -1500.f, 1500.f, 0.f, "X", "px"));
    dp3(configParam(P_Y, -380, 380.f, 0.f, "Y", "px"));
    dp3(configParam(P_W, 0.f, 50.f, .75f, "Width", "px"));
    dp3(configParam(P_R, 0.f, 50.f, 0.f, "Repeat", "px"));
    configSwitch(P_ANGLE, 0.f, 1.f, 0.f, "Angle", { "Horizontal", "Vertical"});
    configSwitch(P_OVERLAY_POSITION, 0.f, 1.f, 0.f, "Overlay position", {
        "Panel",
        "Widgets"
    });
    //configLight(L_CONNECTED, "Connected");
}

json_t * Guide::dataToJson() {
    json_t* root = Base::dataToJson();
    set_json(root, "guides-folder", guide_folder);
    auto j = guide_data.toJson();
    if (j) json_object_set_new(root, "guide-data", j);
    return root;
}

void Guide::dataFromJson(json_t *root) {
    Base::dataFromJson(root);
    guide_folder = get_json_string(root, "guides-folder", "");
    guide_data.fromJson(json_object_get(root, "guide-data"));
}

void Guide::set_guide(const std::shared_ptr<GuideLine> guide)
{
    getParam(P_X).setValue(guide->origin.x);
    getParam(P_Y).setValue(guide->origin.y);
    getParam(P_ANGLE).setValue(guide->angle > 0.f ? 90.f : 0.f);
    getParam(P_W).setValue(guide->width);
    getParam(P_R).setValue(guide->repeat);
}

void Guide::onExpanderChange(const ExpanderChangeEvent &e)
{
    if (!ui || 0 != e.side) return;
    ui->onExpanderChange(getLeftExpander());
}

void Guide::process(const ProcessArgs &args)
{
    if (ui && (0 == ((getId() + args.frame) % 90))) {
        getLight(L_CONNECTED).setBrightness(ui->panel_guides ? 1.f : 0.f);

        OverlayPosition pos = (getParam(P_OVERLAY_POSITION).getValue() > .5f) ? OverlayPosition::OnTop : OverlayPosition::OnPanel;
        if (pos != guide_data.position) {
            ui->set_overlay_position(pos);
        }

        auto guide = ui->guideline;
        bool change{false};
        float v = getParam(P_X).getValue();
        if (v != guide->origin.x) { change = true; guide->origin.x = v; }

        v = getParam(P_Y).getValue();
        if (v != guide->origin.y) { change = true; guide->origin.y = v; }

        v = getParam(P_W).getValue();
        if (v != guide->width) { change = true; guide->width = v; }

        v = getParam(P_R).getValue();
        if (v != guide->repeat) { change = true; guide->repeat = v; }

        v = getParam(P_ANGLE).getValue();
        v = (v > 0) ? 90.f : 0.f;
        if (v != guide->angle) { change = true; guide->angle = v; }

        if (change) {
            sendDirty(ui->panel_guides);
        }
    }
}

// ---- UI ------------------------------------------------

struct TemplateMenu : Hamburger
{
    GuideUi* ui{nullptr};
    void set_ui(GuideUi* w) { ui = w; }
    void appendContextMenu(ui::Menu* menu) override {
        using namespace guides;
        if (!ui->module) return;
        menu->addChild(createMenuLabel<HamburgerTitle>("Guide Templates"));
        menu->addChild(createMenuItem("Open...", "", [=](){ ui->open_guides(); }));
        menu->addChild(createMenuItem("Save...", "", [=](){ ui->save_guides(); }));
        menu->addChild(createMenuLabel<FancyLabel>("factory"));
        menu->addChild(createMenuItem(TopMarginData::tooltip(), "", [=](){ ui->load_guide_file(TopMarginData::guide()); }));
        menu->addChild(createMenuItem(BottomMarginData::tooltip(), "", [=](){ ui->load_guide_file(BottomMarginData::guide()); }));
        menu->addChild(createMenuItem(LeftData::tooltip(), "", [=](){ ui->load_guide_file(LeftData::guide()); }));
        menu->addChild(createMenuItem(RightData::tooltip(), "", [=](){ ui->load_guide_file(RightData::guide()); }));
        menu->addChild(createMenuItem(HorizontalData::tooltip(), "", [=](){ ui->load_guide_file(HorizontalData::guide()); }));
        menu->addChild(createMenuItem(VerticalData::tooltip(), "", [=](){ ui->load_guide_file(VerticalData::guide()); }));
        menu->addChild(createMenuItem(GridData::tooltip(), "", [=](){ ui->load_guide_file(GridData::guide()); }));
        menu->addChild(createMenuLabel<FancyLabel>("goodies"));
        menu->addChild(createMenuItem("10px horizontal rules", "", [=](){ ui->load_guide_file(asset::plugin(pluginInstance, "res/guides/Horizontal 10px.json")); }));
        menu->addChild(createMenuItem("10px vertical rules", "", [=](){ ui->load_guide_file(asset::plugin(pluginInstance, "res/guides/Vertical 10px.json")); }));
        menu->addChild(createMenuItem("10px ruled grid", "", [=](){ ui->load_guide_file(asset::plugin(pluginInstance, "res/guides/Grid 10px.json")); }));
        menu->addChild(createMenuItem("1/2hp striped grid", "", [=](){ ui->load_guide_file(asset::plugin(pluginInstance, "res/guides/Plaid.json")); }));
    }
};

struct GuideSvg {
    static std::string background() { return asset::plugin(pluginInstance, "res/Guide.svg"); }
};

template <typename TData>
void addGuideButton(GuideUi* self, BoundsIndex& bounds) {
    const char * key = TData::key();
    auto button = createThemeSvgButton<TActionButton<TData>>(&self->my_svgs, bounds[key].getCenter());
#ifdef HOT_SVG
    addPosition(self->pos_widgets, key, HotPosKind::Center, button);
#endif
    button->set_handler([self](bool,bool){ self->load_guide_file(TData::guide()); });
    button->describe(TData::tooltip());
    self->addChild(Center(button));
}

GuideUi::GuideUi(Guide* module) : my_module(module)
{
    setModule(module);
    if (my_module) {
        my_module->ui = this;
        theme_holder = my_module;
        guide_data = &my_module->guide_data;
    } else {
        theme_holder = new ThemeBase();
        guide_data = new GuideData();
    }
    theme_holder->setNotify(this);
    guideline = std::make_shared<GuideLine>();

    auto svg_theme = getThemeCache().getTheme(ThemeName(theme_holder->getTheme()));

    auto panel = createSvgThemePanel<GuideSvg>(&my_svgs, nullptr);
    auto layout = panel->svg;
    setPanel(panel);

    TinyKnob* small_knob;
    BoundsIndex bounds = makeBounds(layout, "k:", true);
    Rect r;

    {
        auto light = createLightCentered<MediumLight<BlueLight>>(bounds["k:on-light"].getCenter(), my_module, Guide::L_CONNECTED);
        HOT_POSITION("k:on-light", HotPosKind::Center, light);
        addChild(light);
    }
    {
        auto palette = Center(createThemeSvgButton<Palette1ActionButton>(&my_svgs, bounds["k:pick-panel"].getCenter()));
        HOT_POSITION("k:pick-panel", HotPosKind::Center, palette);
        palette->describe("Panel overlay color");
        palette->set_handler([=](bool,bool) {
            auto picker = new ColorPickerMenu();
            picker->set_color(guide_data->co_overlay);
            picker->set_on_new_color([=](PackedColor color) { set_panel_overlay_color(color); });
            auto menu = createMenu();
            menu->addChild(picker);
        });
        addChild(palette);

        panel_swatch = new Swatch;
        HOT_POSITION("k:panel-swatch", HotPosKind::Box, panel_swatch);
        panel_swatch->box = bounds["k:panel-swatch"];
        panel_swatch->color = guide_data->co_overlay;
        addChild(panel_swatch);

        panel_solid = new SolidSwatch;
        HOT_POSITION("k:panel-solid", HotPosKind::Box, panel_solid);
        panel_solid->box = bounds["k:panel-solid"];
        panel_solid->color = guide_data->co_overlay;
        addChild(panel_solid);
    }

    addGuideButton<guides::TopMarginData>(this, bounds);
    addGuideButton<guides::BottomMarginData>(this, bounds);
    addGuideButton<guides::LeftData>(this, bounds);
    addGuideButton<guides::RightData>(this, bounds);
    addGuideButton<guides::HorizontalData>(this, bounds);
    addGuideButton<guides::VerticalData>(this, bounds);
    addGuideButton<guides::GridData>(this, bounds);

    pos_switch = createParam<widgetry::Switch>(Vec(), my_module, Guide::P_OVERLAY_POSITION);
    HOT_POSITION("k:pos-switch", HotPosKind::Box, pos_switch);
    pos_switch->box = bounds["k:pos-switch"];
    pos_switch->applyTheme(svg_theme);
    addChild(pos_switch);

    small_knob = Center(createThemeSvgParam<TinyKnob>(&my_svgs, bounds["k:x"].getCenter(), my_module, Guide::P_X));
    HOT_POSITION("k:x", HotPosKind::Center, small_knob);
    small_knob->step_increment_by = 3.75f;
    addChild(small_knob);

    small_knob = Center(createThemeSvgParam<TinyKnob>(&my_svgs, bounds["k:y"].getCenter(), my_module, Guide::P_Y));
    HOT_POSITION("k:y", HotPosKind::Center, small_knob);
    small_knob->step_increment_by = 3.75f;
    addChild(small_knob);

    angle_switch = createParam<widgetry::Switch>(Vec(), my_module, Guide::P_ANGLE);
    HOT_POSITION("k:angle", HotPosKind::Box, angle_switch);
    angle_switch->box = bounds["k:angle"];
    angle_switch->applyTheme(svg_theme);
    addChild(angle_switch);

    small_knob = Center(createThemeSvgParam<TinyKnob>(&my_svgs, bounds["k:width"].getCenter(), my_module, Guide::P_W));
    HOT_POSITION("k:width", HotPosKind::Center, small_knob);
    small_knob->step_increment_by = 3.75f;
    addChild(small_knob);

    small_knob = Center(createThemeSvgParam<TinyKnob>(&my_svgs, bounds["k:repeat"].getCenter(), my_module, Guide::P_R));
    HOT_POSITION("k:repeat", HotPosKind::Center, small_knob);
    small_knob->step_increment_by = 3.75f;
    addChild(small_knob);

    {
        auto menu = createWidgetCentered<TemplateMenu>(bounds["k:template-menu"].getCenter());
        HOT_POSITION("k:template-menu", HotPosKind::Center, menu);
        menu->set_ui(this);
        menu->applyTheme(svg_theme);
        addChild(menu);
    }

    name_input = new TextInput();
    name_input->box = bounds["k:name-edit"];
    HOT_POSITION("k:name-edit", HotPosKind::Box, name_input);
    name_input->text_height = 14.f;
    name_input->placeholder = "GUIDE NAME";
    name_input->set_on_change([=](std::string text) {
        guideline->name = text;
    });
    name_input->applyTheme(svg_theme);
    addChild(name_input);

    {
        auto palette = Center(createThemeSvgButton<Palette5ActionButton>(&my_svgs, bounds["k:pick-guide"].getCenter()));
        HOT_POSITION("k:pick-guide", HotPosKind::Center, palette);
        palette->describe("Guide color");
        palette->set_handler([=](bool,bool) {
            auto picker = new ColorPickerMenu();
            picker->set_color(guideline->color);
            picker->set_on_new_color([=](PackedColor color) { set_guide_color(guideline, color); });
            auto menu = createMenu();
            menu->addChild(picker);
        });
        addChild(palette);

        guide_swatch = new Swatch;
        HOT_POSITION("k:guide-swatch", HotPosKind::Box, guide_swatch);
        guide_swatch->box = bounds["k:guide-swatch"];
        guide_swatch->color = guideline->color;
        addChild(guide_swatch);

        guide_solid = new SolidSwatch;
        HOT_POSITION("k:guide-solid", HotPosKind::Box, guide_solid);
        guide_solid->box = bounds["k:guide-solid"];
        guide_solid->color =  guideline->color;
        addChild(guide_solid);
    }

    auto tiny_button = Center(createThemeSvgButton<TinyActionButton>(&my_svgs, bounds["k:add"].getCenter()));
    HOT_POSITION("k:add", HotPosKind::Center, tiny_button);
    tiny_button->describe("Add guide");
    tiny_button->set_handler([=](bool,bool){
        add_guide(guideline);
        name_input->setText("");
    });
    addChild(tiny_button);

    tiny_button = Center(createThemeSvgButton<TinyActionButton>(&my_svgs, bounds["k:remove"].getCenter()));
    HOT_POSITION("k:remove", HotPosKind::Center, tiny_button);
    tiny_button->describe("Remove guide");
    tiny_button->set_handler([=](bool,bool){
        remove_guide(guide_list->get_selected_guide());
        guideline = std::make_shared<GuideLine>();
        name_input->setText("");
    });
    addChild(tiny_button);

    tiny_button = Center(createThemeSvgButton<TinyActionButton>(&my_svgs, bounds["k:clear"].getCenter()));
    HOT_POSITION("k:clear", HotPosKind::Center, tiny_button);
    tiny_button->describe("Clear guides");
    tiny_button->set_handler([=](bool,bool){
        guide_data->guides.clear();
        guideline = std::make_shared<GuideLine>();
        name_input->setText("");
    });
    addChild(tiny_button);

    guide_list = new GuideList;
    HOT_POSITION("k:guide-list", HotPosKind::Box, guide_list);
    guide_list->box = bounds["k:guide-list"];
    guide_list->guide_data = guide_data;
    guide_list->set_click_handler([=](std::shared_ptr<GuideLine> guide){ set_guide(guide); });
    guide_list->applyTheme(svg_theme);
    addChild(guide_list);

    my_svgs.changeTheme(svg_theme);
}

GuideUi::~GuideUi()
{
    if (panel_guides) {
        panel_guides->ui = nullptr;
        panel_guides->requestDelete();
        panel_guides = nullptr;
    }
}

void GuideUi::onExpanderChange(Module::Expander &expander)
{
    if (expander.module) {
        auto ms = APP->scene->rack->getModules();
        for (auto mw: ms) {
            if (mw->module == expander.module) {
                auto pg = getPanelGuide(mw);
                if (panel_guides && panel_guides != pg) {
                    panel_guides->requestDelete();
                    panel_guides = pg;
                }
                if (!panel_guides) {
                    panel_guides = new PanelGuides();
                    panel_guides->data = guide_data;
                    add_layered_child(mw, panel_guides, guide_data->position);
                }
                break;
            }
        }
    } else {
        if (panel_guides) {
            panel_guides->requestDelete();
            panel_guides = nullptr;
        }
    }
}

void GuideUi::set_overlay_position(OverlayPosition pos)
{
    if (pos != guide_data->position) {
        guide_data->position = pos;
        if (!panel_guides) return;

        auto mw = panel_guides->getParent();
        mw->removeChild(panel_guides);
        add_layered_child(mw, panel_guides, pos);
    }
}

void GuideUi::set_panel_overlay_color(PackedColor co_panel)
{
    panel_swatch->color = panel_solid->color = co_panel;
    guide_data->co_overlay = co_panel;
    if (panel_guides) { sendDirty(panel_guides); }
}

void GuideUi::set_guide_color(std::shared_ptr<GuideLine> guide, PackedColor co_guide) {
    guide_swatch->color = guide_solid->color = co_guide;
    if (guide) {
        guide->color = co_guide;
        if (panel_guides) { sendDirty(panel_guides); }
    }
}

void GuideUi::add_guide(std::shared_ptr<GuideLine> guide) {
    auto it = std::find(guide_data->guides.begin(), guide_data->guides.end(), guide);
    if (it == guide_data->guides.end()) {
        if (guide_data->guides.size() < 14) {
            guide_data->guides.push_back(guide);
            if (guide->name.empty()) {
                guide->name = format_string("Guide #%d", guide_data->guides.size());
            }
        }
    } else {
        guide_list->selected_guide = (it - guide_data->guides.begin());
    }
}

void GuideUi::remove_guide(std::shared_ptr<GuideLine> guide) {
    if (!guide) return;
    auto it = std::find(guide_data->guides.begin(), guide_data->guides.end(), guide);
    if (it != guide_data->guides.end()) {
        guide_data->guides.erase(it);
        guide_list->selected_guide = it - guide_data->guides.begin() - 1;
    }
}

void GuideUi::set_guide(std::shared_ptr<GuideLine> guide) {
    if (!guide) {
        guide = std::make_shared<GuideLine>();
    }
    guideline = guide;
    if (my_module) my_module->set_guide(guide);
    guide_swatch->color = guide_solid->color = guideline->color;
    name_input->setText(guide->name);
    dirtyWidget(this);
}

void GuideUi::save_guides() {
    if (!module) return;
    std::string path;
    if (my_module->guide_folder.empty()) {
        my_module->guide_folder = user_plugin_asset("Guides");
        system::createDirectories(my_module->guide_folder);
    }
    bool ok = saveFileDialog(my_module->guide_folder, file_dialog_filter, "", path);
    if (ok) {
        my_module->guide_folder = system::getDirectory(path);
        auto ext = system::getExtension(path);
        if (ext.empty()) {
            path.append(".json");
        }

        auto root = guide_data->toJson();
        if (!root) return;
        DEFER({json_decref(root);});

        std::string tmpPath = system::join(my_module->guide_folder, TempName(".tmp.json"));
        FILE* file = std::fopen(tmpPath.c_str(), "w");
        if (!file) {
            system::remove(tmpPath);
            return;
        }
        json_dumpf(root, file, JSON_INDENT(2));
        std::fclose(file);
        system::sleep(0.0005);
        system::remove(path);
        system::sleep(0.0005);
        system::rename(tmpPath, path);
    }

}

void GuideUi::load_guide_file(std::string path) {
    auto first = guide_data->guides.size();
    FILE* file = std::fopen(path.c_str(), "r");
    if (!file) return;
    DEFER({std::fclose(file);});

    json_error_t error;
    json_t* root = json_loadf(file, 0, &error);
    if (!root) {
        WARN("Invalid JSON at %d:%d %s in %s", error.line, error.column, error.text, path.c_str());
        return;
    }
    DEFER({json_decref(root);});
    guide_data->fromJson(root);

    if (guide_data->guides.size() > first) {
        guide_list->selected_guide = first;
        set_guide(guide_data->guides[first]);
    }
}

void GuideUi::open_guides() {
    if (!module) return;
    std::string path;
    if (my_module->guide_folder.empty()) {
        my_module->guide_folder = user_plugin_asset("Guides");
        system::createDirectories(my_module->guide_folder);
    }
    if (openFileDialog(my_module->guide_folder, file_dialog_filter, "", path)) {
        load_guide_file(path);
    }
}

void GuideUi::onChangeTheme(ChangedItem item) {
    if (ChangedItem::Theme == item) {
        auto theme = theme_holder->getTheme();
        auto svg_theme = getThemeCache().getTheme(ThemeName(theme));
        my_svgs.changeTheme(svg_theme);
        applyChildrenTheme(this, svg_theme); // any IThemed widgets
        sendDirty(this);
    }
}

#ifdef HOT_SVG
void GuideUi::onHoverKey(const HoverKeyEvent& e)
{
    if (!my_module) return;
    auto mods = e.mods & RACK_MOD_MASK;
    switch (e.key) {
    case GLFW_KEY_F5: {
        if (e.action == GLFW_RELEASE && (0 == mods)) {
            e.consume(this);
            reloadThemeCache();
            my_svgs.reloadAll();
            onChangeTheme(ChangedItem::Theme);
            auto panel = dynamic_cast<SvgThemePanel<GuideSvg>*>(getPanel());
            positionWidgets(pos_widgets, makeBounds(panel->svg, "k:", true));
            sendDirty(this);
        }
    } break;
    }
    Base::onHoverKey(e);
}
#endif

void GuideUi::appendContextMenu(Menu* menu) {
    if (!module) return;

    menu->addChild(createMenuLabel<HamburgerTitle>("#d Guide"));
    //menu->addChild(createMenuLabel<FancyLabel>("theme"));
    AddThemeMenu(menu, this, theme_holder, false, false, false);
}

void GuideUi::step() {
    Base::step();
    if (!my_module) return;
    theme_holder->pollRackThemeChanged();
}


}

Model* modelGuide = createModel<pachde::Guide, pachde::GuideUi>("pachde-guide");
