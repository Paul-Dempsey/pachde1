#include "fancy-dialogs.hpp"
#include "services/svg-query.hpp"
#include "services/colors.hpp"
#include "widgets/close-button.hpp"
#include "widgets/color-picker.hpp"
#define DIALOG_THEMED
#include "widgets/dialog.hpp"

using namespace ::widgetry;
namespace pachde {

struct FillDialogSvg {
    static std::string background() {
        return asset::plugin(pluginInstance, "res/dialogs/fancy-fill.svg");
    }
};

struct FillDialog : SvgDialog<FillDialogSvg> {
    using Base = SvgDialog<FillDialogSvg>;

    SvgCache my_svgs;
    Fancy * fancy_module{nullptr};
    DialogStyles styles;
    FancySwatch* fill_swatch{nullptr};

    FillDialog(ModuleWidget* source, ILoadSvg* svg_loader) :
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
        addChild(TextLabel::createLabel(bounds["k:dlg-title"], "FancyBox | Tone options", styles.title_style));

        add_check(this, bounds, "k:fill-check", fancy_module, Fancy::P_FANCY_FILL_ON, svg_theme);
        auto palette = Center(createThemeSvgButton<Palette20ActionButton>(&my_svgs, bounds["k:fill-co"].getCenter()));
        palette->describe("Tone color");
        if (fancy_module) {
            palette->set_handler([=](bool,bool) {
                auto picker = new ColorPickerMenu();
                createMenu()->addChild(picker);
                picker->set_color(fancy_module->fancy_data.fill.color);
                picker->set_on_new_color([=](PackedColor color) { set_fill_color(color); });
            });
        }
        addChild(palette);

        addChild(fill_swatch = new FancySwatch(bounds["k:fill-swatch"], fancy_module->fancy_data.fill.color));

        add_knob(this, bounds, "k:fill-fade", fancy_module, Fancy::P_FANCY_FILL_FADE);
        add_label(this, bounds, "k:fill-fade-label", "FADE", styles.center_label_style, svg_theme);
    }

    void set_fill_color(PackedColor color) {
        fill_swatch->set_color(color);
        if (fancy_module) {
            if (fancy_module->my_cloak) {
                fancy_module->my_cloak->data.fill.color = color;
            }
            fancy_module->fancy_data.fill.color = color;
        }
    }

};

void show_fill_dialog(ModuleWidget* source, Fancy* module, Theme theme) {
    SvgNoCache nocache;
    auto dlg = createDialog<FillDialog, ModuleWidget>(source, Vec(source->box.size.x *.5, source->box.size.y *.5 - 15.f), &nocache, true);
    dlg->set_module(module);
    dlg->create_ui(theme);
}

}