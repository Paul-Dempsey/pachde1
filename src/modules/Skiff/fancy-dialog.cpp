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

void FancyDialog::add_check(::svg_query::BoundsIndex &bounds, const char *key, int param, std::shared_ptr<svg_theme::SvgTheme> svg_theme)
{
    addChild(Center(createThemeParamButton<CheckButton>(load_svg, bounds[key].getCenter(), skiff->my_module, param, svg_theme)));
}

void FancyDialog::make_ui() {
    auto svg_theme = getThemeCache().getTheme(ThemeName(skiff->theme_holder->getTheme()));
    auto layout = get_svg();
    ::svg_query::BoundsIndex bounds;
    svg_query::addBounds(layout, "k:", bounds, true);

    title_style = new LabelStyle("fancy-title", colors::Black, 14.f, true);
    title_style->valign = VAlign::Top;
    title_style->applyTheme(svg_theme);

    group_style = new LabelStyle("fancy-group", "hsl(42, .5, .6)", 12.f, false);
    group_style->orientation = Orientation::Up;
    group_style->halign = HAlign::Center;
    group_style->valign = VAlign::Middle;
    group_style->applyTheme(svg_theme);

    label_style = new LabelStyle("fancy-label");
    label_style->applyTheme(svg_theme);

    auto close = createWidgetCentered<CloseButton>(bounds["k:close"].getCenter());
    close->set_handler([=](){ Base::close(); } );
    close->applyTheme(svg_theme);
    addChild(close);

    //add_label(bounds, "k:fancy-title", "FancyBox Options", title_style, svg_theme);

    add_check(bounds, "k:fill-check", Skiff::P_FANCY_FILL_ON, svg_theme);
    add_label(bounds, "k:fill-group", "FILL", group_style, svg_theme);
    auto palette = Center(createThemeSvgButton<Palette20ActionButton>(load_svg, bounds["k:fill-co"].getCenter()));
    palette->describe("Fill color");
    palette->set_handler([=](bool,bool) {
        auto picker = new ColorPickerMenu();
        picker->set_color(skiff->my_module->fancy_data.fill.color);
        picker->set_on_new_color([=](PackedColor color) {
            skiff->my_module->fancy_data.fill.color = color;
        });
        createMenu()->addChild(picker);
    });
    addChild(palette);
    add_knob(bounds, "k:fill-fade", Skiff::P_FANCY_FILL_FADE);
    add_label(bounds, "k:fill-fade-label", "FADE", label_style, svg_theme);

    add_check(bounds, "k:lg-check", Skiff::P_FANCY_LINEAR_ON, svg_theme);
    add_label(bounds, "k:lg-group", "LINEAR GRADIENT", group_style, svg_theme);
    palette = Center(createThemeSvgButton<Palette20ActionButton>(load_svg, bounds["k:lg-start-co"].getCenter()));
    palette->describe("Linear gradient start color");
    palette->set_handler([=](bool,bool) {
        auto picker = new ColorPickerMenu();
        picker->set_color(skiff->my_module->fancy_data.linear.icol);
        picker->set_on_new_color([=](PackedColor color) { skiff->my_module->fancy_data.linear.icol = color; });
        createMenu()->addChild(picker);
    });
    addChild(palette);
    add_knob(bounds, "k:lg-start-fade", Skiff::P_FANCY_LINEAR_START_FADE);
    add_knob(bounds, "k:lg-start-x", Skiff::P_FANCY_LINEAR_X1);
    add_knob(bounds, "k:lg-start-y", Skiff::P_FANCY_LINEAR_Y1);
    // add_label(bounds, "k:lg-start-label", "start", label_style, svg_theme);
    add_label(bounds, "k:lg-start-fade-label", "FADE", label_style, svg_theme);
    add_label(bounds, "k:lg-x1-label", "X1", label_style, svg_theme);
    // add_label(bounds, "k:lg-spos-label", "start", label_style, svg_theme);
    add_label(bounds, "k:lg-y1-label", "Y1", label_style, svg_theme);

    palette = Center(createThemeSvgButton<Palette20ActionButton>(load_svg, bounds["k:lg-end-co"].getCenter()));
    palette->describe("Linear gradient end color");
    palette->set_handler([=](bool,bool) {
        auto picker = new ColorPickerMenu();
        picker->set_color(skiff->my_module->fancy_data.linear.ocol);
        picker->set_on_new_color([=](PackedColor color) { skiff->my_module->fancy_data.linear.ocol = color; });
        createMenu()->addChild(picker);
    });
    addChild(palette);
    add_knob(bounds, "k:lg-end-fade", Skiff::P_FANCY_LINEAR_END_FADE);
    add_knob(bounds, "k:lg-end-x", Skiff::P_FANCY_LINEAR_X2);
    add_knob(bounds, "k:lg-end-y", Skiff::P_FANCY_LINEAR_Y2);
    // add_label(bounds, "k:lg-end-label", "end", label_style, svg_theme);
    add_label(bounds, "k:lg-end-fade-label", "FADE", label_style, svg_theme);
    add_label(bounds, "k:lg-x2-label", "X2", label_style, svg_theme);
    // add_label(bounds, "k:lg-epos-label", "end", label_style, svg_theme);
    add_label(bounds, "k:lg-y2-label", "Y2", label_style, svg_theme);

    add_check(bounds, "k:rg-check", Skiff::P_FANCY_RADIAL_ON, svg_theme);
    add_label(bounds, "k:rg-group", "RADIAL GRADIENT", group_style, svg_theme);
    palette = Center(createThemeSvgButton<Palette20ActionButton>(load_svg, bounds["k:rg-inner-co"].getCenter()));
    palette->describe("Radial gradient inner color");
    palette->set_handler([=](bool,bool) {
        auto picker = new ColorPickerMenu();
        picker->set_color(skiff->my_module->fancy_data.radial.icol);
        picker->set_on_new_color([=](PackedColor color) { skiff->my_module->fancy_data.radial.icol = color; });
        createMenu()->addChild(picker);
    });
    addChild(palette);
    add_knob(bounds, "k:rg-inner-fade", Skiff::P_FANCY_RADIAL_INNER_FADE);
    add_knob(bounds, "k:rg-cx", Skiff::P_FANCY_RADIAL_CX);
    add_knob(bounds, "k:rg-cy", Skiff::P_FANCY_RADIAL_CY);
    // add_label(bounds, "k:rg-inner-label", "inner", label_style, svg_theme);
    add_label(bounds, "k:rg-inner-fade-label", "FADE", label_style, svg_theme);
    add_label(bounds, "k:rg-cx-label", "CX", label_style, svg_theme);
    add_label(bounds, "k:rg-cy-label", "CY", label_style, svg_theme);
    palette = Center(createThemeSvgButton<Palette20ActionButton>(load_svg, bounds["k:rg-outer-co"].getCenter()));
    palette->describe("Radial gradient outer color");
    palette->set_handler([=](bool,bool) {
        auto picker = new ColorPickerMenu();
        picker->set_color(skiff->my_module->fancy_data.radial.ocol);
        picker->set_on_new_color([=](PackedColor color) { skiff->my_module->fancy_data.radial.ocol = color; });
        createMenu()->addChild(picker);
    });
    addChild(palette);
    add_knob(bounds, "k:rg-outer-fade", Skiff::P_FANCY_RADIAL_OUTER_FADE);
    add_knob(bounds, "k:rg-radius", Skiff::P_FANCY_RADIAL_RADIUS);
    // add_label(bounds, "k:rg-outer-label", "outer", label_style, svg_theme);
    add_label(bounds, "k:rg-outer-fade-label", "FADE", label_style, svg_theme);
    add_label(bounds, "k:rg-radius-label", "RADIUS", label_style, svg_theme);

    add_check(bounds, "k:bg-check", Skiff::P_FANCY_BOX_ON, svg_theme);
    add_label(bounds, "k:bg-group", "BOX GRADIENT", group_style, svg_theme);
    palette = Center(createThemeSvgButton<Palette20ActionButton>(load_svg, bounds["k:bg-inner-co"].getCenter()));
    palette->describe("Box gradient inner color");
    palette->set_handler([=](bool,bool) {
        auto picker = new ColorPickerMenu();
        picker->set_color(skiff->my_module->fancy_data.boxg.icol);
        picker->set_on_new_color([=](PackedColor color) { skiff->my_module->fancy_data.boxg.icol = color; });
        createMenu()->addChild(picker);
    });
    addChild(palette);
    add_knob(bounds, "k:bg-inner-fade", Skiff::P_FANCY_BOX_INNER_FADE);
    add_knob(bounds, "k:bg-shrink-x", Skiff::P_FANCY_BOX_SHRINK_X);
    add_knob(bounds, "k:bg-shrink-y", Skiff::P_FANCY_BOX_SHRINK_Y);
    // add_label(bounds, "k:bg-inner-label", "inner", label_style, svg_theme);
    add_label(bounds, "k:bg-inner-fade-label", "FADE", label_style, svg_theme);
    add_label(bounds, "k:bg-shrink-x-label", "X", label_style, svg_theme);
    add_label(bounds, "k:bg-shrink-label", "shrink", label_style, svg_theme);
    add_label(bounds, "k:bg-shrink-y-label", "Y", label_style, svg_theme);
    palette = Center(createThemeSvgButton<Palette20ActionButton>(load_svg, bounds["k:bg-outer-co"].getCenter()));
    palette->describe("Box gradient outer color");
    palette->set_handler([=](bool,bool) {
        auto picker = new ColorPickerMenu();
        picker->set_color(skiff->my_module->fancy_data.boxg.ocol);
        picker->set_on_new_color([=](PackedColor color) { skiff->my_module->fancy_data.boxg.ocol = color; });
        createMenu()->addChild(picker);
    });
    addChild(palette);
    add_knob(bounds, "k:bg-outer-fade", Skiff::P_FANCY_BOX_OUTER_FADE);
    add_knob(bounds, "k:bg-radius", Skiff::P_FANCY_BOX_RADIUS);
    add_knob(bounds, "k:bg-feather", Skiff::P_FANCY_BOX_FEATHER);

    // add_label(bounds, "k:bg-outer-label", "outer", label_style, svg_theme);
    add_label(bounds, "k:bg-outer-fade-label", "FADE", label_style, svg_theme);
    add_label(bounds, "k:bg-radius-label", "RADIUS", label_style, svg_theme);
    add_label(bounds, "k:bg-feather-label", "FEATHER", label_style, svg_theme);

}

}