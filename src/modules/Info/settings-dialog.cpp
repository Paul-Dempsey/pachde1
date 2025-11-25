#include "settings-dialog.hpp"
#define DIALOG_THEMED
#include "services/svg-query.hpp"
#include "widgets/dialog.hpp"
#include "widgets/action-button.hpp"
#include "widgets/close-button.hpp"
#include "widgets/hamburger.hpp"
#include "widgets/label.hpp"
using namespace widgetry;

namespace pachde {

struct SettingsDialogSvg {
    static std::string background() {
        return asset::plugin(pluginInstance, "res/dialogs/info-settings.svg");
    }
};

struct SettingsDialog : SvgDialog<SettingsDialogSvg> {
    using Base = SvgDialog<SettingsDialogSvg>;
    SvgNoCache nocache;
    InfoSettings* settings{nullptr};
    LabelStyle* title_style{nullptr};
    LabelStyle* center_label_style{nullptr};
    LabelStyle* up_label_style{nullptr};
    LabelStyle* down_label_style{nullptr};
    LabelStyle* inverted_label_style{nullptr};
    LabelStyle* left_label_style{nullptr};
    LabelStyle* right_label_style{nullptr};
    LabelStyle* info_label_style{nullptr};

    void createStyles(std::shared_ptr<svg_theme::SvgTheme> svg_theme) {
        title_style = new LabelStyle("dlg-title", colors::Black, 14.f, true);
        title_style->valign = VAlign::Top;
        title_style->halign = HAlign::Left;

        left_label_style = new LabelStyle("dlg-label");
        left_label_style->halign = HAlign::Left;

        center_label_style = new LabelStyle("dlg-label");
        center_label_style->halign = HAlign::Center;

        right_label_style = new LabelStyle("dlg-label");
        center_label_style->halign = HAlign::Right;

        up_label_style = new LabelStyle("dlg-label");
        up_label_style->orientation = Orientation::Up;
        up_label_style->halign = HAlign::Left;

        down_label_style = new LabelStyle("dlg-label");
        down_label_style->orientation = Orientation::Down;
        down_label_style->halign = HAlign::Left;

        inverted_label_style = new LabelStyle("dlg-label");
        inverted_label_style->orientation = Orientation::Inverted;
        inverted_label_style->halign = HAlign::Left;

        title_style->applyTheme(svg_theme);
        left_label_style->applyTheme(svg_theme);
        center_label_style->applyTheme(svg_theme);
        right_label_style->applyTheme(svg_theme);
        up_label_style->applyTheme(svg_theme);
        down_label_style->applyTheme(svg_theme);
        inverted_label_style->applyTheme(svg_theme);
    }

    SettingsDialog(InfoModuleWidget* src, ILoadSvg* loader) :
        Base(src, loader),
        settings(src->settings)
    {
        auto theme = src->theme_holder->getTheme();
        auto svg_theme = getThemeCache().getTheme(ThemeName(theme));
        auto layout = Base::get_svg();
        applySvgTheme(layout, svg_theme);
        createStyles(svg_theme);

        ::svg_query::BoundsIndex bounds;
        svg_query::addBounds(layout, "k:", bounds, true);

        auto close = createWidgetCentered<CloseButton>(bounds["k:close"].getCenter());
        close->set_handler([=](){ Base::close(); } );
        close->applyTheme(svg_theme);
        addChild(close);

        addChild(TextLabel::createLabel(bounds["k:dlg-title"], "Info | text options", title_style));
        addChild(TextLabel::createLabel(bounds["k:L-label"], "L", right_label_style));
        addChild(TextLabel::createLabel(bounds["k:R-label"], "R", left_label_style));
        addChild(TextLabel::createLabel(bounds["k:top-label"], "TOP", right_label_style));
        addChild(TextLabel::createLabel(bounds["k:mid-label"], "MIDDLE", right_label_style));
        addChild(TextLabel::createLabel(bounds["k:bot-label"], "BOTTOM", right_label_style));
        addChild(TextLabel::createLabel(bounds["k:left-label"], "LEFT", left_label_style));
        addChild(TextLabel::createLabel(bounds["k:center-label" ], "CENTER", left_label_style));
        addChild(TextLabel::createLabel(bounds["k:right-label"], "RIGHT", left_label_style));
        addChild(TextLabel::createLabel(bounds["k:up-label"], "UP", up_label_style));
        addChild(TextLabel::createLabel(bounds["k:down-label"], "DOWN", down_label_style));
        addChild(TextLabel::createLabel(bounds["k:normal-label"], "NORMAL", left_label_style));
        addChild(TextLabel::createLabel(bounds["k:invert-label"], "INVERTED", inverted_label_style));
        addChild(TextLabel::createLabel(bounds["k:panel-co-label"], "PANEL", center_label_style));
        addChild(TextLabel::createLabel(bounds["k:text-co-label"], "TEXT", center_label_style));
        addChild(TextLabel::createLabel(bounds["k:font-size-label"], "16px", center_label_style));

        auto info = TextLabel::createLabel(bounds["k:font-label"], "HankenGrotesk", nullptr);
        LabelStyle* style = info->create_owned_format();
        style->color = colors::PortCorn;
        style->bold = false;
        style->halign = HAlign::Left;
        style->key = "dlg-info";
        style->applyTheme(svg_theme);
        addChild(info);

    }
};

void show_settings_dialog(InfoModuleWidget* src, Vec pos) {
    SvgNoCache nocache;
    createDialog<SettingsDialog, InfoModuleWidget>(src, Vec(src->box.size.x, 0.f), &nocache, false);
}


}