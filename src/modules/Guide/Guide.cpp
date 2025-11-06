#include "Guide.hpp"
#include "services/json-help.hpp"
#include "widgets/switch.hpp"

namespace pachde {

void add_layered_child(Widget* widget, Widget* child, OverlayPosition position)
{
    Widget* first = *widget->children.begin();
    if (first) {
        switch (position) {
        default:
        case OverlayPosition::OnPanel:
            widget->addChildAbove(child, first);
            break;
        case OverlayPosition::OnTop:
            widget->addChild(child);
            break;
        }
    } else {
        widget->addChild(child);
    }
}

//json_t * GuideLine::toJson() { return nullptr; }
//void GuideLine::fromJson(json_t * root) { }

//json_t * GuideData::toJson() { return nullptr; }
//void GuideData::fromJson(json_t * root) { }

Guide::Guide() {
    config(Params::NUM_PARAMS, Inputs::NUM_INPUTS, Outputs::NUM_OUTPUTS, Lights::NUM_LIGHTS);
    dp3(configParam(P_X, 0.f, 1500.f, 7.5f, "X", "px"));
    dp3(configParam(P_Y, 0.f, 380.f, 15.f, "Y", "px"));
    dp3(configParam(P_ANGLE, 0.f, 180.f, 90.f, "Angle", "°"));
    dp3(configParam(P_W, 0.f, 60.f, .75f, "Width", "px"));
    dp3(configParam(P_R, 0.f, 60.f, 0.f, "Repeat interval", "px"));
    configSwitch(P_OVERLAY_POSITION, 0.f, 1.f, 0.f, "Overlay position", {
        "Panel",
        "Widgets"
    });
}

json_t * Guide::dataToJson() {
    json_t* root = Base::dataToJson();
    // auto j = guide_data.toJson();
    // if (j) json_object_set_new(root, "guides", j);
    return root;
}

void Guide::dataFromJson(json_t *root) {
    Base::dataFromJson(root);
    //guide_data.fromJson(json_object_get(root, "guides"));
}

void Guide::set_guide(const std::shared_ptr<GuideLine> guide)
{
    getParam(P_X).setValue(guide->origin.x);
    getParam(P_Y).setValue(guide->origin.y);
    getParam(P_ANGLE).setValue(guide->angle);
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

        v = getParam(P_ANGLE).getValue();
        if (v != guide->angle) { change = true; guide->angle = v; }

        v = getParam(P_W).getValue();
        if (v != guide->width) { change = true; guide->width = v; }

        v = getParam(P_R).getValue();
        if (v != guide->repeat) { change = true; guide->repeat = v; }

        if (change) {
            sendDirty(ui->panel_guide);
        }
    }
}

struct GuideSvg {
    static std::string background() { return asset::plugin(pluginInstance, "res/Guide.svg"); }
};


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
    BoundsIndex bounds;
    boundsIndex(layout, "k:", bounds, true);
    list_box = elementBounds(layout, "guide-list");

    {
        auto palette = Center(createThemeSvgButton<Palette1ActionButton>(&my_svgs, bounds["k:pick-panel"].getCenter()));
        HOT_POSITION("k:pick-panel", palette);
        palette->describe("Panel overlay color");
        palette->set_handler([=](bool,bool) {
            auto picker = new ColorPickerMenu();
            picker->set_color(guide_data->co_overlay);
            picker->set_on_new_color([=](PackedColor color) { set_panel_overlay_color(color); });
            auto menu = createMenu();
            menu->addChild(picker);
        });
        addChild(palette);

        Rect r = bounds["k:panel-swatch"];
        panel_swatch = createWidgetCentered<Swatch>(r.getCenter());
        HOT_POSITION("k:panel-swatch", panel_swatch);
        panel_swatch->box = r;
        panel_swatch->color = guide_data->co_overlay;
        addChild(panel_swatch);
    }

    {
        Rect r = bounds["k:pos-switch"];
        auto pos_switch = Center(createThemeParam<widgetry::Switch>(theme_holder->getTheme(), r.getCenter(), my_module, Guide::P_OVERLAY_POSITION));
        HOT_POSITION("k:pos-switch", pos_switch);
        pos_switch->box = r;
        addChild(pos_switch);
    }

    small_knob = Center(createThemeSvgParam<TinyKnob>(&my_svgs, bounds["k:x"].getCenter(), my_module, Guide::P_X));
    HOT_POSITION("k:x", small_knob);
    small_knob->step_increment_by = 3.75f;
    addChild(small_knob);

    small_knob = Center(createThemeSvgParam<TinyKnob>(&my_svgs, bounds["k:y"].getCenter(), my_module, Guide::P_Y));
    HOT_POSITION("k:y", small_knob);
    small_knob->step_increment_by = 3.75f;
    addChild(small_knob);

    small_knob = Center(createThemeSvgParam<TinyKnob>(&my_svgs, bounds["k:angle"].getCenter(), my_module, Guide::P_ANGLE));
    HOT_POSITION("k:angle", small_knob);
    small_knob->step_increment_by = 15.f;
    addChild(small_knob);

    small_knob = Center(createThemeSvgParam<TinyKnob>(&my_svgs, bounds["k:width"].getCenter(), my_module, Guide::P_W));
    HOT_POSITION("k:width", small_knob);
    small_knob->step_increment_by = 3.75f;
    addChild(small_knob);

    small_knob = Center(createThemeSvgParam<TinyKnob>(&my_svgs, bounds["k:repeat"].getCenter(), my_module, Guide::P_R));
    HOT_POSITION("k:repeat", small_knob);
    small_knob->step_increment_by = 3.75f;
    addChild(small_knob);

    Rect r = bounds["k:name-edit"];
    name_input = new TextInput();
    name_input->box = r;
    HOT_POSITION("k:name-edit", name_input);
    name_input->text_height = 14.f;
    name_input->set_on_change([=](std::string text) {
        guideline->name = text;
    });
    addChild(name_input);

    {
        auto palette = Center(createThemeSvgButton<Palette5ActionButton>(&my_svgs, bounds["k:pick-guide"].getCenter()));
        HOT_POSITION("k:pick-guide", palette);
        palette->describe("Guide color");
        palette->set_handler([=](bool,bool) {
            auto picker = new ColorPickerMenu();
            picker->set_color(guideline->color);
            picker->set_on_new_color([=](PackedColor color) { set_guide_color(guideline, color); });
            auto menu = createMenu();
            menu->addChild(picker);
        });
        addChild(palette);

        Rect r = bounds["k:guide-swatch"];
        guide_swatch = createWidgetCentered<Swatch>(r.getCenter());
        HOT_POSITION("k:guide-swatch", guide_swatch);
        guide_swatch->box = r;
        guide_swatch->color = guideline->color;
        addChild(guide_swatch);
    }

    auto tiny_button = Center(createThemeSvgButton<TinyActionButton>(&my_svgs, bounds["k:add"].getCenter()));
    HOT_POSITION("k:add", tiny_button);
    tiny_button->describe("Add guide");
    tiny_button->set_handler([=](bool,bool){
        auto old_guide = guideline;
        guideline = nullptr;
        add_guide(old_guide);
        guideline = std::make_shared<GuideLine>();
        name_input->setText("");
    });
    addChild(tiny_button);

    tiny_button = Center(createThemeSvgButton<TinyActionButton>(&my_svgs, bounds["k:remove"].getCenter()));
    HOT_POSITION("k:remove", tiny_button);
    tiny_button->describe("Remove guide");
    tiny_button->set_handler([=](bool,bool){
        remove_guide(guideline);
    });
    addChild(tiny_button);

    my_svgs.changeTheme(svg_theme);
}

GuideUi::~GuideUi()
{
    if (panel_guide) {
        panel_guide->ui = nullptr;
        //panel_guide->requestDelete();
        //panel_guide = nullptr;
    }
}

void GuideUi::onExpanderChange(Module::Expander &expander) {
    if (expander.module) {
        Model* expander_model = expander.module->model;
        auto ms = APP->scene->rack->getModules();
        for (auto mw: ms) {
            if (mw->model == expander_model) {
                auto pg = getPanelGuide(mw);
                if (panel_guide && panel_guide != pg) {
                    panel_guide->requestDelete();
                    panel_guide = pg;
                }
                if (!panel_guide) {
                    panel_guide = new PanelGuide();
                    panel_guide->data = guide_data;
                    add_layered_child(mw, panel_guide, guide_data->position);
                }
                break;
            }
        }
    } else {
        if (panel_guide) {
            panel_guide->requestDelete();
            panel_guide = nullptr;
        }
    }
}

void GuideUi::set_overlay_position(OverlayPosition pos)
{
    if (pos != guide_data->position) {
        guide_data->position = pos;
        if (!panel_guide) return;

        auto mw = panel_guide->getParent();
        mw->removeChild(panel_guide);
        add_layered_child(mw, panel_guide, pos);
    }
}

void GuideUi::set_panel_overlay_color(PackedColor co_panel)
{
    panel_swatch->color = co_panel;
    guide_data->co_overlay = co_panel;
    if (panel_guide) { sendDirty(panel_guide); }
}

void GuideUi::set_guide_color(std::shared_ptr<GuideLine> guide, PackedColor co_guide) {
    guide_swatch->color = co_guide;
    if (guide) {
        guide->color = co_guide;
        if (panel_guide) { sendDirty(panel_guide); }
    }
}

void GuideUi::add_guide(std::shared_ptr<GuideLine> guide) {
    auto it = std::find(guide_data->guides.begin(), guide_data->guides.end(), guide);
    if (it == guide_data->guides.end()) {
        guide_data->guides.push_back(guide);
    }
}

void GuideUi::remove_guide(std::shared_ptr<GuideLine> guide) {
    // auto it = std::find(guide_data->guides.begin(), guide_data->guides.end(), guide);
    // if (it != guide_data->guides.end()) {
    //     guide_data->guides.erase(it);
    // }
    auto lit = guide_data->guides.begin();
    for (auto it = guide_data->guides.begin() + 1; it != guide_data->guides.end(); it++) {
        lit = it;
    }
    guide_data->guides.erase(lit);
}

void GuideUi::onChangeTheme(ChangedItem item) {
    if (ChangedItem::Theme == item) {
        auto svg_theme = getThemeCache().getTheme(ThemeName(theme_holder->getTheme()));
        my_svgs.changeTheme(svg_theme);
        //applyChildrenTheme(this, svg_theme); // any IThemed widgets
        sendDirty(this);
    }
}

void GuideUi::onHoverKey(const HoverKeyEvent& e)
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
            auto panel = dynamic_cast<SvgThemePanel<GuideSvg>*>(getPanel());
            BoundsIndex bounds;
            boundsIndex(panel->svg, "k:", bounds, true);
            for (auto kv: positioned_widgets) {
                kv.second->box.pos = bounds[kv.first].getCenter();
                Center(kv.second);
            }
            sendDirty(this);
        }
    } break;
#endif
    }
    Base::onHoverKey(e);
}

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

void GuideUi::draw(const DrawArgs &args)
{
    Base::draw(args);
    if (guide_data->guides.size() > 0) {

        auto font = GetPluginFontRegular();
        if (!FontOk(font)) return;
        auto vg = args.vg;

        NVGcolor co_bg = RampGray(G_65);
        NVGcolor co_text = RampGray(G_0);
        NVGcolor co_hi = nvgHSL(42.f/360.f, .8, .8);

        nvgTextAlign(vg, NVG_ALIGN_TOP|NVG_ALIGN_LEFT);
        nvgFontSize(vg, 14.f);

        int guide_id = 1;
        float x = list_box.pos.x + 1.5f;
        float y = list_box.pos.y + 1.5f;
        float w = list_box.size.x - 3.f;
        float h = 16;
        for (auto guide: guide_data->guides) {

            FillRect(vg, x, y, w, h-.5f, co_bg);

            nvgFillColor(vg, co_text);
            std::string name = guide->name.empty() ? format_string("Guide #%d", guide_id) : guide->name;
            nvgText(vg, x + 2.5f, y + 1.5f, name.c_str(), nullptr);

            if (guide == guideline) {
                FittedBoxRect(vg, x, y, w, h, co_hi, Fit::Inside, 1.f);
            }
            guide_id++;
            y += h;
        }

    }
}

}

Model* modelGuide = createModel<pachde::Guide, pachde::GuideUi>("pachde-guide");
