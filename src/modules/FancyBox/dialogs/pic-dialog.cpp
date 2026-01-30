#include "fancy-dialogs.hpp"
#include "../picture.hpp"
#include "services/svg-query.hpp"
#include "services/colors.hpp"
#include "widgets/close-button.hpp"
#include "widgets/color-widgets.hpp"
#define DIALOG_THEMED
#include "widgets/dialog.hpp"
#include "widgets/label.hpp"
#include "widgets/pic_button.hpp"
#include "widgets/switch.hpp"

namespace pachde {

struct PictureDialogSvg {
    static std::string background() {
        return asset::plugin(pluginInstance, "res/dialogs/fancy-image.svg");
    }
};

struct PictureDialog : SvgDialog<PictureDialogSvg> {
    using Base = SvgDialog<PictureDialogSvg>;

    SvgCache my_svgs;
    Fancy * fancy_module{nullptr};
    DialogStyles styles;
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

    PictureDialog(ModuleWidget* source, ILoadSvg* svg_loader) :
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

        // Close button
        add_close_button(this, bounds, "k:close", svg_theme);

        // Dialog title
        addChild(TextLabel::createLabel(bounds["k:dlg-title"], "FancyBox | Image options", styles.title_style));

        // Preview
        Rect preview_rect{get_preview_rect()};

        pic_bg = new SolidSwatch;
        pic_bg->color = 0xff808080;
        pic_bg->box = preview_rect;
        addChild(pic_bg);

        preview = new Picture(&fancy_module->fancy_data.image.options);
        preview->box = preview_rect;
        addChild(preview);

        auto name = system::getFilename(fancy_module->fancy_data.image.options.path);
        image_name = add_label(this, bounds, "k:pic-label", name.c_str(), styles.info_label_style, svg_theme);

        auto pic_button = createWidgetCentered<PicButton>(bounds["k:pic-button"].getCenter());
        pic_button->setTheme(theme);
        pic_button->describe(fancy_module->fancy_data.image.options.path);
        pic_button->set_handler([=](bool c, bool s){
            if (pictureFileDialog(&fancy_module->fancy_data.image.options, fancy_module->pic_folder)) {
                auto name = system::getFilename(fancy_module->fancy_data.image.options.path);
                image_name->set_text(name);
                pic_button->describe(fancy_module->fancy_data.image.options.path);
                //preview->open();
            }
        });
        addChild(pic_button);
        add_check(this, bounds, "k:pic-check", fancy_module, Fancy::P_FANCY_IMAGE_ON, svg_theme);
        add_label(this, bounds, "k:pic-check-label", "Enable Image", styles.left_label_style, svg_theme);
        {
            auto option = createParam<widgetry::Switch>(0, fancy_module, Fancy::P_FANCY_IMAGE_FIT);
            option->box = bounds["k:pic-fit"];
            option->applyTheme(svg_theme);
            addChild(option);
            fit_label = add_label(this, bounds, "k:fit-label", "", styles.info_label_style, svg_theme);
        }

        add_check(this, bounds,"k:pic-gray", fancy_module, Fancy::P_FANCY_IMAGE_GRAY, svg_theme);

        add_knob(this, bounds, "k:off-x", fancy_module, Fancy::P_FANCY_IMAGE_X_OFFSET);
        add_knob(this, bounds, "k:off-y", fancy_module, Fancy::P_FANCY_IMAGE_Y_OFFSET);
        add_knob(this, bounds, "k:scale", fancy_module, Fancy::P_FANCY_IMAGE_SCALE);

        add_label(this, bounds, "k:off-x-label", "X", styles.center_label_style, svg_theme);
        add_label(this, bounds, "k:off-y-label", "Y", styles.center_label_style, svg_theme);
        add_label(this, bounds, "k:scale-label", "SCALE",  styles.center_label_style, svg_theme);

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
        auto pq = fancy_module->getParamQuantity(Fancy::P_FANCY_IMAGE_FIT);
        if (pq) {
            fit_label->set_text(pq->getDisplayValueString());
        }
    }
};

void show_picture_dialog(ModuleWidget* source, Fancy* module, Theme theme) {
    SvgNoCache nocache;
    auto dlg = createDialog<PictureDialog, ModuleWidget>(source, Vec(source->box.size.x *.5, source->box.size.y *.5 - 15.f), &nocache, true);
    dlg->set_module(module);
    dlg->create_ui(theme);
}

}