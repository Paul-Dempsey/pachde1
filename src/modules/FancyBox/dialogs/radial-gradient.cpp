#include "fancy-dialogs.hpp"
#include "services/svg-query.hpp"
#include "services/colors.hpp"
#include "widgets/close-button.hpp"
#include "widgets/color-picker.hpp"
#define DIALOG_THEMED
#include "widgets/dialog.hpp"

using namespace ::widgetry;
namespace pachde {

struct RadialGradientSvg {
    static std::string background() {
        return asset::plugin(pluginInstance, "res/dialogs/fancy-radial.svg");
    }
};

struct RadialGradientDialog : SvgDialog<RadialGradientSvg> {
    using Base = SvgDialog<RadialGradientSvg>;

    SvgCache my_svgs;
    Fancy * fancy_module{nullptr};
    DialogStyles styles;
    FancySwatch* rg_inner_swatch{nullptr};
    FancySwatch* rg_outer_swatch{nullptr};

    RadialGradientDialog(ModuleWidget* source, ILoadSvg* svg_loader) :
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
        addChild(TextLabel::createLabel(bounds["k:dlg-title"], "FancyBox | Radial gradient options", styles.title_style));

        add_check(this, bounds, "k:rg-check", fancy_module, Fancy::P_FANCY_RADIAL_ON, svg_theme);
        add_label(this, bounds, "k:rg-check-label", "Enable Radial gradient", styles.left_label_style, svg_theme);

        auto palette = Center(createThemeSvgButton<Palette20ActionButton>(&my_svgs, bounds["k:rg-inner-co"].getCenter()));
        palette->describe("Radial gradient inner color");
        if (fancy_module) {
            palette->set_handler([=](bool,bool) {
                auto picker = new ColorPickerMenu();
                createMenu()->addChild(picker);
                picker->set_color(fancy_module->fancy_data.linear.icol);
                picker->set_on_new_color([=](PackedColor color) { set_rg_inner_color(color); });
            });
        }
        addChild(palette);
        add_knob(this, bounds, "k:rg-inner-fade", fancy_module, Fancy::P_FANCY_RADIAL_INNER_FADE);
        add_knob(this, bounds, "k:rg-cx", fancy_module, Fancy::P_FANCY_RADIAL_CX);
        add_knob(this, bounds, "k:rg-cy", fancy_module, Fancy::P_FANCY_RADIAL_CY);
        add_label(this, bounds, "k:rg-inner-fade-label", "FADE", styles.center_label_style, svg_theme);
        add_label(this, bounds, "k:rg-cx-label", "CX", styles.center_label_style, svg_theme);
        add_label(this, bounds, "k:rg-cy-label", "CY", styles.center_label_style, svg_theme);
        palette = Center(createThemeSvgButton<Palette20ActionButton>(&my_svgs, bounds["k:rg-outer-co"].getCenter()));
        palette->describe("Radial gradient outer color");
        palette->set_handler([=](bool,bool) {
            auto picker = new ColorPickerMenu();
            createMenu()->addChild(picker);
            picker->set_color(fancy_module->fancy_data.radial.ocol);
            picker->set_on_new_color([=](PackedColor color) { set_rg_outer_color(color); });
        });
        addChild(palette);
        add_knob(this, bounds, "k:rg-outer-fade", fancy_module, Fancy::P_FANCY_RADIAL_OUTER_FADE);
        add_knob(this, bounds, "k:rg-radius", fancy_module, Fancy::P_FANCY_RADIAL_RADIUS);
        add_label(this, bounds, "k:rg-outer-fade-label", "FADE", styles.center_label_style, svg_theme);
        add_label(this, bounds, "k:rg-radius-label", "RADIUS", styles.center_label_style, svg_theme);

        addChild(rg_inner_swatch = new FancySwatch(bounds["k:rg-inner-swatch"], fancy_module->fancy_data.radial.icol));
        addChild(rg_outer_swatch = new FancySwatch(bounds["k:rg-outer-swatch"], fancy_module->fancy_data.radial.ocol));
    }

    void set_rg_inner_color(PackedColor color) {
        rg_inner_swatch->set_color(color);
        if (fancy_module) {
            if (fancy_module->my_cloak) {
                fancy_module->my_cloak->data.radial.icol = color;
            }
            fancy_module->fancy_data.radial.icol = color;
        }
    }

    void set_rg_outer_color(PackedColor color) {
        rg_outer_swatch->set_color(color);
        if (fancy_module) {
            if (fancy_module->my_cloak) {
                fancy_module->my_cloak->data.radial.ocol = color;
            }
            fancy_module->fancy_data.radial.ocol = color;
        }
    }
};

void show_radial_gradient_dialog(ModuleWidget* source, Fancy* module, Theme theme) {
    SvgNoCache nocache;
    auto dlg = createDialog<RadialGradientDialog, ModuleWidget>(source, Vec(source->box.size.x *.5, source->box.size.y *.5 - 15.f), &nocache, true);
    dlg->set_module(module);
    dlg->create_ui(theme);
}

}