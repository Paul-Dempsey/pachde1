#include "fancy-dialogs.hpp"
#include "services/svg-query.hpp"
#include "services/colors.hpp"
#include "widgets/close-button.hpp"
#include "widgets/color-picker.hpp"
#define DIALOG_THEMED
#include "widgets/dialog.hpp"

using namespace ::widgetry;
namespace pachde {

struct SkiffDialogSvg {
    static std::string background() {
        return asset::plugin(pluginInstance, "res/dialogs/fancy-skiff.svg");
    }
};

struct SkiffDialog : SvgDialog<SkiffDialogSvg> {
    using Base = SvgDialog<SkiffDialogSvg>;

    SvgCache my_svgs;
    Fancy * fancy_module{nullptr};
    DialogStyles styles;
    FancySwatch* edge_swatch{nullptr};
    FancySwatch* bezel_swatch{nullptr};
    FancySwatch* inside_swatch{nullptr};
    SkiffDialog(ModuleWidget* source, ILoadSvg* svg_loader) :
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
        addChild(TextLabel::createLabel(bounds["k:dlg-title"], "FancyBox | Skiff options", styles.title_style));

        add_check(this, bounds, "k:skiff-check", fancy_module, Fancy::P_FANCY_SKIFF_ON, svg_theme);
        add_label(this, bounds, "k:skiff-check-label", "Enable Skiff boxes", styles.left_label_style, svg_theme);

        // EDGE
        add_label(this, bounds, "k:skiff-edge-label", "EDGE", styles.center_label_style, svg_theme);
        auto palette = Center(createThemeSvgButton<Palette20ActionButton>(&my_svgs, bounds["k:skiff-edge-co"].getCenter()));
        palette->describe("Edge color");
        if (fancy_module) {
            palette->set_handler([=](bool,bool) {
                auto picker = new ColorPickerMenu();
                createMenu()->addChild(picker);
                picker->set_color(fancy_module->fancy_data.skiff.options.edge_color);
                picker->set_on_new_color([=](PackedColor color) { set_edge_color(color); });
            });
        }
        addChild(palette);
        addChild(edge_swatch = new FancySwatch(bounds["k:skiff-edge-swatch"], fancy_module->fancy_data.skiff.options.edge_color));
        add_knob(this, bounds, "k:skiff-edge", fancy_module, Fancy::P_FANCY_SKIFF_EDGE);

        // BEZEL
        add_label(this, bounds, "k:skiff-bezel-label", "BEZEL", styles.center_label_style, svg_theme);
        palette = Center(createThemeSvgButton<Palette20ActionButton>(&my_svgs, bounds["k:skiff-bezel-co"].getCenter()));
        palette->describe("Bezel color");
        if (fancy_module) {
            palette->set_handler([=](bool,bool) {
                auto picker = new ColorPickerMenu();
                createMenu()->addChild(picker);
                picker->set_color(fancy_module->fancy_data.skiff.options.bezel_color);
                picker->set_on_new_color([=](PackedColor color) { set_bezel_color(color); });
            });
        }
        addChild(palette);
        addChild(bezel_swatch = new FancySwatch(bounds["k:skiff-bezel-swatch"], fancy_module->fancy_data.skiff.options.bezel_color));
        add_knob(this, bounds, "k:skiff-bezel", fancy_module, Fancy::P_FANCY_SKIFF_BEZEL);

        // INSIDE
        add_label(this, bounds, "k:skiff-inside-label", "INSIDE", styles.center_label_style, svg_theme);
        palette = Center(createThemeSvgButton<Palette20ActionButton>(&my_svgs, bounds["k:skiff-inside-co"].getCenter()));
        palette->describe("Inside color");
        if (fancy_module) {
            palette->set_handler([=](bool,bool) {
                auto picker = new ColorPickerMenu();
                createMenu()->addChild(picker);
                picker->set_color(fancy_module->fancy_data.skiff.options.inside_color);
                picker->set_on_new_color([=](PackedColor color) { set_inside_color(color); });
            });
        }
        addChild(palette);
        addChild(inside_swatch = new FancySwatch(bounds["k:skiff-inside-swatch"], fancy_module->fancy_data.skiff.options.inside_color));

        add_check(this, bounds, "k:shadow-check", fancy_module, Fancy::P_FANCY_SKIFF_SHADOW, svg_theme);
        add_label(this, bounds, "k:shadow-check-label", "Shadow", styles.left_label_style, svg_theme);
    }

    void set_edge_color(PackedColor color) {
        edge_swatch->set_color(color);
        if (fancy_module) {
            fancy_module->fancy_data.skiff.options.edge_color = color;
            if (fancy_module->my_cloak) {
                fancy_module->my_cloak->data.skiff.options.edge_color = color;
            }
        }
    }

    void set_bezel_color(PackedColor color) {
        bezel_swatch->set_color(color);
        if (fancy_module) {
            fancy_module->fancy_data.skiff.options.bezel_color = color;
            if (fancy_module->my_cloak) {
                fancy_module->my_cloak->data.skiff.options.bezel_color = color;
            }
        }
    }
    void set_inside_color(PackedColor color) {
        inside_swatch->set_color(color);
        if (fancy_module) {
            fancy_module->fancy_data.skiff.options.inside_color = color;
            if (fancy_module->my_cloak) {
                fancy_module->my_cloak->data.skiff.options.inside_color = color;
            }
        }
    }
};

void show_skiff_dialog(ModuleWidget* source, Fancy* module, Theme theme) {
    SvgNoCache nocache;
    auto dlg = createDialog<SkiffDialog, ModuleWidget>(source, Vec(source->box.size.x *.5, source->box.size.y *.5 - 15.f), &nocache, true);
    dlg->set_module(module);
    dlg->create_ui(theme);
}

}