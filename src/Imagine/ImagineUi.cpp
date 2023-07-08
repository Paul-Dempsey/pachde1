#include "Imagine.hpp"
#include "imagine_layout.hpp"
#include "../pic_button.hpp"
#include "../port.hpp"
#include "../small_push.hpp"
#include "pic_widget.hpp"

namespace pachde {

ImagineUi::ImagineUi(Imagine *module)
{
    imagine = module;
    setModule(module);
    setTheme(ModuleTheme(module));
}

void ImagineUi::step()
{
    ModuleWidget::step();
    if (imagine && playButton) {
        playButton->pressed = imagine->isPlaying();
    }
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

void ImagineUi::makeUi(Imagine* module, Theme theme)
{
    assert(children.empty());

    box.size = Vec(300, RACK_GRID_HEIGHT);
    panel = new ImaginePanel(module, box.size);
    setPanel(panel);

    if (!module || module->hasScrews()) {
        AddScrewCaps(this, theme, COLOR_NONE, SCREWS_OUTSIDE);
    }

    auto image = new PicWidget(module);
    image->box.pos = Vec((PANEL_WIDTH - PANEL_IMAGE_WIDTH)/2.f, PANEL_IMAGE_TOP);
    image->box.size = Vec(PANEL_IMAGE_WIDTH, PANEL_IMAGE_HEIGHT);
    addChild(image);

    addInput(createColorInputCentered<ColorPort>(theme, PORT_ORANGE, Vec(33.f, CONTROL_ROW_2), module, Imagine::X_INPUT));
    addInput(createColorInputCentered<ColorPort>(theme, PORT_ORANGE, Vec(58.f, CONTROL_ROW_2), module, Imagine::Y_INPUT));

    auto reset = createThemeWidgetCentered<SmallPush>(theme, Vec(215.f, CONTROL_ROW_2));
    if (module) {
        reset->describe("Reset head position\n(Shift to save new position)");
        reset->onClick([this](bool ctrl, bool shift) {
            this->resetHeadPosition(ctrl, shift);
        });
    }
    addChild(reset);
    addChild(createColorInputCentered<ColorPort>(theme, PORT_LIGHT_LIME, Vec(245.f, CONTROL_ROW_2), module, Imagine::RESET_POS_INPUT));
    addChild(createColorInputCentered<ColorPort>(theme, PORT_LIGHT_VIOLET, Vec(275.f, CONTROL_ROW_2), module, Imagine::PLAY_INPUT));

    auto picButton = new PicButton(theme);
    picButton->center(Vec (PANEL_CENTER - 15, CONTROL_ROW_2));
    if (module) {
        picButton->onClick([this, module]() {
            module->loadImageDialog();
        });
    }
    addChild(picButton);

    playButton = createThemeParamCentered<PlayPauseButton>(theme, Vec(PANEL_CENTER + 15, CONTROL_ROW_2), module, Imagine::RUN_PARAM);
    if (module) { playButton->onClick([module]() { module->setPlaying(!module->isPlaying()); }); }
    addParam(playButton);


    auto speed = createThemeParamCentered<SmallKnob>(theme, Vec(25.f, CONTROL_ROW), module, Imagine::SPEED_PARAM);
    speed->stepIncrementBy = 0.1;
    addParam(speed);

    auto knob = createThemeParamCentered<SmallKnob>(theme, Vec(55.f, CONTROL_ROW), module, Imagine::SPEED_MULT_PARAM);
    knob->snap = true;
    addParam(knob);

    knob = createThemeParamCentered<SmallKnob>(theme, Vec(85.f, CONTROL_ROW), module, Imagine::PATH_PARAM);
    knob->minAngle = -1.75;
    knob->maxAngle = 1.75;
    knob->snap = true;
    knob->forceLinear = true;
    addParam(knob);

    knob = createThemeParamCentered<SmallKnob>(theme, Vec(215.f, CONTROL_ROW), module, Imagine::SLEW_PARAM);
    knob->stepIncrementBy = 0.01f;
    addParam(knob);

    knob = createThemeParamCentered<SmallKnob>(theme, Vec(245.f, CONTROL_ROW), module, Imagine::COMP_PARAM);
    knob->snap = true;
    addParam(knob);

    knob = createThemeParamCentered<SmallKnob>(theme, Vec(275.f, CONTROL_ROW), module, Imagine::GT_PARAM);
    knob->stepIncrementBy = 0.05f;
    addParam(knob);

    addOutput(createThemeOutput<ColorPort>(theme, Vec(21.f, OUTPUT_ROW), module, Imagine::X_OUT));
    addOutput(createThemeOutput<ColorPort>(theme, Vec(46.f, OUTPUT_ROW), module, Imagine::Y_OUT));
    addOutput(createColorOutput<ColorPort>(theme, PORT_RED,   Vec(70.f, OUTPUT_ROW - 12.f), module, Imagine::RED_OUT));
    addOutput(createColorOutput<ColorPort>(theme, PORT_GREEN, Vec(70.f, OUTPUT_ROW + 13.f), module, Imagine::GREEN_OUT));
    addOutput(createColorOutput<ColorPort>(theme, PORT_BLUE,  Vec(95.f, OUTPUT_ROW), module, Imagine::BLUE_OUT));

    auto p = createThemeParam<Switch>(theme, Vec(190.f, OUTPUT_ROW + 1.5f), module, Imagine::POLARITY_PARAM);
    p->box.size.y = 18.f;
    addParam(p);
    addOutput(createThemeOutput<ColorPort>(theme, Vec(210.f, OUTPUT_ROW), module, Imagine::VOLTAGE_OUT));
    addOutput(createThemeOutput<ColorPort>(theme, Vec(240.f, OUTPUT_ROW), module, Imagine::GATE_OUT));
    addOutput(createThemeOutput<ColorPort>(theme, Vec(265.f, OUTPUT_ROW), module, Imagine::TRIGGER_OUT));

    //addOutput(createColorOutput<ColorPort>(theme, PORT_LIME, Vec(280.f, OUTPUT_ROW), module, Imagine::TEST_OUT));

}

void ImagineUi::setTheme(Theme theme)
{
    auto module = dynamic_cast<Imagine *>(this->module);
    if (module) {
        module->setTheme(theme);
    } else {
        ThemeBase::setTheme(theme);
    }
    if (children.empty()) {
        makeUi(module, theme);
    } else {
        SetChildrenTheme(this, theme);
    }
}

void ImagineUi::setScrews(bool screws)
{
    if (imagine) { 
        imagine->setScrews(screws);
    } else {
        ThemeBase::setScrews(screws);
    }
    if (screws) {
        if (HaveScrewChildren(this)) return;
        AddScrewCaps(this, getTheme(), COLOR_NONE, SCREWS_OUTSIDE);
    } else {
        RemoveScrewCaps(this);
    }
}

void ImagineUi::appendContextMenu(Menu *menu)
{
    if (!this->module) return;

    AddThemeMenu(menu, this, false, true);
    menu->addChild(createCheckMenuItem(
        "Labels", "",
        [this]() { return imagine->labels; },
        [this]() { imagine->labels = !imagine->labels; }));
    menu->addChild(createCheckMenuItem(
        "Bright image in a dark room", "",
        [this]() { return imagine->bright_image; },
        [this]() { imagine->bright_image = !imagine->bright_image; }));
}

}