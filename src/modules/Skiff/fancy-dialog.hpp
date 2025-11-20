#pragma once
#include "Skiff.hpp"
#define DIALOG_THEMED
#include "widgets/dialog.hpp"
#include "services/svg-theme.hpp"
#include "services/svg-query.hpp"
#include "widgets/action-button.hpp"
#include "widgets/close-button.hpp"
#include "widgets/color-picker.hpp"
#include "widgets/label.hpp"
using namespace widgetry;
namespace pachde {

struct FancySvg {
    static std::string background() { return asset::plugin(pluginInstance, "res/skiff/fancy-options.svg"); }
};

struct FancyDialog : SvgDialog<FancySvg>
{
    using Base = SvgDialog<FancySvg>;
    SkiffUi* skiff{nullptr};
    LabelStyle* label_style{nullptr};
    LabelStyle* title_style{nullptr};
    LabelStyle* group_style{nullptr};

    FancyDialog(SkiffUi* src, ILoadSvg* loader);

    void add_knob(::svg_query::BoundsIndex& bounds, const char* key, int param);
    void add_label(::svg_query::BoundsIndex& bounds, const char* key, const char* text, LabelStyle* style, std::shared_ptr<svg_theme::SvgTheme> svg_theme);
    void add_check(::svg_query::BoundsIndex& bounds, const char* key, int param, std::shared_ptr<svg_theme::SvgTheme> svg_theme);
    void make_ui();
};

void pre_cache_fancy_dialog_svg(ILoadSvg* loader);
FancyDialog * show_fancy_dialog(SkiffUi *ui);

}