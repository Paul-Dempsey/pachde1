#include "settings-dialog.hpp"
#define DIALOG_THEMED
#include "services/svg-query.hpp"
#include "widgets/action-button.hpp"
#include "widgets/close-button.hpp"
#include "widgets/color-picker.hpp"
#include "widgets/dialog.hpp"
#include "widgets/fancy-swatch.hpp"
#include "widgets/hamburger.hpp"
#include "widgets/label.hpp"
#include "widgets/switch.hpp"
#include "widgets/text-button.hpp"
#include "widgets/text-input.hpp"

using namespace widgetry;

namespace pachde {
const NVGcolor rack_light{fromPacked(0xffdedede)};
const NVGcolor rack_dark{fromPacked(0xff303030)};
const NVGcolor rack_hc{fromPacked(0xff030303)};

struct Preview : TransparentWidget {
    InfoSettings* settings{nullptr};
    InfoModule* info_mod{nullptr};

    Preview(Rect box, InfoSettings* settings, InfoModule* mod) : settings{settings}, info_mod(mod) {
        this->box = box;
    }

    const float preview_scale{.25f};

    void draw(const DrawArgs& args) override {
        auto vg = args.vg;
        Rect preview_box{box.zeroPos()};
        if (info_mod) {
            float w = info_mod->width * 15 * preview_scale;
            if (w < preview_box.size.x) {
                preview_box.pos.x = preview_box.size.x*.5 - w*.5;
                preview_box.size.x = w;
            }
        }

        auto co_info_panel = settings->getDisplayPanelColor();
        if (!packed_color::isOpaque(co_info_panel)) {
            switch (getActualTheme(ThemeSetting::FollowRackUi)) {
            case Theme::Light:
                FillRect(vg, RECT_ARGS(preview_box), rack_light);
                break;

            default:
            case Theme::Dark:
                FillRect(vg, RECT_ARGS(preview_box), rack_dark);
                break;

            case Theme::HighContrast:
                FillRect(vg, RECT_ARGS(preview_box), rack_hc);
                break;
            }
        }
        FillRect(vg, RECT_ARGS(preview_box), fromPacked(settings->getDisplayPanelColor()));

        std::string text = (info_mod) ? info_mod->text : "";
        if (text.empty()) text = "Info sample text";
        draw_oriented_text_box(vg,
            preview_box,
            settings->left_margin *preview_scale, settings->right_margin *preview_scale,
            settings->top_margin *preview_scale, settings->bottom_margin *preview_scale,
            text,
            settings->getFont(),
            settings->getFontSize() *preview_scale,
            settings->getDisplayTextColor(),
            settings->getHorizontalAlignment(),
            settings->getVerticalAlignment(),
            settings->getOrientation(),
            0x60cc6c51
        );
     }
};

struct SettingsDialogSvg {
    static std::string background() {
        return asset::plugin(pluginInstance, "res/dialogs/info-settings.svg");
    }
};

struct SettingsDialog : SvgDialog<SettingsDialogSvg> {
    using Base = SvgDialog<SettingsDialogSvg>;

    SvgNoCache my_svgs;
    InfoModuleWidget* info{nullptr};
    InfoSettings* settings{nullptr};
    LabelStyle* title_style{nullptr};
    LabelStyle* center_label_style{nullptr};
    LabelStyle* up_label_style{nullptr};
    LabelStyle* down_label_style{nullptr};
    LabelStyle* inverted_label_style{nullptr};
    LabelStyle* left_label_style{nullptr};
    LabelStyle* info_label_style{nullptr};
    //LabelStyle* info_center_style{nullptr};

    FancySwatch* panel_swatch{nullptr};
    FancySwatch* text_swatch{nullptr};
    TextLabel* font_size_label{nullptr};
    TextLabel* horz_actual_label{nullptr};
    TextLabel* vert_actual_label{nullptr};
    TextLabel* orient_actual_label{nullptr};
    TextLabel* copper_left_actual_label{nullptr};
    TextLabel* copper_right_actual_label{nullptr};

    float last_font_size{-1.f};
    HAlign last_horz{(HAlign)-1};
    VAlign last_vert{(VAlign)-1};
    Orientation last_orient{(Orientation)-1};
    CopperTarget last_left_copper{(CopperTarget)-1};
    CopperTarget last_right_copper{(CopperTarget)-1};

    Rect preview_box{Vec(141,26), Vec(60,76)};

    void createStyles(std::shared_ptr<svg_theme::SvgTheme> svg_theme) {
        title_style = new LabelStyle("dlg-title", colors::Black, 14.f, true);
        title_style->valign = VAlign::Top;
        title_style->halign = HAlign::Left;

        left_label_style = new LabelStyle("dlg-label");
        left_label_style->halign = HAlign::Left;

        center_label_style = new LabelStyle("dlg-label");
        center_label_style->halign = HAlign::Center;

        info_label_style = new LabelStyle("dlg-info", colors::PortCorn, 12.f);
        info_label_style->halign = HAlign::Left;
        //info_center_style  = new LabelStyle("dlg-info", colors::PortCorn, 12.f);
        //info_center_style->halign = HAlign::Center;

        title_style->applyTheme(svg_theme);
        left_label_style->applyTheme(svg_theme);
        center_label_style->applyTheme(svg_theme);
        info_label_style->applyTheme(svg_theme);
        //info_center_style->applyTheme(svg_theme);
    }

    TextButton* makeTextButton (
        std::map<std::string,::math::Rect>& bounds,
        const char* key,
        //bool sticky,
        const char* title,
        const char* tip,
        std::shared_ptr<svg_theme::SvgTheme> svg_theme,
        std::function<void(bool,bool)> handler)
    {
        auto button = new TextButton;
        button->box = bounds[key];
        //button->set_sticky(sticky);
        button->set_text(title);
        if (source->module) {
            if (tip) button->describe(tip);
            button->set_handler(handler);
        }
        button->applyTheme(svg_theme);
        return button;
    }

    SettingsDialog(InfoModuleWidget* src, ILoadSvg* loader) :
        Base(src, loader),
        info(src),
        settings(src->settings)
    {
        auto theme = src->theme_holder->getTheme();
        auto svg_theme = getThemeCache().getTheme(ThemeName(theme));
        auto layout = Base::get_svg();
        applySvgTheme(layout, svg_theme);
        createStyles(svg_theme);

        ::svg_query::BoundsIndex bounds;
        svg_query::addBounds(layout, "k:", bounds, true);

        // Close button
        auto close = createWidgetCentered<CloseButton>(bounds["k:close"].getCenter());
        close->set_handler([=](){ Base::close(); } );
        close->applyTheme(svg_theme);
        addChild(close);

        // Dialog title
        addChild(TextLabel::createLabel(bounds["k:dlg-title"], "Info | Text options", title_style));

        // Horizontal alignment
        auto option = createParam<widgetry::Switch>(Vec(0,0), info->my_module, InfoModule::P_HAlign);
        option->box = bounds["k:horz-switch"];
        option->applyTheme(svg_theme);
        addChild(option);
        addChild(TextLabel::createLabel(bounds["k:horz-label"], "H", center_label_style));
        addChild(horz_actual_label = TextLabel::createLabel(bounds["k:horz-actual"], "", info_label_style));

        // Vertical alignment
        option = createParam<widgetry::Switch>(Vec(0,0), info->my_module, InfoModule::P_VAlign);
        option->box = bounds["k:vert-switch"];
        option->applyTheme(svg_theme);
        addChild(option);
        addChild(TextLabel::createLabel(bounds["k:vert-label"], "V", center_label_style));
        addChild(vert_actual_label = TextLabel::createLabel(bounds["k:vert-actual"], "", info_label_style));

        // Orientaiton
        option = createParam<widgetry::Switch>(Vec(0,0), info->my_module, InfoModule::P_Orientation);
        option->box = bounds["k:orient-switch"];
        option->applyTheme(svg_theme);
        addChild(option);
        addChild(TextLabel::createLabel(bounds["k:orient-label"], "O", center_label_style));
        addChild(orient_actual_label = TextLabel::createLabel(bounds["k:orient-actual"], "", info_label_style));

        // Copper
        option = createParam<widgetry::Switch>(Vec(0,0), info->my_module, InfoModule::P_CopperLeft);
        option->box = bounds["k:copr-l-switch"];
        option->applyTheme(svg_theme);
        addChild(option);
        addChild(TextLabel::createLabel(bounds["k:copr-l-label"], "CL", center_label_style));
        addChild(copper_left_actual_label = TextLabel::createLabel(bounds["k:copr-l-actual"], "", info_label_style));

        option = createParam<widgetry::Switch>(Vec(0,0), info->my_module, InfoModule::P_CopperRight);
        option->box = bounds["k:copr-r-switch"];
        option->applyTheme(svg_theme);
        addChild(option);
        addChild(TextLabel::createLabel(bounds["k:copr-r-label"], "CR", center_label_style));
        addChild(copper_right_actual_label = TextLabel::createLabel(bounds["k:copr-r-actual"], "", info_label_style));

        // Left margin
        addChild(createParamCentered<RoundBlackKnob>(bounds["k:left-margin"].getCenter(), info->my_module, InfoModule::P_MarginLeft));
        addChild(TextLabel::createLabel(bounds["k:L-label"], "L", center_label_style));

        // Right margin
        addChild(createParamCentered<RoundBlackKnob>(bounds["k:right-margin"].getCenter(), info->my_module, InfoModule::P_MarginRight));
        addChild(TextLabel::createLabel(bounds["k:R-label"], "R", center_label_style));

        // Top margin
        addChild(createParamCentered<RoundBlackKnob>(bounds["k:top-margin"].getCenter(), info->my_module, InfoModule::P_MarginTop));
        addChild(TextLabel::createLabel(bounds["k:T-label"], "T", center_label_style));

        // Bottom margin
        addChild(createParamCentered<RoundBlackKnob>(bounds["k:bottom-margin"].getCenter(), info->my_module, InfoModule::P_MarginBottom));
        addChild(TextLabel::createLabel(bounds["k:B-label"], "B", center_label_style));


        // Preview
        addChild(new Preview(bounds["k:preview"], settings, info->my_module));

        // Panel color
        addChild(panel_swatch = new FancySwatch(bounds["k:panel-swatch"], settings->getUserPanelColor()));
        auto palette = Center(createThemeSvgButton<Palette1ActionButton>(&my_svgs, bounds["k:panel-co"].getCenter()));
        palette->describe("Panel color");
        palette->set_handler([=](bool,bool) {
            auto picker = new ColorPickerMenu();
            picker->set_color(settings->getUserPanelColor());
            picker->set_on_new_color([=](PackedColor color) {
                settings->setUserPanelColor(color);
                panel_swatch->set_color(color);
            });
            auto menu = createMenu();
            menu->addChild(picker);
        });
        addChild(palette);
        addChild(TextLabel::createLabel(bounds["k:panel-co-label"], "PANEL", center_label_style));

        // Text color
        addChild(text_swatch = new FancySwatch(bounds["k:text-swatch"], settings->getUserTextColor()));
        palette = Center(createThemeSvgButton<Palette1ActionButton>(&my_svgs, bounds["k:text-co"].getCenter()));
        palette->describe("Text color");
        palette->set_handler([=](bool,bool) {
            auto picker = new ColorPickerMenu();
            picker->set_color(settings->getUserTextColor());
            picker->set_on_new_color([=](PackedColor color) {
                settings->setUserTextColor(color);
                text_swatch->set_color(color);
            });
            auto menu = createMenu();
            menu->addChild(picker);
        });
        addChild(palette);
        addChild(TextLabel::createLabel(bounds["k:text-co-label"], "TEXT", center_label_style));

         // Font size knob
        addChild(createParamCentered<RoundBlackKnob>(bounds["k:font-size"].getCenter(), info->my_module, InfoModule::P_FontSize));
        addChild(font_size_label = TextLabel::createLabel(bounds["k:font-size-label"], "", center_label_style));

        // Font button
        addChild(makeTextButton(bounds, "k:font-btn", "Font...", "Choose a font file", svg_theme, [=](bool,bool){
            settings->fontDialog();
        }));
        // font name
        addChild(TextLabel::createLabel(bounds["k:font-label"], system::getStem(settings->font_file), info_label_style));

        // Text input
        auto input = new MultiTextInput();
        input->box = bounds["k:edit"];
        input->multiline = true;
        input->placeholder = "Type or paste your Info here.";
        input->change_handler = [=](std::string content){
            if (info->my_module) {
                info->my_module->text = content;
            }
        };
        if (info->my_module) {
            input->setText(info->my_module->text);
        }
        addChild(input);
    }

    void step() override {
        Base::step();
        if (!info->my_module) return;
        if (settings->getFontSize() != last_font_size) {
            last_font_size = settings->getFontSize();
            auto pq = info->my_module->getParamQuantity(InfoModule::P_FontSize);
            if (pq) {
                font_size_label->set_text(pq->getDisplayValueString() + "px");
            }
        }
        if (settings->getHorizontalAlignment() != last_horz) {
            last_horz = settings->getHorizontalAlignment();
            auto pq = info->my_module->getParamQuantity(InfoModule::P_HAlign);
            if (pq) {
                horz_actual_label->set_text(pq->getDisplayValueString());
            }
        }
        if (settings->getVerticalAlignment() != last_vert) {
            last_vert = settings->getVerticalAlignment();
            auto pq = info->my_module->getParamQuantity(InfoModule::P_VAlign);
            if (pq) {
                vert_actual_label->set_text(pq->getDisplayValueString());
            }
        }
        if (settings->getOrientation() != last_orient) {
            last_orient = settings->getOrientation();
            auto pq = info->my_module->getParamQuantity(InfoModule::P_Orientation);
            if (pq) {
                orient_actual_label->set_text(pq->getDisplayValueString());
            }
        }
        if (info->my_module->getLeftCopperTarget() != last_left_copper) {
            last_left_copper = info->my_module->getLeftCopperTarget();
            auto pq = info->my_module->getParamQuantity(InfoModule::P_CopperLeft);
            if (pq) {
                copper_left_actual_label->set_text(pq->getDisplayValueString());
            }
        }
        if (info->my_module->getRightCopperTarget() != last_right_copper) {
            last_right_copper = info->my_module->getRightCopperTarget();
            auto pq = info->my_module->getParamQuantity(InfoModule::P_CopperRight);
            if (pq) {
                copper_right_actual_label->set_text(pq->getDisplayValueString());
            }
        }
    }

};

void show_settings_dialog(InfoModuleWidget* src) {
    SvgNoCache nocache;
    createDialog<SettingsDialog, InfoModuleWidget>(src, Vec(src->box.size.x - 12.f, 34.f), &nocache, false);
}


}