#include "fancy-dialogs.hpp"
#include "services/svg-query.hpp"
#include "services/colors.hpp"
#include "widgets/close-button.hpp"
#include "widgets/color-picker.hpp"
#define DIALOG_THEMED
#include "widgets/dialog.hpp"

using namespace ::widgetry;
namespace pachde {

struct LinearGradientSvg {
    static std::string background() {
        return asset::plugin(pluginInstance, "res/dialogs/fancy-linear.svg");
    }
};

struct LinearGradientDialog : SvgDialog<LinearGradientSvg> {
    using Base = SvgDialog<LinearGradientSvg>;

    SvgCache my_svgs;
    Fancy * fancy_module{nullptr};
    DialogStyles styles;
    FancySwatch* lg_start_swatch{nullptr};
    FancySwatch* lg_end_swatch{nullptr};

    LinearGradientDialog(ModuleWidget* source, ILoadSvg* svg_loader) :
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
        addChild(TextLabel::createLabel(bounds["k:dlg-title"], "FancyBox | Linear gradient options", styles.title_style));

        add_check(this, bounds, "k:lg-check", fancy_module, Fancy::P_FANCY_LINEAR_ON, svg_theme);
        add_label(this, bounds, "k:lg-check-label", "Enable Linear gradient", styles.left_label_style, svg_theme);

        auto palette = Center(createThemeSvgButton<Palette20ActionButton>(&my_svgs, bounds["k:lg-start-co"].getCenter()));
        palette->describe("Linear gradient start color");
        if (fancy_module) {
            palette->set_handler([=](bool,bool) {
                auto picker = new ColorPickerMenu();
                createMenu()->addChild(picker);
                picker->set_color(fancy_module->fancy_data.linear.icol);
                picker->set_on_new_color([=](PackedColor color) { set_lg_start_color(color); });
            });
        }
        addChild(palette);
        addChild(lg_start_swatch = new FancySwatch(bounds["k:lg-start-swatch"], fancy_module->fancy_data.linear.icol));
        add_knob(this, bounds, "k:lg-start-fade", fancy_module, Fancy::P_FANCY_LINEAR_START_FADE);
        add_knob(this, bounds, "k:lg-start-x", fancy_module, Fancy::P_FANCY_LINEAR_X1);
        add_knob(this, bounds, "k:lg-start-y", fancy_module, Fancy::P_FANCY_LINEAR_Y1);
        add_label(this, bounds, "k:lg-start-fade-label", "FADE", styles.center_label_style, svg_theme);
        add_label(this, bounds, "k:lg-x1-label", "X1", styles.center_label_style, svg_theme);
        add_label(this, bounds, "k:lg-y1-label", "Y1", styles.center_label_style, svg_theme);

        palette = Center(createThemeSvgButton<Palette20ActionButton>(&my_svgs, bounds["k:lg-end-co"].getCenter()));
        palette->describe("Linear gradient end color");
        palette->set_handler([=](bool,bool) {
            auto picker = new ColorPickerMenu();
            createMenu()->addChild(picker);
            picker->set_color(fancy_module->fancy_data.linear.ocol);
            picker->set_on_new_color([=](PackedColor color) { set_lg_end_color(color); });
        });
        addChild(palette);
        addChild(lg_end_swatch = new FancySwatch(bounds["k:lg-end-swatch"], fancy_module->fancy_data.linear.ocol));
        add_knob(this, bounds, "k:lg-end-fade", fancy_module, Fancy::P_FANCY_LINEAR_END_FADE);
        add_knob(this, bounds, "k:lg-end-x", fancy_module, Fancy::P_FANCY_LINEAR_X2);
        add_knob(this, bounds, "k:lg-end-y", fancy_module, Fancy::P_FANCY_LINEAR_Y2);
        add_label(this, bounds, "k:lg-end-fade-label", "FADE", styles.center_label_style, svg_theme);
        add_label(this, bounds, "k:lg-x2-label", "X2", styles.center_label_style, svg_theme);
        add_label(this, bounds, "k:lg-y2-label", "Y2", styles.center_label_style, svg_theme);
    }

    void set_lg_start_color(PackedColor color) {
        lg_start_swatch->set_color(color);
        if (fancy_module) {
            if (fancy_module->my_cloak) {
                fancy_module->my_cloak->data.linear.icol = color;
            }
            fancy_module->fancy_data.linear.icol = color;
        }
    }

    void set_lg_end_color(PackedColor color) {
        lg_end_swatch->set_color(color);
        if (fancy_module) {
            if (fancy_module->my_cloak) {
                fancy_module->my_cloak->data.linear.ocol = color;
            }
            fancy_module->fancy_data.linear.ocol = color;
        }
    }
};

void show_linear_gradient_dialog(ModuleWidget* source, Fancy* module, Theme theme) {
    SvgNoCache nocache;
    auto dlg = createDialog<LinearGradientDialog, ModuleWidget>(source, Vec(source->box.size.x *.5, source->box.size.y *.5 - 15.f), &nocache, true);
    dlg->set_module(module);
    dlg->create_ui(theme);
}

}