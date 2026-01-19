#include "fancy-dialogs.hpp"
#include "services/svg-query.hpp"
#include "services/colors.hpp"
#include "widgets/close-button.hpp"
#include "widgets/color-picker.hpp"
#define DIALOG_THEMED
#include "widgets/dialog.hpp"

using namespace ::widgetry;
namespace pachde {
struct BoxGradientSvg {
    static std::string background() {
        return asset::plugin(pluginInstance, "res/dialogs/fancy-box-gradient.svg");
    }
};

struct BoxGradientDialog : SvgDialog<BoxGradientSvg> {
    using Base = SvgDialog<BoxGradientSvg>;

    SvgCache my_svgs;
    Fancy * fancy_module{nullptr};
    DialogStyles styles;
    FancySwatch* bg_inner_swatch{nullptr};
    FancySwatch* bg_outer_swatch{nullptr};

    BoxGradientDialog(ModuleWidget* source, ILoadSvg* svg_loader) :
        Base(source, svg_loader)
    {
    }
    void set_module(Fancy* module) { fancy_module = module; }
    void create_ui(Theme theme) {
        assert(fancy_module); // must set_module before creating UI
        auto svg_theme = getThemeCache().getTheme(ThemeName(theme));
        auto layout = Base::get_svg();
        applySvgTheme(layout, svg_theme);
        styles.createStyles(svg_theme);
        ::svg_query::BoundsIndex bounds;
        svg_query::addBounds(layout, "k:", bounds, true);

        add_close_button(this, bounds, "k:close", svg_theme);
        addChild(TextLabel::createLabel(bounds["k:dlg-title"], "FancyBox | Box gradient options", styles.title_style));

        add_check(this, bounds, "k:bg-check", fancy_module, Fancy::P_FANCY_BOX_ON, svg_theme);
        add_label(this, bounds, "k:bg-check-label", "Enable Box gradient", styles.left_label_style, svg_theme);
        auto palette = Center(createThemeSvgButton<Palette20ActionButton>(&my_svgs, bounds["k:bg-inner-co"].getCenter()));
        palette->describe("Box gradient inner color");
        palette->set_handler([=](bool,bool) {
            auto picker = new ColorPickerMenu();
            createMenu()->addChild(picker);
            picker->set_color(fancy_module->fancy_data.boxg.icol);
            picker->set_on_new_color([=](PackedColor color) { set_bg_inner_color(color); });
        });
        addChild(palette);
        addChild(bg_inner_swatch = new FancySwatch(bounds["k:bg-inner-swatch"], fancy_module->fancy_data.boxg.icol));
        add_knob(this, bounds, "k:bg-inner-fade", fancy_module, Fancy::P_FANCY_BOX_INNER_FADE);
        add_knob(this, bounds, "k:bg-shrink-x", fancy_module, Fancy::P_FANCY_BOX_SHRINK_X);
        add_knob(this, bounds, "k:bg-shrink-y", fancy_module, Fancy::P_FANCY_BOX_SHRINK_Y);
        add_label(this, bounds, "k:bg-inner-fade-label", "FADE", styles.center_label_style, svg_theme);
        add_label(this, bounds, "k:bg-shrink-x-label", "X", styles.center_label_style, svg_theme);
        add_label(this, bounds, "k:bg-shrink-label", "shrink", styles.center_label_style, svg_theme);
        add_label(this, bounds, "k:bg-shrink-y-label", "Y", styles.center_label_style, svg_theme);

        palette = Center(createThemeSvgButton<Palette20ActionButton>(&my_svgs, bounds["k:bg-outer-co"].getCenter()));
        palette->describe("Box gradient outer color");
        palette->set_handler([=](bool,bool) {
            auto picker = new ColorPickerMenu();
            createMenu()->addChild(picker);
            picker->set_color(fancy_module->fancy_data.boxg.ocol);
            picker->set_on_new_color([=](PackedColor color) { set_bg_outer_color(color); });
        });
        addChild(palette);
        addChild(bg_outer_swatch = new FancySwatch(bounds["k:bg-outer-swatch"], fancy_module->fancy_data.boxg.ocol));
        add_knob(this, bounds, "k:bg-outer-fade", fancy_module, Fancy::P_FANCY_BOX_OUTER_FADE);
        add_knob(this, bounds, "k:bg-radius", fancy_module, Fancy::P_FANCY_BOX_RADIUS);
        add_knob(this, bounds, "k:bg-feather", fancy_module, Fancy::P_FANCY_BOX_FEATHER);

        add_label(this, bounds, "k:bg-outer-fade-label", "FADE", styles.center_label_style, svg_theme);
        add_label(this, bounds, "k:bg-radius-label", "RADIUS", styles.center_label_style, svg_theme);
        add_label(this, bounds, "k:bg-feather-label", "FEATHER", styles.center_label_style, svg_theme);

    }
    void set_bg_inner_color(PackedColor color) {
        bg_inner_swatch->set_color(color);
        if (fancy_module) {
            if (fancy_module->my_cloak) {
                fancy_module->my_cloak->data.boxg.icol = color;
            }
            fancy_module->fancy_data.boxg.icol = color;
        }
    }

    void set_bg_outer_color(PackedColor color) {
        bg_outer_swatch->set_color(color);
        if (fancy_module) {
            if (fancy_module->my_cloak) {
                fancy_module->my_cloak->data.boxg.ocol = color;
            }
            fancy_module->fancy_data.boxg.ocol = color;
        }
    }


};

void show_box_gradient_dialog(ModuleWidget* source, Fancy* module, Theme theme) {
    SvgNoCache nocache;
    auto dlg = createDialog<BoxGradientDialog, ModuleWidget>(source, Vec(source->box.size.x *.5, source->box.size.y *.5 - 15.f), &nocache, true);
    dlg->set_module(module);
    dlg->create_ui(theme);
}

}