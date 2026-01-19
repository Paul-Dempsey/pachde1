#pragma once
#include <rack.hpp>
using namespace ::rack;
#include "../Fancy.hpp"
#include "widgets/close-button.hpp"

namespace pachde {

void show_picture_dialog(ModuleWidget* source, Fancy* module, Theme theme);
void show_fill_dialog(ModuleWidget* source, Fancy* module, Theme theme);
void show_linear_gradient_dialog(ModuleWidget* source, Fancy* module, Theme theme);
void show_radial_gradient_dialog(ModuleWidget* source, Fancy* module, Theme theme);
void show_box_gradient_dialog(ModuleWidget* source, Fancy* module, Theme theme);
void show_skiff_dialog(ModuleWidget* source, Fancy* module, Theme theme);

struct DialogStyles {
    LabelStyle* title_style{nullptr};
    LabelStyle* center_label_style{nullptr};
    LabelStyle* left_label_style{nullptr};
    LabelStyle* info_label_style{nullptr};

    void createStyles(std::shared_ptr<svg_theme::SvgTheme> svg_theme) {
        title_style = new LabelStyle("dlg-title", colors::Black, 14.f, true);
        title_style->valign = VAlign::Top;
        title_style->halign = HAlign::Left;

        center_label_style = new LabelStyle("dlg-label");
        center_label_style->halign = HAlign::Center;
        left_label_style = new LabelStyle("dlg-label");
        left_label_style->halign = HAlign::Left;

        info_label_style = new LabelStyle("dlg-info", colors::PortCorn, 12.f);

        title_style->applyTheme(svg_theme);
        center_label_style->applyTheme(svg_theme);
        left_label_style->applyTheme(svg_theme);
        info_label_style->applyTheme(svg_theme);
    }
};

template <typename TDialog>
void add_close_button(TDialog* dlg, ::svg_query::BoundsIndex bounds, const char* key, std::shared_ptr<SvgTheme> svg_theme) {
    auto close = createWidgetCentered<CloseButton>(bounds[key].getCenter());
    close->set_handler([=](){ dlg->close(); } );
    close->applyTheme(svg_theme);
    dlg->addChild(close);
}

template <typename TDialog>
void add_knob(TDialog* dlg, ::svg_query::BoundsIndex &bounds, const char *key, Module* module, int param) {
    dlg->addChild(createParamCentered<RoundBlackKnob>(bounds[key].getCenter(), module, param));
}

template <typename TDialog>
void add_check(TDialog* dlg, ::svg_query::BoundsIndex &bounds, const char *key,
    Module* module, int param, std::shared_ptr<svg_theme::SvgTheme> svg_theme
) {
    auto check = Center(createThemeParamButton<CheckButton>(
        &dlg->my_svgs,
        bounds[key].getCenter(),
        module,
        param,
        svg_theme
    ));
    dlg->addChild(check);
}

template <typename TDialog>
TextLabel* add_label(
    TDialog* dlg,
    ::svg_query::BoundsIndex &bounds,
    const char *key,
    const char *text,
    LabelStyle* style,
    std::shared_ptr<svg_theme::SvgTheme> svg_theme
) {
    auto label = TextLabel::createLabel(bounds[key], text, style, svg_theme);
    dlg->addChild(label);
    return label;
}
}