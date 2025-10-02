#include "Imagine.hpp"
#include "imagine_layout.hpp"
#include "../theme.hpp"
#include "../create-theme-widget.hpp"
#include "../pic_button.hpp"
#include "../widgets/logo-widget.hpp"
#include "../widgets/port.hpp"
#include "../widgets/screws.hpp"
#include "../widgets/switch.hpp"
#include "../small_push.hpp"
#include "pic_widget.hpp"
using namespace widgetry;
namespace pachde {

ImagineUi::ImagineUi(Imagine *module)
{
    imagine = module;
    setModule(module);
    if (imagine) {
        imagine->setNotify(this);
        image_source = imagine;
    } else {
        image_source = this;
    }
    applyTheme(GetPreferredTheme(getITheme()));
}

void ImagineUi::onChangeTheme(ChangedItem item)
{
    auto itheme = getITheme();
    switch (item) {
    case ChangedItem::Theme:
        applyTheme(GetPreferredTheme(itheme));
        break;
    case ChangedItem::DarkTheme:
    case ChangedItem::FollowDark:
        if (itheme->getFollowRack()) {
            applyTheme(GetPreferredTheme(itheme));
        }
        break;
    case ChangedItem::MainColor:
        break;
    case ChangedItem::Screws:
        applyScrews(itheme->hasScrews());
        break;
    }
}

void ImagineUi::step()
{
    if (imagine && playButton) {
        playButton->pressed = imagine->isPlaying();
    }

    auto itheme = getITheme();
    bool changed = itheme->pollRackDarkChanged();
    if (imagine) {
        if (!changed && imagine->isDirty()) {
            // sync to cover preset loading
            applyScrews(itheme->hasScrews());
            applyTheme(GetPreferredTheme(itheme));
        }
        imagine->setClean();
    }
    ModuleWidget::step();
}

void ImagineUi::resetHeadPosition(bool ctrl, bool shift)
{
    if (!imagine || !imagine->traversal) return;
    if (shift) {
        auto pos = imagine->traversal->get_position();
        imagine->setResetPos(pos);
    } else {
        auto pos = imagine->getResetPos();
        imagine->traversal->reset();
        if (pos.x >= 0.f && pos.y >= 0.f) {
            imagine->traversal->set_position(pos);
        }
    }
}

Pic * ImagineUi::getImage()
{
    if (!preview_image) {
        preview_image = new(Pic);
        std::string path = MakeUnPluginPath("{plug}/presets/images/guitar-shirt.jpg");
        preview_image->open(path);
    }
    return preview_image;
}

void ImagineUi::makeUi(Theme theme)
{
    assert(children.empty());

    box.size = Vec(300, RACK_GRID_HEIGHT);
    panel = new ImaginePanel(imagine, box.size);
    setPanel(panel);

    if (!imagine || imagine->hasScrews()) {
        AddScrewCaps(this, theme, COLOR_NONE, SCREWS_OUTSIDE);
    }

    {
        auto pic_widget = new PicWidget(imagine);
        pic_widget->box.pos = Vec((PANEL_WIDTH - PANEL_IMAGE_WIDTH)/2.f, PANEL_IMAGE_TOP);
        pic_widget->box.size = Vec(PANEL_IMAGE_WIDTH, PANEL_IMAGE_HEIGHT);
        addChild(pic_widget);
        if (!imagine) {
            pic_widget->setImageSource(this);
        } else {
            pic_widget->setImageSource(imagine);
        }
    }

    auto x = CONTROL_START;
    addInput(createColorInputCentered<ColorPort>(theme, PORT_LIGHT_VIOLET, Vec(x, CONTROL_ROW_2), module, Imagine::SPEED_INPUT));
    x += CONTROL_SPACING;
    addInput(createColorInputCentered<ColorPort>(theme, PORT_LIGHT_ORANGE, Vec(x, CONTROL_ROW_2), module, Imagine::X_INPUT));
    x += CONTROL_SPACING - TIGHT;
    addInput(createColorInputCentered<ColorPort>(theme, PORT_LIGHT_ORANGE, Vec(x, CONTROL_ROW_2), module, Imagine::Y_INPUT));

    {
        auto picButton = new PicButton();
        picButton->setTheme(theme);
        picButton->center(Vec(PANEL_CENTER - CONTROL_SPACING * .5f, CONTROL_ROW_2));
        if (imagine) {
            picButton->onClick([this](bool ctrl, bool shift) {
                if (shift) {
                    if (ctrl) {
                        imagine->closeImage();
                    } else {
                        imagine->reloadImage();
                    }
                } else {
                    imagine->loadImageDialog();
                }
            });
        }
        addChild(picButton);
    }

    playButton = createThemeParamCentered<PlayPauseButton>(theme, Vec(PANEL_CENTER + CONTROL_SPACING * .5f, CONTROL_ROW_2), imagine, Imagine::RUN_PARAM);
    if (imagine) { playButton->onClick([this]() { imagine->setPlaying(!imagine->isPlaying()); }); }
    addParam(playButton);

    x = box.size.x - CONTROL_START - 3.f * CONTROL_SPACING;
    {
        auto reset = createThemeWidgetCentered<SmallPush>(theme, Vec(x, CONTROL_ROW_2));
        if (imagine) {
            reset->describe("Reset head position\n(Shift to save new position)");
            reset->onClick([this](bool ctrl, bool shift) {
                resetHeadPosition(ctrl, shift);
            });
        }
        addChild(reset);
    }
    x += CONTROL_SPACING;
    addChild(createColorInputCentered<ColorPort>(theme, PORT_LIGHT_LIME, Vec(x, CONTROL_ROW_2), module, Imagine::RESET_POS_INPUT));
    x += CONTROL_SPACING;
    addChild(createColorInputCentered<ColorPort>(theme, PORT_LIGHT_VIOLET, Vec(x, CONTROL_ROW_2), module, Imagine::PLAY_INPUT));
    x += CONTROL_SPACING;
    addChild(createColorInputCentered<ColorPort>(theme, PORT_PINK, Vec(x, CONTROL_ROW_2), module, Imagine::MIN_TRIGGER_INPUT));

    x = CONTROL_START;
    {
        auto speed = createThemeParamCentered<SmallKnob>(theme, Vec(x, CONTROL_ROW), imagine, Imagine::SPEED_PARAM);
        speed->stepIncrementBy = 0.1;
        addParam(speed);
    }

    x += CONTROL_SPACING;
    auto knob = createThemeParamCentered<SmallKnob>(theme, Vec(x, CONTROL_ROW), imagine, Imagine::SPEED_MULT_PARAM);
    knob->snap = true;
    addParam(knob);

    x += CONTROL_SPACING;
    knob = createThemeParamCentered<SmallKnob>(theme, Vec(x, CONTROL_ROW), imagine, Imagine::PATH_PARAM);
    knob->minAngle = -1.75;
    knob->maxAngle = 1.75;
    knob->snap = true;
    knob->forceLinear = true;
    addParam(knob);

    x = box.size.x - CONTROL_START - 3.f * CONTROL_SPACING;
    knob = createThemeParamCentered<SmallKnob>(theme, Vec(x, CONTROL_ROW), imagine, Imagine::SLEW_PARAM);
    knob->stepIncrementBy = 0.01f;
    addParam(knob);

    x += CONTROL_SPACING;
    knob = createThemeParamCentered<SmallKnob>(theme, Vec(x, CONTROL_ROW), imagine, Imagine::COMP_PARAM);
    knob->snap = true;
    addParam(knob);

    x += CONTROL_SPACING;
    knob = createThemeParamCentered<SmallKnob>(theme, Vec(x, CONTROL_ROW), imagine, Imagine::GT_PARAM);
    knob->stepIncrementBy = 0.05f;
    addParam(knob);

    x += CONTROL_SPACING;
    {
        auto gtrate = createThemeParamCentered<SmallKnob>(theme, Vec(x, CONTROL_ROW), imagine, Imagine::MIN_TRIGGER_PARAM);
        gtrate->stepIncrementBy = .01f;
        addChild(gtrate);
    }

    addOutput(createThemeOutput<ColorPort>(theme, Vec(21.f, OUTPUT_ROW), imagine, Imagine::X_OUT));
    addOutput(createThemeOutput<ColorPort>(theme, Vec(46.f, OUTPUT_ROW), imagine, Imagine::Y_OUT));
    addOutput(createColorOutput<ColorPort>(theme, PORT_RED,   Vec(71.f, OUTPUT_ROW - 12.f), imagine, Imagine::RED_OUT));
    addOutput(createColorOutput<ColorPort>(theme, PORT_GREEN, Vec(71.f, OUTPUT_ROW + 13.f), imagine, Imagine::GREEN_OUT));
    addOutput(createColorOutput<ColorPort>(theme, PORT_BLUE,  Vec(95.f, OUTPUT_ROW), imagine, Imagine::BLUE_OUT));
    {
        auto p = createThemeParam<widgetry::Switch>(theme, Vec(190.f, OUTPUT_ROW + 1.5f), imagine, Imagine::POLARITY_PARAM);
        p->box.size.y = 18.f;
        addParam(p);
        addOutput(createThemeOutput<ColorPort>(theme, Vec(210.f, OUTPUT_ROW), imagine, Imagine::VOLTAGE_OUT));
        addOutput(createThemeOutput<ColorPort>(theme, Vec(240.f, OUTPUT_ROW), imagine, Imagine::GATE_OUT));
        addOutput(createThemeOutput<ColorPort>(theme, Vec(265.f, OUTPUT_ROW), imagine, Imagine::TRIGGER_OUT));
    }
    // addOutput(createColorOutput<ColorPort>(theme, PORT_LIME, Vec(280.f, OUTPUT_ROW), imagine, Imagine::TEST_OUT));

    auto logo = new LogoWidget(Theme::Light, .18f);
    logo->box.pos = Vec(box.size.x*.5f, RACK_GRID_HEIGHT - RACK_GRID_WIDTH + 3.5f);
    logo->ignore_theme = true;
    addChild(widgetry::Center(logo));

}

void ImagineUi::applyTheme(Theme theme)
{
    if (children.empty()) {
        makeUi(theme);
    } else {
        SetChildrenTheme(this, theme);
    }
}

void ImagineUi::applyScrews(bool screws)
{
    auto itheme = getITheme();
    if (screws) {
        if (HaveScrewChildren(this)) return;
        AddScrewCaps(this, GetPreferredTheme(itheme), COLOR_NONE, SCREWS_OUTSIDE);
    } else {
        RemoveScrewCaps(this);
    }
}

void ImagineUi::appendContextMenu(Menu *menu)
{
    if (!this->module) return;

    AddThemeMenu(menu, getITheme(), false, true);
    menu->addChild(createCheckMenuItem(
        "Labels", "",
        [this]() { return imagine->labels; },
        [this]() { imagine->labels = !imagine->labels; }));
    menu->addChild(createCheckMenuItem(
        "Gold medallion", "",
        [this]() { return imagine->medallion_fill; },
        [this]() { imagine->medallion_fill = !imagine->medallion_fill; }));

    menu->addChild(createCheckMenuItem(
        "Bright image in a dark room", "",
        [this]() { return imagine->bright_image; },
        [this]() { imagine->bright_image = !imagine->bright_image; }));
}

}