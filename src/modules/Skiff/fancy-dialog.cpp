#include "fancy-dialog.hpp"
using namespace widgetry;
namespace pachde {

void pre_cache_fancy_dialog_svg(ILoadSvg* loader) {
    loader->loadSvg(FancySvg::background());
}

FancyDialog * show_fancy_dialog(SkiffUi *ui) {
    auto result = createDialog<FancyDialog>(ui, Vec(ui->box.size.x, 0.f), &ui->my_svgs);
    result->make_ui();
    return result;
}

FancyDialog::FancyDialog(SkiffUi *src, ILoadSvg* loader) :
    Base(src, loader), skiff(src)
{}

void FancyDialog::add_knob(::svg_query::BoundsIndex &bounds, const char *key, int param) {
    addChild(createParamCentered<RoundBlackKnob>(bounds[key].getCenter(), skiff->my_module, param));
}

void FancyDialog::add_label(::svg_query::BoundsIndex &bounds, const char *key, const char *text, LabelStyle* style, std::shared_ptr<svg_theme::SvgTheme> svg_theme){
    addChild(TextLabel::createLabel(bounds[key], text, style, svg_theme));
}

void FancyDialog::make_ui() {
    auto svg_theme = getThemeCache().getTheme(ThemeName(skiff->theme_holder->getTheme()));
    auto layout = get_svg();
    ::svg_query::BoundsIndex bounds;
    svg_query::addBounds(layout, "k:", bounds, true);

    title_style = new LabelStyle("fancy-title", colors::Black, 14.f, true);
    title_style->valign = VAlign::Top;
    title_style->applyTheme(svg_theme);

    group_style = new LabelStyle("fancy-group", "hsl(42, .5, .6)", 14.f, false);
    group_style->applyTheme(svg_theme);

    label_style = new LabelStyle("fancy-label");
    label_style->applyTheme(svg_theme);

    auto close = createWidgetCentered<CloseButton>(bounds["k:close"].getCenter());
    close->set_handler([=](){ Base::close(); } );
    close->applyTheme(svg_theme);
    addChild(close);

    add_label(bounds, "k:fancy-title", "FancyBox Options", title_style, svg_theme);

// "k:fill-check", Skiff::P_FANCY_FILL_ON,
//
    add_label(bounds, "k:fill-title", "Fill", group_style, svg_theme);

    auto palette = Center(createThemeSvgButton<Palette20ActionButton>(load_svg, bounds["k:fill-co"].getCenter()));
    palette->describe("Fill color");
    palette->set_handler([=](bool,bool) {
        auto picker = new ColorPickerMenu();
        picker->set_color(skiff->my_module->fancy_data.fill.color);
        picker->set_on_new_color([=](PackedColor color) {
            skiff->my_module->fancy_data.fill.color = color;
        });
        auto menu = createMenu();
        menu->addChild(picker);
    });
    addChild(palette);

    add_knob(bounds, "k:fill-fade-knob", Skiff::P_FANCY_FILL_FADE);
    add_label(bounds, "k:fill-fade", "Fade", label_style, svg_theme);

// k:lg-check", Skiff::P_FANCY_LINEAR_ON,
    add_label(bounds, "k:lg-title", "Linear Gradient", group_style, svg_theme);
// "k:lg-start-co"
    add_knob(bounds, "k:lg-start-fade-knob", Skiff::P_FANCY_LINEAR_START_FADE);
    add_knob(bounds, "k:lg-start-x-knob", Skiff::P_FANCY_LINEAR_X1);
    add_knob(bounds, "k:lg-start-y-knob", Skiff::P_FANCY_LINEAR_Y1);
    add_label(bounds, "k:lg-start-label", "start", label_style, svg_theme);
    add_label(bounds, "k:lg-fade-label", "fade", label_style, svg_theme);
    add_label(bounds, "k:lg-x1-label", "x1", label_style, svg_theme);
    add_label(bounds, "k:lg-spos-label", "start", label_style, svg_theme);
    add_label(bounds, "k:lg-y1-label", "y1", label_style, svg_theme);

// "k:lg-end-co"
    add_knob(bounds, "k:lg=end-fade-knob", Skiff::P_FANCY_LINEAR_END_FADE);
    add_knob(bounds, "k:lg-end-x-knob", Skiff::P_FANCY_LINEAR_X2);
    add_knob(bounds, "k:lg-end-y-knob", Skiff::P_FANCY_LINEAR_Y2);
    add_label(bounds, "k:lg-end-label", "end", label_style, svg_theme);
    add_label(bounds, "k:lg-efade-label", "fade", label_style, svg_theme);
    add_label(bounds, "k:lg-x2-label", "x2", label_style, svg_theme);
    add_label(bounds, "k:lg-epos-label", "end", label_style, svg_theme);
    add_label(bounds, "k:lg-y2-label", "y2", label_style, svg_theme);

// k:rg-check", Skiff::P_FANCY_RADIAL_ON
    add_label(bounds, "k:rg-title", "Radial Gradient", group_style, svg_theme);
// "k:rg.start-co"
    add_knob(bounds, "k:rg-inner-fade-knob", Skiff::P_FANCY_RADIAL_INNER_FADE);
    add_knob(bounds, "k:rg-cx-knob", Skiff::P_FANCY_RADIAL_CX);
    add_knob(bounds, "k:rg-cy-knob", Skiff::P_FANCY_RADIAL_CY);
    add_label(bounds, "k:rg-inner-label", "inner", label_style, svg_theme);
    add_label(bounds, "k:rg-innerfade-label", "fade", label_style, svg_theme);
    add_label(bounds, "k:rg-cx-label", "cx", label_style, svg_theme);
    add_label(bounds, "k:rg-cy-label", "cy", label_style, svg_theme);

// "k:rg-end-co"
    add_knob(bounds, "k:rg-outer-fade-knob", Skiff::P_FANCY_RADIAL_OUTER_FADE);
    add_knob(bounds, "k:rg-radius-knob", Skiff::P_FANCY_RADIAL_RADIUS);
    add_label(bounds, "k:rg-outer-label", "outer", label_style, svg_theme);
    add_label(bounds, "k:rg-ofade-label", "fade", label_style, svg_theme);
    add_label(bounds, "k:rg-radius-label", "radius", label_style, svg_theme);

// "k:bg-check", Skiff::P_FANCY_BOX_ON);
    add_label(bounds, "k:bg-title", "Box gradient", group_style, svg_theme);
// "k:bg-inner-co"
    add_knob(bounds, "k:bg-inner-fade-knob", Skiff::P_FANCY_BOX_INNER_FADE);
    add_knob(bounds, "k:bg-shrink-x-knob", Skiff::P_FANCY_BOX_SHRINK_X);
    add_knob(bounds, "k:bg-shrink-y-knob", Skiff::P_FANCY_BOX_SHRINK_Y);
    add_label(bounds, "k:bg-inner-label", "inner", label_style, svg_theme);
    add_label(bounds, "k:bg-innerfade-label", "fade", label_style, svg_theme);
    add_label(bounds, "k:bg-cx-label", "cx", label_style, svg_theme);
    add_label(bounds, "k:bg-shrink-label", "shrink", label_style, svg_theme);
    add_label(bounds, "k:bg-cy-label", "cy", label_style, svg_theme);
// "k:bg-outer-co"
    add_knob(bounds, "k:bg-outer-fade-knob", Skiff::P_FANCY_BOX_OUTER_FADE);
    add_knob(bounds, "k:bg-radius-knob", Skiff::P_FANCY_BOX_RADIUS);
    add_knob(bounds, "k:bg-feather-knob", Skiff::P_FANCY_BOX_FEATHER);

    add_label(bounds, "k:bg-outer-label", "outer", label_style, svg_theme);
    add_label(bounds, "k:bg-ofade-label", "fade", label_style, svg_theme);
    add_label(bounds, "k:bg-radius-label", "radius", label_style, svg_theme);
    add_label(bounds, "k:bg-feather-label", "feather", label_style, svg_theme);

}

}