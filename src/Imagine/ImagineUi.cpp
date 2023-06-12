#include "Imagine.hpp"
#include "pic_widget.hpp"
#include "../port.hpp"

ImagineUi::ImagineUi(Imagine *module)
{
    imagine = module;
    setModule(module);
    setTheme(ModuleTheme(module));
}

void ImagineUi::makeUi(Imagine* module, Theme theme)
{
    assert(children.empty());

    box.size = Vec(300, RACK_GRID_HEIGHT);
    panel = new ImaginePanel(module, box.size);
    setPanel(panel);

    if (!module || module->hasScrews()) {
        AddScrewCaps(this, theme, COLOR_NONE);
    }
    addChild(createThemeWidgetCentered<LogoWidget>(theme, Vec(box.size.x / 2.f, RACK_GRID_HEIGHT - ONE_HP + 7.5f)));

    addOutput(createThemeOutput<ColorPort>(theme, Vec(15.f, 320.f), module, Imagine::X_OUT));
    addOutput(createThemeOutput<ColorPort>(theme, Vec(40.f, 320.f), module, Imagine::Y_OUT));

    addParam(createThemeParamCentered<SmallKnob>(theme, Vec(82.5f, 330.f), module, Imagine::SLEW_PARAM));
    addOutput(createThemeOutput<ColorPort>(theme, Vec(100.f, 320.f), module, Imagine::VOLTAGE_OUT));
    auto p = createThemeParam<Switch>(theme, Vec(130.f, 324.f), module, Imagine::VOLTAGE_RANGE_PARAM);
    p->box.size.y = 18.f;
    addParam(p);

    addOutput(createThemeOutput<ColorPort>(theme, Vec(150.f, 320.f), module, Imagine::GATE_OUT));
    addOutput(createThemeOutput<ColorPort>(theme, Vec(175.f, 320.f), module, Imagine::TRIGGER_OUT));

    auto image = new PicWidget(module);
    image->box.pos = Vec((PANEL_WIDTH - PANEL_IMAGE_WIDTH)/2.f, PANEL_IMAGE_TOP);
    image->box.size = Vec(PANEL_IMAGE_WIDTH, PANEL_IMAGE_HEIGHT);
    addChild(image);

    auto control_row = 280.f;
    addParam(createThemeParamCentered<SmallKnob>(theme, Vec(25.f, control_row), module, Imagine::SPEED_PARAM));
    auto knob =createThemeParamCentered<SmallKnob>(theme, Vec(55.f, control_row), module, Imagine::SPEED_MULT_PARAM);
    knob->snap = true;
    addParam(knob);

    knob = createThemeParamCentered<SmallKnob>(theme, Vec(85.f, control_row), module, Imagine::COMP_PARAM);
    knob->snap = true;
    addParam(knob);

    knob = createThemeParamCentered<SmallKnob>(theme, Vec(115.f, control_row), module, Imagine::PATH_PARAM);
    knob->minAngle = -1.5;
    knob->maxAngle = 1.5;
    knob->snap = true;
    knob->forceLinear = true;
    addParam(knob);

    auto run = createThemeParamCentered<PlayPauseButton>(theme, Vec (265.f, 280.f), module, Imagine::RUN_PARAM);
    //run->momentary = false;
    if (module) {
        run->onClick([module]() {
            module->setPlaying(!module->isPlaying());
        });
    }
    addParam(run);

    auto picButton = new PicButton(theme);
    picButton->center(Vec (285.f, 280.f));
    if (module) {
        picButton->onClick([this, module]() {
            module->loadImageDialog();
        });
    }
    addChild(picButton);
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
        AddScrewCaps(this, getTheme(), COLOR_NONE);
    } else {
        RemoveScrewCaps(this);
    }
}


void ImagineUi::appendContextMenu(Menu *menu)
{
    if (!this->module)
        return;

    auto module = dynamic_cast<Imagine*>(this->module);
    menu->addChild(new MenuSeparator);
    menu->addChild(createCheckMenuItem(
        "Bright image in a dark room", "",
        [=]() { return module->bright_image; },
        [=]() { module->bright_image = !module->bright_image; }));

    AddThemeMenu(menu, this, false, true);
}
