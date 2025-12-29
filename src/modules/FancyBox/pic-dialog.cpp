#include "pic-dialog.hpp"
#include "picture.hpp"
#include "services/svg-query.hpp"
#include "services/colors.hpp"
#include "widgets/close-button.hpp"
#include "widgets/color-widgets.hpp"
#define DIALOG_THEMED
#include "widgets/dialog.hpp"
#include "widgets/label.hpp"
#include "widgets/pic_button.hpp"
#include "widgets/switch.hpp"

using namespace widgetry;
namespace pachde {

struct PictureDialogSvg {
    static std::string background() {
        return asset::plugin(pluginInstance, "res/dialogs/fancy-image.svg");
    }
};

struct PictureDialog : SvgDialog<PictureDialogSvg> {
    using Base = SvgDialog<PictureDialogSvg>;

    SvgCache my_svgs;
    FancyUi *fancy{nullptr};
    LabelStyle* title_style{nullptr};
    LabelStyle* center_label_style{nullptr};
    LabelStyle* info_label_style{nullptr};
    TextLabel* image_name{nullptr};
    TextLabel* fit_label{nullptr};
    Picture* preview{nullptr};
    SolidSwatch* pic_bg{nullptr};
    Vec window_size{APP->window->getSize()};

    ~PictureDialog() {
        APP->scene->rack->setTouchedParam(NULL);
    }

    Rect get_preview_rect() {
        Rect preview_box = svg_query::elementBounds(Base::get_svg(), "preview");
        float preview_aspect = preview_box.size.x / preview_box.size.y;
        Rect r{0,0};
        r.size = APP->window->getSize();
        if (APP->scene->menuBar->isVisible()) {
            r.size.y -= APP->scene->menuBar->box.size.y;
        }
        float aspect{r.size.x / r.size.y};
        if (aspect == preview_aspect) {
            return preview_box;
        } else {
            float w = preview_box.size.y * aspect;
            r.size = (w > preview_box.size.x)
                ? Vec(preview_box.size.x, preview_box.size.x / aspect)
                : Vec(w, preview_box.size.y);
            r.pos.x = preview_box.pos.x + (preview_box.size.x - r.size.x) * .5f;
            r.pos.y = preview_box.pos.y + (preview_box.size.y - r.size.y) * .5f;
        }
        return r;
    }

    PictureDialog(FancyUi *fancy, ILoadSvg* svg_loader) :
        Base(fancy, svg_loader),
        fancy(fancy)
    {
        auto theme = fancy->theme_holder->getTheme();
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
        addChild(TextLabel::createLabel(bounds["k:dlg-title"], "FancyBox | Image options", title_style));

        // Preview
        Rect preview_rect{get_preview_rect()};

        pic_bg = new SolidSwatch;
        pic_bg->color = 0xff808080;
        pic_bg->box = preview_rect;
        addChild(pic_bg);

        preview = new Picture(&fancy->my_module->fancy_data.image.options);
        preview->box = preview_rect;
        addChild(preview);

        auto name = system::getFilename(fancy->my_module->fancy_data.image.options.path);
        image_name = add_label(bounds, "k:pic-label", name.c_str(), info_label_style, svg_theme);

        auto pic_button = createWidgetCentered<PicButton>(bounds["k:pic-button"].getCenter());
        pic_button->setTheme(fancy->theme_holder->getTheme());
        pic_button->set_handler([=](bool c, bool s){
            fancy->click_pic(c, s);
            auto name = system::getFilename(fancy->my_module->fancy_data.image.options.path);
            image_name->set_text(name);

        });
        addChild(pic_button);

        {
            auto option = createParam<widgetry::Switch>(Vec(0,0), fancy->my_module, Fancy::P_FANCY_IMAGE_FIT);
            option->box = bounds["k:pic-fit"];
            option->applyTheme(svg_theme);
            addChild(option);
            fit_label = add_label(bounds, "k:fit-label", "", info_label_style, svg_theme);
        }

        add_check(bounds,"k:pic-gray", Fancy::P_FANCY_IMAGE_GRAY, svg_theme);

        add_knob(bounds, "k:off-x", Fancy::P_FANCY_IMAGE_X_OFFSET);
        add_knob(bounds, "k:off-y", Fancy::P_FANCY_IMAGE_Y_OFFSET);
        add_knob(bounds, "k:scale", Fancy::P_FANCY_IMAGE_SCALE);

        add_label(bounds, "k:off-x-label", "X", center_label_style, svg_theme);
        add_label(bounds, "k:off-y-label", "Y", center_label_style, svg_theme);
        add_label(bounds, "k:scale-label", "SCALE",  center_label_style, svg_theme);

        preview->open();
        my_svgs.changeTheme(svg_theme);
        applyChildrenTheme(this, svg_theme);
    }

    void step() override {
        Base::step();

        if (preview) {
            Vec current_size{APP->window->getSize()};
            if (window_size != current_size) {
                window_size = current_size;
                Rect preview_rect{get_preview_rect()};
                pic_bg->box = preview_rect;
                preview->box = preview_rect;
            }
        }
        auto pq = fancy->my_module->getParamQuantity(Fancy::P_FANCY_IMAGE_FIT);
        if (pq) {
            fit_label->set_text(pq->getDisplayValueString());
        }
    }

    void add_check(::svg_query::BoundsIndex &bounds, const char *key,
        int param, std::shared_ptr<svg_theme::SvgTheme> svg_theme
    ) {
        auto check = Center(createThemeParamButton<CheckButton>(
            &my_svgs,
            bounds[key].getCenter(),
            fancy->my_module,
            param,
            svg_theme
        ));
        //HOT_POSITION(key, HotPosKind::Center, check);
        addChild(check);
    }

    TextLabel* add_label(
        ::svg_query::BoundsIndex &bounds,
        const char *key,
        const char *text,
        LabelStyle* style,
        std::shared_ptr<svg_theme::SvgTheme> svg_theme
    ) {
        auto label = TextLabel::createLabel(bounds[key], text, style, svg_theme);
        //HOT_POSITION(key, HotPosKind::Box, label);
        addChild(label);
        return label;
    }

    void add_knob(::svg_query::BoundsIndex &bounds, const char *key, int param) {
        auto knob = createParamCentered<RoundBlackKnob>(bounds[key].getCenter(), fancy->my_module, param);
        //HOT_POSITION(key, HotPosKind::Center, knob);
        addChild(knob);
    }

    void createStyles(std::shared_ptr<svg_theme::SvgTheme> svg_theme) {
        title_style = new LabelStyle("dlg-title", colors::Black, 14.f, true);
        title_style->valign = VAlign::Top;
        title_style->halign = HAlign::Left;

        center_label_style = new LabelStyle("dlg-label");
        center_label_style->halign = HAlign::Center;

        info_label_style = new LabelStyle("dlg-info", colors::PortCorn, 12.f);

        title_style->applyTheme(svg_theme);
        center_label_style->applyTheme(svg_theme);
        info_label_style->applyTheme(svg_theme);
    }
};

void show_picture_dialog(FancyUi *fancy) {
    SvgNoCache nocache;
    createDialog<PictureDialog, FancyUi>(fancy, Vec(fancy->box.size.x *.5, fancy->box.size.y *.5 - 15.f), &nocache, true);
}

}