#include "Fancy.hpp"
#include "services/rack-help.hpp"
#include "services/svg-query.hpp"
#include "services/svg-theme-load.hpp"
#include "widgets/action-button.hpp"
#include "widgets/color-picker.hpp"
#include "widgets/components.hpp"
#include "widgets/hamburger.hpp"
#include "widgets/label.hpp"
#include "widgets/port.hpp"
#include "widgets/screws.hpp"

using namespace widgetry;

namespace pachde {

struct FancySvg {
    static std::string background() { return asset::plugin(pluginInstance, "res/FancyBox.svg"); }
};

FancyUi::FancyUi(Fancy* module) : my_module(module) {
    setModule(module);
    if (my_module) {
        my_module->ui = this;
        my_module->other_fancy = !is_singleton(my_module);
    }
    theme_holder = my_module ? my_module : new ThemeBase();
    theme_holder->setNotify(this);
    auto theme = theme_holder->getTheme();
    auto svg_theme = getThemeCache().getTheme(ThemeName(theme));
    auto panel = createSvgThemePanel<FancySvg>(&my_svgs, nullptr);
    if (my_module && !my_module->show_ports) {
        panel->box.size.x = 165.f;
    }
    auto layout = panel->svg;
    setPanel(panel);

    if (my_module && my_module->other_fancy) return;

    title_style = new LabelStyle("fancy-title", colors::Black, 14.f, true);
    title_style->valign = VAlign::Top;
    title_style->applyTheme(svg_theme);

    group_style = new LabelStyle("fancy-group", "hsl(42, .5, .6)", 12.f, false);
    group_style->orientation = Orientation::Up;
    group_style->halign = HAlign::Right;
    group_style->valign = VAlign::Middle;
    group_style->applyTheme(svg_theme);

    label_style = new LabelStyle("fancy-label");
    label_style->applyTheme(svg_theme);

    ::svg_query::BoundsIndex bounds;
    svg_query::addBounds(layout, "k:", bounds, true);

    auto light = createLightCentered<SmallLight<RedLight>>(bounds["k:fancy-light"].getCenter(), my_module, Fancy::L_FANCY);
    HOT_POSITION("k:fancy-light", HotPosKind::Center, light);
    addChild(light);

    addChild(fancy_button = makeTextButton(bounds, "k:fancy-btn", true, "", "Toggle Fancy background", svg_theme,
        [this](bool ctrl, bool shift) { if(!my_module) return; fancy_background(!my_module->fancy); }));

    { // Jack toggle
        const char * jack_key = show_ports() ? "k:jack-btn-close-ports" : "k:jack-btn-open-ports";
        addChild(jack_button = Center(createThemeSvgButton<JackButton>(&my_svgs, bounds[jack_key].getCenter())));
        HOT_POSITION(jack_key, HotPosKind::Center, jack_button);
        jack_button->set_sticky(true);
        jack_button->set_hover(true);
        jack_button->set_hover_key("jack-hover");
        jack_button->applyTheme(svg_theme);
        if (my_module) {
            jack_button->set_handler([=](bool,bool) { toggle_ports(); });
        }
    }
    add_check(bounds, "k:fill-check", Fancy::P_FANCY_FILL_ON, svg_theme);
    add_label(bounds, "k:fill-group", "TONE", group_style, svg_theme);
    auto palette = Center(createThemeSvgButton<Palette20ActionButton>(&my_svgs, bounds["k:fill-co"].getCenter()));
    HOT_POSITION("k:fill-co", HotPosKind::Center, palette);
    palette->describe("Fill color");
    if (my_module) {
        palette->set_handler([=](bool,bool) {
            auto picker = new ColorPickerMenu();
            createMenu()->addChild(picker);
            picker->set_color(my_module->fancy_data.fill.color);
            picker->set_on_new_color([=](PackedColor color) { set_fill_color(color); });
        });
    }
    addChild(palette);
    add_knob(bounds, "k:fill-fade", Fancy::P_FANCY_FILL_FADE);
    add_label(bounds, "k:fill-fade-label", "FADE", label_style, svg_theme);

    add_check(bounds, "k:lg-check", Fancy::P_FANCY_LINEAR_ON, svg_theme);
    add_label(bounds, "k:lg-group", "LINEAR GRADIENT", group_style, svg_theme);
    palette = Center(createThemeSvgButton<Palette20ActionButton>(&my_svgs, bounds["k:lg-start-co"].getCenter()));
    HOT_POSITION("k:lg-start-co", HotPosKind::Center, palette);
    palette->describe("Linear gradient start color");
    if (my_module) {
        palette->set_handler([=](bool,bool) {
            auto picker = new ColorPickerMenu();
            createMenu()->addChild(picker);
            picker->set_color(my_module->fancy_data.linear.icol);
            picker->set_on_new_color([=](PackedColor color) { set_lg_start_color(color); });
        });
    }
    addChild(palette);
    add_knob(bounds, "k:lg-start-fade", Fancy::P_FANCY_LINEAR_START_FADE);
    add_knob(bounds, "k:lg-start-x", Fancy::P_FANCY_LINEAR_X1);
    add_knob(bounds, "k:lg-start-y", Fancy::P_FANCY_LINEAR_Y1);
    add_label(bounds, "k:lg-start-fade-label", "FADE", label_style, svg_theme);
    add_label(bounds, "k:lg-x1-label", "X1", label_style, svg_theme);
    add_label(bounds, "k:lg-y1-label", "Y1", label_style, svg_theme);

    palette = Center(createThemeSvgButton<Palette20ActionButton>(&my_svgs, bounds["k:lg-end-co"].getCenter()));
    HOT_POSITION("k:lg-end-co", HotPosKind::Center, palette);
    palette->describe("Linear gradient end color");
    if (my_module) {
        palette->set_handler([=](bool,bool) {
            auto picker = new ColorPickerMenu();
            createMenu()->addChild(picker);
            picker->set_color(my_module->fancy_data.linear.ocol);
            picker->set_on_new_color([=](PackedColor color) { set_lg_end_color(color); });
        });
    }
    addChild(palette);
    add_knob(bounds, "k:lg-end-fade", Fancy::P_FANCY_LINEAR_END_FADE);
    add_knob(bounds, "k:lg-end-x", Fancy::P_FANCY_LINEAR_X2);
    add_knob(bounds, "k:lg-end-y", Fancy::P_FANCY_LINEAR_Y2);
    add_label(bounds, "k:lg-end-fade-label", "FADE", label_style, svg_theme);
    add_label(bounds, "k:lg-x2-label", "X2", label_style, svg_theme);
    add_label(bounds, "k:lg-y2-label", "Y2", label_style, svg_theme);

    add_check(bounds, "k:rg-check", Fancy::P_FANCY_RADIAL_ON, svg_theme);
    add_label(bounds, "k:rg-group", "RADIAL GRADIENT", group_style, svg_theme);
    palette = Center(createThemeSvgButton<Palette20ActionButton>(&my_svgs, bounds["k:rg-inner-co"].getCenter()));
    HOT_POSITION("k:rg-inner-co", HotPosKind::Center, palette);
    palette->describe("Radial gradient inner color");
    if (my_module) {
        palette->set_handler([=](bool,bool) {
            auto picker = new ColorPickerMenu();
            createMenu()->addChild(picker);
            picker->set_color(my_module->fancy_data.radial.icol);
            picker->set_on_new_color([=](PackedColor color) { set_rg_inner_color(color); });
        });
    }
    addChild(palette);
    add_knob(bounds, "k:rg-inner-fade", Fancy::P_FANCY_RADIAL_INNER_FADE);
    add_knob(bounds, "k:rg-cx", Fancy::P_FANCY_RADIAL_CX);
    add_knob(bounds, "k:rg-cy", Fancy::P_FANCY_RADIAL_CY);
    add_label(bounds, "k:rg-inner-fade-label", "FADE", label_style, svg_theme);
    add_label(bounds, "k:rg-cx-label", "CX", label_style, svg_theme);
    add_label(bounds, "k:rg-cy-label", "CY", label_style, svg_theme);
    palette = Center(createThemeSvgButton<Palette20ActionButton>(&my_svgs, bounds["k:rg-outer-co"].getCenter()));
    HOT_POSITION("k:rg-outer-co", HotPosKind::Center, palette);
    palette->describe("Radial gradient outer color");
    if (my_module) {
        palette->set_handler([=](bool,bool) {
            auto picker = new ColorPickerMenu();
            createMenu()->addChild(picker);
            picker->set_color(my_module->fancy_data.radial.ocol);
            picker->set_on_new_color([=](PackedColor color) { set_rg_outer_color(color); });
        });
    }
    addChild(palette);
    add_knob(bounds, "k:rg-outer-fade", Fancy::P_FANCY_RADIAL_OUTER_FADE);
    add_knob(bounds, "k:rg-radius", Fancy::P_FANCY_RADIAL_RADIUS);
    // add_label(bounds, "k:rg-outer-label", "outer", label_style, svg_theme);
    add_label(bounds, "k:rg-outer-fade-label", "FADE", label_style, svg_theme);
    add_label(bounds, "k:rg-radius-label", "RADIUS", label_style, svg_theme);

    add_check(bounds, "k:bg-check", Fancy::P_FANCY_BOX_ON, svg_theme);
    add_label(bounds, "k:bg-group", "BOX GRADIENT", group_style, svg_theme);
    palette = Center(createThemeSvgButton<Palette20ActionButton>(&my_svgs, bounds["k:bg-inner-co"].getCenter()));
    HOT_POSITION("k:bg-inner-co", HotPosKind::Center, palette);
    palette->describe("Box gradient inner color");
    if (my_module) {
        palette->set_handler([=](bool,bool) {
            auto picker = new ColorPickerMenu();
            createMenu()->addChild(picker);
            picker->set_color(my_module->fancy_data.boxg.icol);
            picker->set_on_new_color([=](PackedColor color) { set_bg_inner_color(color); });
        });
    }
    addChild(palette);
    add_knob(bounds, "k:bg-inner-fade", Fancy::P_FANCY_BOX_INNER_FADE);
    add_knob(bounds, "k:bg-shrink-x", Fancy::P_FANCY_BOX_SHRINK_X);
    add_knob(bounds, "k:bg-shrink-y", Fancy::P_FANCY_BOX_SHRINK_Y);
    add_label(bounds, "k:bg-inner-fade-label", "FADE", label_style, svg_theme);
    add_label(bounds, "k:bg-shrink-x-label", "X", label_style, svg_theme);
    add_label(bounds, "k:bg-shrink-label", "shrink", label_style, svg_theme);
    add_label(bounds, "k:bg-shrink-y-label", "Y", label_style, svg_theme);
    palette = Center(createThemeSvgButton<Palette20ActionButton>(&my_svgs, bounds["k:bg-outer-co"].getCenter()));
    HOT_POSITION("k:bg-outer-co", HotPosKind::Center, palette);
    palette->describe("Box gradient outer color");
    palette->set_handler([=](bool,bool) {
        auto picker = new ColorPickerMenu();
        createMenu()->addChild(picker);
        picker->set_color(my_module->fancy_data.boxg.ocol);
        picker->set_on_new_color([=](PackedColor color) { set_bg_outer_color(color); });
    });
    addChild(palette);
    add_knob(bounds, "k:bg-outer-fade", Fancy::P_FANCY_BOX_OUTER_FADE);
    add_knob(bounds, "k:bg-radius", Fancy::P_FANCY_BOX_RADIUS);
    add_knob(bounds, "k:bg-feather", Fancy::P_FANCY_BOX_FEATHER);

    add_label(bounds, "k:bg-outer-fade-label", "FADE", label_style, svg_theme);
    add_label(bounds, "k:bg-radius-label", "RADIUS", label_style, svg_theme);
    add_label(bounds, "k:bg-feather-label", "FEATHER", label_style, svg_theme);

    { // swatches
        CloakData * fancy_data = my_module ? &my_module->fancy_data : new CloakData;
        addChild(fill_swatch     = new FancySwatch(bounds["k:fill-swatch"], fancy_data->fill.color));
        addChild(lg_start_swatch = new FancySwatch(bounds["k:lg-start-swatch"], fancy_data->linear.icol));
        addChild(lg_end_swatch   = new FancySwatch(bounds["k:lg-end-swatch"], fancy_data->linear.ocol));
        addChild(rg_inner_swatch = new FancySwatch(bounds["k:rg-inner-swatch"], fancy_data->radial.icol));
        addChild(rg_outer_swatch = new FancySwatch(bounds["k:rg-outer-swatch"], fancy_data->radial.ocol));
        addChild(bg_inner_swatch = new FancySwatch(bounds["k:bg-inner-swatch"], fancy_data->boxg.icol));
        addChild(bg_outer_swatch = new FancySwatch(bounds["k:bg-outer-swatch"], fancy_data->boxg.ocol));
        if (!my_module) delete fancy_data;

#ifdef HOT_SVG
        addPosition(pos_widgets, "k:fill-swatch",     HotPosKind::Box, fill_swatch);
        addPosition(pos_widgets, "k:lg-start-swatch", HotPosKind::Box, lg_start_swatch);
        addPosition(pos_widgets, "k:lg-end-swatch",   HotPosKind::Box, lg_end_swatch);
        addPosition(pos_widgets, "k:rg-inner-swatch", HotPosKind::Box, rg_inner_swatch);
        addPosition(pos_widgets, "k:rg-outer-swatch", HotPosKind::Box, rg_outer_swatch);
        addPosition(pos_widgets, "k:bg-inner-swatch", HotPosKind::Box, bg_inner_swatch);
        addPosition(pos_widgets, "k:bg-outer-swatch", HotPosKind::Box, bg_outer_swatch);
#endif
    }
    if (show_ports()) {
        add_ports(bounds, svg_theme);
    }
    shouting_buttons(my_module ? my_module->shouting : true);
    my_svgs.changeTheme(svg_theme);

// #ifdef DEV_BUILD
//     DEBUG("BOUNDS: %d", (int)bounds.size());
//     for (auto bound: bounds) {
//         DEBUG("%s (%.3f,%.3f:%.3f,%.3f)", bound.first.c_str(), RECT_ARGS(bound.second));
//     }
//     DEBUG("POSITIONS: %d", (int)pos_widgets.size());
//     for (auto pos: pos_widgets) {
//         DEBUG("%s %d (%.3f,%.3f:%.3f,%.3f)", pos.first, static_cast<int>(pos.second.kind), RECT_ARGS(pos.second.widget->box));
//     }
// #endif
    from_module();

}

void FancyUi::add_ports(::svg_query::BoundsIndex &bounds, std::shared_ptr<svg_theme::SvgTheme> svg_theme)
{
    add_input(bounds, "k:in-fill-H",          Fancy::IN_FANCY_FILL_H, colors::PortRed);
    add_input(bounds, "k:in-fill-S",          Fancy::IN_FANCY_FILL_S, colors::PortMagenta);
    add_input(bounds, "k:in-fill-L",          Fancy::IN_FANCY_FILL_L, colors::PortLightOrange);
    add_input(bounds, "k:in-fill-A",          Fancy::IN_FANCY_FILL_A, colors::PortLightViolet);
    add_input(bounds, "k:in-fill-fade",       Fancy::IN_FANCY_FILL_FADE, colors::PortDefault);

    add_label(bounds, "k:in-H-label", "H", label_style, svg_theme, true);
    add_label(bounds, "k:in-S-label", "S", label_style, svg_theme, true);
    add_label(bounds, "k:in-L-label", "L", label_style, svg_theme, true);
    add_label(bounds, "k:in-A-label", "A", label_style, svg_theme, true);
    add_label(bounds, "k:in-fade-label", "FADE", label_style, svg_theme, true);

    add_input(bounds, "k:in-lg-start-H",      Fancy::IN_FANCY_LINEAR_START_H, colors::PortRed);
    add_input(bounds, "k:in-lg-start-S",      Fancy::IN_FANCY_LINEAR_START_S, colors::PortMagenta);
    add_input(bounds, "k:in-lg-start-L",      Fancy::IN_FANCY_LINEAR_START_L, colors::PortLightOrange);
    add_input(bounds, "k:in-lg-start-A",      Fancy::IN_FANCY_LINEAR_START_A, colors::PortLightViolet);
    add_input(bounds, "k:in-lg-start-fade",   Fancy::IN_FANCY_LINEAR_START_FADE, colors::PortDefault);

    add_input(bounds, "k:in-lg-end-H",        Fancy::IN_FANCY_LINEAR_END_H, colors::PortRed);
    add_input(bounds, "k:in-lg-end-S",        Fancy::IN_FANCY_LINEAR_END_S, colors::PortMagenta);
    add_input(bounds, "k:in-lg-end-L",        Fancy::IN_FANCY_LINEAR_END_L, colors::PortLightOrange);
    add_input(bounds, "k:in-lg-end-A",        Fancy::IN_FANCY_LINEAR_END_A, colors::PortLightViolet);
    add_input(bounds, "k:in-lg-end-fade",     Fancy::IN_FANCY_LINEAR_END_FADE, colors::PortDefault);

    add_input(bounds, "k:in-lg-x1",           Fancy::IN_FANCY_LINEAR_X1, colors::PortDefault);
    add_input(bounds, "k:in-lg-y1",           Fancy::IN_FANCY_LINEAR_Y1, colors::PortDefault);
    add_input(bounds, "k:in-lg-x2",           Fancy::IN_FANCY_LINEAR_X2, colors::PortDefault);
    add_input(bounds, "k:in-lg-y2",           Fancy::IN_FANCY_LINEAR_Y2, colors::PortDefault);
    add_label(bounds, "k:in-lg-x1-label", "X1", label_style, svg_theme, true);
    add_label(bounds, "k:in-lg-y1-label", "Y1", label_style, svg_theme, true);
    add_label(bounds, "k:in-lg-x2-label", "X2", label_style, svg_theme, true);
    add_label(bounds, "k:in-lg-y2-label", "Y2", label_style, svg_theme, true);

    add_input(bounds, "k:in-rg-inner-H",      Fancy::IN_FANCY_RADIAL_INNER_H, colors::PortRed);
    add_input(bounds, "k:in-rg-inner-S",      Fancy::IN_FANCY_RADIAL_INNER_S, colors::PortMagenta);
    add_input(bounds, "k:in-rg-inner-L",      Fancy::IN_FANCY_RADIAL_INNER_L, colors::PortLightOrange);
    add_input(bounds, "k:in-rg-inner-A",      Fancy::IN_FANCY_RADIAL_INNER_A, colors::PortLightViolet);
    add_input(bounds, "k:in-rg-inner-fade",   Fancy::IN_FANCY_RADIAL_INNER_FADE, colors::PortDefault);

    add_input(bounds, "k:in-rg-outer-H",      Fancy::IN_FANCY_RADIAL_OUTER_H, colors::PortRed);
    add_input(bounds, "k:in-rg-outer-S",      Fancy::IN_FANCY_RADIAL_OUTER_S, colors::PortMagenta);
    add_input(bounds, "k:in-rg-outer-L",      Fancy::IN_FANCY_RADIAL_OUTER_L, colors::PortLightOrange);
    add_input(bounds, "k:in-rg-outer-A",      Fancy::IN_FANCY_RADIAL_OUTER_A, colors::PortLightViolet);
    add_input(bounds, "k:in-rg-outer-fade",   Fancy::IN_FANCY_RADIAL_OUTER_FADE, colors::PortDefault);

    add_input(bounds, "k:in-rg-cx",           Fancy::IN_FANCY_RADIAL_CX, colors::PortDefault);
    add_input(bounds, "k:in-rg-cy",           Fancy::IN_FANCY_RADIAL_CY, colors::PortDefault);
    add_input(bounds, "k:in-rg-radius",       Fancy::IN_FANCY_RADIAL_RADIUS, colors::PortDefault);

    add_label(bounds, "k:in-rg-cx-label", "CX", label_style, svg_theme, true);
    add_label(bounds, "k:in-rg-cy-label", "CY", label_style, svg_theme, true);
    add_label(bounds, "k:in-rg-radius-label", "RADIUS", label_style, svg_theme, true);

    add_input(bounds, "k:in-bg-inner-H",      Fancy::IN_FANCY_BOX_INNER_H, colors::PortRed);
    add_input(bounds, "k:in-bg-inner-S",      Fancy::IN_FANCY_BOX_INNER_S, colors::PortMagenta);
    add_input(bounds, "k:in-bg-inner-L",      Fancy::IN_FANCY_BOX_INNER_L, colors::PortLightOrange);
    add_input(bounds, "k:in-bg-inner-A",      Fancy::IN_FANCY_BOX_INNER_A, colors::PortLightViolet);
    add_input(bounds, "k:in-bg-inner-fade",   Fancy::IN_FANCY_BOX_INNER_FADE, colors::PortDefault);

    add_input(bounds, "k:in-bg-outer-H",      Fancy::IN_FANCY_BOX_OUTER_H, colors::PortRed);
    add_input(bounds, "k:in-bg-outer-S",      Fancy::IN_FANCY_BOX_OUTER_S, colors::PortMagenta);
    add_input(bounds, "k:in-bg-outer-L",      Fancy::IN_FANCY_BOX_OUTER_L, colors::PortLightOrange);
    add_input(bounds, "k:in-bg-outer-A",      Fancy::IN_FANCY_BOX_OUTER_A, colors::PortLightViolet);
    add_input(bounds, "k:in-bg-outer-fade",   Fancy::IN_FANCY_BOX_OUTER_FADE, colors::PortDefault);

    add_input(bounds, "k:in-bg-x",            Fancy::IN_FANCY_BOX_SHRINK_X, colors::PortDefault);
    add_input(bounds, "k:in-bg-y",            Fancy::IN_FANCY_BOX_SHRINK_Y, colors::PortDefault);
    add_input(bounds, "k:in-bg-radius",       Fancy::IN_FANCY_BOX_RADIUS, colors::PortDefault);
    add_input(bounds, "k:in-bg-feather",      Fancy::IN_FANCY_BOX_FEATHER, colors::PortDefault);

    add_label(bounds, "k:in-bg-x-label", "X", label_style, svg_theme, true);
    add_label(bounds, "k:in-bg-y-label", "Y", label_style, svg_theme, true);
    add_label(bounds, "k:in-bg-radius-label", "RADIUS", label_style, svg_theme, true);
    add_label(bounds, "k:in-bg-feather-label", "FEATHER", label_style, svg_theme, true);
}

void FancyUi::remove_ports() {
    for (auto child: removables) {
#ifdef HOT_SVG
        auto it = std::find_if(pos_widgets.begin(), pos_widgets.end(), [child](const std::pair<const char *const, svg_query::HotPos>& p){
            return child == p.second.widget;
        });
        if (it != pos_widgets.end()) {
            pos_widgets.erase(it);
        }
#endif
        child->requestDelete();
    }
    removables.clear();
}

void FancyUi::toggle_ports()
{
    if (!my_module) return;
    if (show_ports()) {
        // refuse to collapse if any inputs connected
        for (Input& input: my_module->inputs) {
            if (input.isConnected()) {
                jack_button->latched = true;
                return;
            }
        }
    }

    auto layout = panelWidgetSvg(getPanel());
    BoundsIndex bounds;
    addBounds(layout, "k:", bounds, true);
    if (show_ports()) {
        box.size.x = 165;
        APP->scene->rack->setModulePosForce(this, box.pos);
        remove_ports();
        jack_button->setPosition(bounds["k:jack-btn-open-ports"].getCenter());
        Center(jack_button);
        my_module->show_ports = false;
    } else {
        box.size.x = 315;
        APP->scene->rack->setModulePosForce(this, box.pos);
        auto svg_theme = getThemeCache().getTheme(ThemeName(theme_holder->getTheme()));
        add_ports(bounds, svg_theme);
        jack_button->setPosition(bounds["k:jack-btn-close-ports"].getCenter());
        Center(jack_button);
        my_module->show_ports = true;
    }
}

TextButton* FancyUi::makeTextButton (
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

void FancyUi::add_knob(::svg_query::BoundsIndex &bounds, const char *key, int param) {
    auto knob = createParamCentered<RoundBlackKnob>(bounds[key].getCenter(), my_module, param);
    HOT_POSITION(key, HotPosKind::Center, knob);
    addChild(knob);
}

void FancyUi::add_label(
    ::svg_query::BoundsIndex &bounds,
    const char *key,
    const char *text,
    LabelStyle* style,
    std::shared_ptr<svg_theme::SvgTheme> svg_theme,
    bool removable
) {
    auto label = TextLabel::createLabel(bounds[key], text, style, svg_theme);
    HOT_POSITION(key, HotPosKind::Box, label);
    addChild(label);
}

void FancyUi::add_check(
    ::svg_query::BoundsIndex &bounds,
    const char *key,
    int param,
    std::shared_ptr<svg_theme::SvgTheme> svg_theme
) {
    auto check = Center(createThemeParamButton<CheckButton>(
        &my_svgs,
        bounds[key].getCenter(),
        my_module,
        param,
        svg_theme
    ));
    HOT_POSITION(key, HotPosKind::Center, check);
    addChild(check);
}

void FancyUi::add_input(
    ::svg_query::BoundsIndex &bounds,
    const char *key,
    int id,
    PackedColor color
) {
    ColorPort * input;
    input = Center(createColorInput<ColorPort>(
        theme_holder->getTheme(),
        color,
        bounds[key].getCenter(),
        my_module,
        id
    ));
    HOT_POSITION(key, HotPosKind::Center, input);
    addChild(input);
    removables.push_back(input);
}

void FancyUi::shouting_buttons(bool shouting) {
    fancy_button->set_text(shouting ? "FANCYBOX" : "FancyBox");
}

void FancyUi::set_fill_color(PackedColor color) {
    fill_swatch->set_color(color);
    if (my_cloak) {
        my_cloak->data.fill.color = color;
    }
    if (my_module) {
        my_module->fancy_data.fill.color = color;
    }
}

void FancyUi::set_lg_start_color(PackedColor color) {
    lg_start_swatch->set_color(color);
    if (my_cloak) {
        my_cloak->data.linear.icol = color;
    }
    if (my_module) {
        my_module->fancy_data.linear.icol = color;
    }
}

void FancyUi::set_lg_end_color(PackedColor color) {
    lg_end_swatch->set_color(color);
    if (my_cloak) {
        my_cloak->data.linear.ocol = color;
    }
    if (my_module) {
        my_module->fancy_data.linear.ocol = color;
    }
}

void FancyUi::set_rg_inner_color(PackedColor color) {
    rg_inner_swatch->set_color(color);
    if (my_cloak) {
        my_cloak->data.radial.icol = color;
    }
    if (my_module) {
        my_module->fancy_data.radial.icol = color;
    }
}

void FancyUi::set_rg_outer_color(PackedColor color) {
    rg_outer_swatch->set_color(color);
    if (my_cloak) {
        my_cloak->data.radial.ocol = color;
    }
    if (my_module) {
        my_module->fancy_data.radial.ocol = color;
    }
}

void FancyUi::set_bg_inner_color(PackedColor color) {
    bg_inner_swatch->set_color(color);
    if (my_cloak) {
        my_cloak->data.boxg.icol = color;
    }
    if (my_module) {
        my_module->fancy_data.boxg.icol = color;
    }
}

void FancyUi::set_bg_outer_color(PackedColor color) {
    bg_outer_swatch->set_color(color);
    if (my_cloak) {
        my_cloak->data.boxg.ocol = color;
    }
    if (my_module) {
        my_module->fancy_data.boxg.ocol = color;
    }
}

void FancyUi::restore_unmodulated_parameters()
{
    if (!my_module || !my_cloak) return;
    my_cloak->data.init(my_module->fancy_data);
}

void FancyUi::onChangeTheme(ChangedItem item)
{
    if (ChangedItem::Theme == item) {
        auto svg_theme = getThemeCache().getTheme(ThemeName(theme_holder->getTheme()));
        my_svgs.changeTheme(svg_theme);
        applyChildrenTheme(this, svg_theme);
        sendDirty(this);
    }
}

void FancyUi::sync_latch_state() {
    if (!my_module) return;
    fancy_button->latched   = my_module->fancy;
}

void FancyUi::from_module() {
    if (!my_module) return;

    if (my_module->fancy) {
        request_cloak = true;
    }
}

void FancyUi::onDeleteCloak(CloakBackgroundWidget *cloak) {
    assert(my_cloak == cloak);
    if (my_cloak) my_cloak = nullptr;
    my_module->fancy = false;
}

void FancyUi::fancy_background(bool fancy) {
    if (!my_module) return;

    my_module->fancy = fancy;
    auto cloak = getBackgroundCloak();
    if (fancy) {
        if (!cloak) {
            my_cloak = ensureBackgroundCloak(this, &my_module->fancy_data);
        }
    } else {
        if (cloak) {
            my_cloak = nullptr;
            cloak->requestDelete();
        }
    }
}

void FancyUi::onHoverKey(const HoverKeyEvent &e)
{
    if (!my_module) return;
#ifdef HOT_SVG
    auto mods = e.mods & RACK_MOD_MASK;
    switch (e.key) {
    case GLFW_KEY_F5: {
        if (e.action == GLFW_RELEASE && (0 == mods)) {
            e.consume(this);
            reloadThemeCache();
            my_svgs.reloadAll();
            onChangeTheme(ChangedItem::Theme);
            if (!my_module->other_fancy) {
                auto panel = dynamic_cast<SvgThemePanel<FancySvg>*>(getPanel());
                auto bounds = makeBounds(panel->svg, "k:", true);
                positionWidgets(pos_widgets, bounds);
            }
            sendDirty(this);
        }
    } break;
}
#endif
    Base::onHoverKey(e);
}

void FancyUi::step() {
    Base::step();
    if (!my_module) return;

    if (request_cloak) {
        request_cloak = false;
        fancy_background(my_module->fancy);
   }
   theme_holder->pollRackThemeChanged();
   sync_latch_state();
}

void FancyUi::draw(const DrawArgs& args) {
    Base::draw(args);
    if (my_module && my_module->other_fancy) {
        draw_disabled_panel(this, GetPreferredTheme(theme_holder), args, 20.f, 20.f);
        return;
    }
 }

void FancyUi::appendContextMenu(Menu* menu) {
    if (!module) return;

    menu->addChild(createMenuLabel<HamburgerTitle>("#d FancyBox"));
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
}
