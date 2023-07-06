#include "Imagine.hpp"
#include "imagine_layout.hpp"
#include "../pic_button.hpp"
#include "../port.hpp"
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

    addChild(createColorInputCentered<ColorPort>(theme, nvgHSL(270./360., 0.75, 0.75), Vec(242.f, CONTROL_ROW), module, Imagine::PLAY_INPUT));

    playButton = createThemeParamCentered<PlayPauseButton>(theme, Vec(265.f, CONTROL_ROW), module, Imagine::RUN_PARAM);
    if (module) {
        playButton->onClick([module]() {
            module->setPlaying(!module->isPlaying());
        });
    }
    addParam(playButton);

    auto picButton = new PicButton(theme);
    picButton->center(Vec (285.f, CONTROL_ROW));
    if (module) {
        picButton->onClick([this, module]() {
            module->loadImageDialog();
        });
    }
    addChild(picButton);

    auto speed = createThemeParamCentered<SmallKnob>(theme, Vec(25.f, CONTROL_ROW), module, Imagine::SPEED_PARAM);
    speed->stepIncrementBy = 0.1;
    addParam(speed);


    auto knob = createThemeParamCentered<SmallKnob>(theme, Vec(55.f, CONTROL_ROW), module, Imagine::SPEED_MULT_PARAM);
    knob->snap = true;
    addParam(knob);

    knob = createThemeParamCentered<SmallKnob>(theme, Vec(85.f, CONTROL_ROW), module, Imagine::SLEW_PARAM);
    knob->stepIncrementBy = 0.01f;
    addParam(knob);

    knob = createThemeParamCentered<SmallKnob>(theme, Vec(115.f, CONTROL_ROW), module, Imagine::COMP_PARAM);
    knob->snap = true;
    addParam(knob);

    knob = createThemeParamCentered<SmallKnob>(theme, Vec(145.f, CONTROL_ROW), module, Imagine::GT_PARAM);
    knob->stepIncrementBy = 0.05f;
    addParam(knob);

    knob = createThemeParamCentered<SmallKnob>(theme, Vec(175.f, CONTROL_ROW), module, Imagine::PATH_PARAM);
    knob->minAngle = -1.5;
    knob->maxAngle = 1.5;
    knob->snap = true;
    knob->forceLinear = true;
    addParam(knob);

    addOutput(createThemeOutput<ColorPort>(theme, Vec(15.f, OUTPUT_ROW), module, Imagine::X_OUT));
    addOutput(createThemeOutput<ColorPort>(theme, Vec(40.f, OUTPUT_ROW), module, Imagine::Y_OUT));
    addOutput(createColorOutput<ColorPort>(theme, PORT_RED,   Vec(64.f, OUTPUT_ROW - 12.f), module, Imagine::RED_OUT));
    addOutput(createColorOutput<ColorPort>(theme, PORT_GREEN, Vec(64.f, OUTPUT_ROW + 13.f), module, Imagine::GREEN_OUT));
    addOutput(createColorOutput<ColorPort>(theme, PORT_BLUE,  Vec(87.f, OUTPUT_ROW), module, Imagine::BLUE_OUT));

    auto p = createThemeParam<Switch>(theme, Vec(180.f, OUTPUT_ROW + 1.5f), module, Imagine::POLARITY_PARAM);
    p->box.size.y = 18.f;
    addParam(p);
    addOutput(createThemeOutput<ColorPort>(theme, Vec(200.f, OUTPUT_ROW), module, Imagine::VOLTAGE_OUT));
    addOutput(createThemeOutput<ColorPort>(theme, Vec(230.f, OUTPUT_ROW), module, Imagine::GATE_OUT));
    addOutput(createThemeOutput<ColorPort>(theme, Vec(255.f, OUTPUT_ROW), module, Imagine::TRIGGER_OUT));

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

    auto module = dynamic_cast<Imagine*>(this->module);
    menu->addChild(new MenuSeparator);
    menu->addChild(createCheckMenuItem(
        "Bright image in a dark room", "",
        [=]() { return module->bright_image; },
        [=]() { module->bright_image = !module->bright_image; }));

    AddThemeMenu(menu, this, false, true);
}

}