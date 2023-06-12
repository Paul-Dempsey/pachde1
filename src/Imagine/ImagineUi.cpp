#include "Imagine.hpp"
#include "pic_widget.hpp"

ImagineUi::ImagineUi(Imagine *module)
{
    setModule(module);
    setTheme(ModuleTheme(module));
}

void ImagineUi::makeUi(Imagine *module, Theme theme)
{
    assert(children.empty());

    box.size = Vec(300, RACK_GRID_HEIGHT);
    panel = new ImaginePanel(module, theme, box.size);
    setPanel(panel);
    AddScrewCaps(this, theme, COLOR_NONE);
    addChild(createThemeWidgetCentered<LogoWidget>(theme, Vec(box.size.x / 2.f, RACK_GRID_HEIGHT - ONE_HP + 7.5f)));

    addOutput(createThemeOutput<BluePort>(theme, Vec(15.f, 320.f), module, Imagine::X_OUT));
    addOutput(createThemeOutput<BluePort>(theme, Vec(40.f, 320.f), module, Imagine::Y_OUT));

    addParam(createThemeParamCentered<SmallKnob>(theme, Vec(82.5f, 330.f), module, Imagine::SLEW_PARAM));
    addOutput(createThemeOutput<BluePort>(theme, Vec(100.f, 320.f), module, Imagine::VOLTAGE_OUT));
    auto p = createThemeParam<Switch>(theme, Vec(130.f, 324.f), module, Imagine::VOLTAGE_RANGE_PARAM);
    p->box.size.y = 18.f;
    addParam(p);

    addOutput(createThemeOutput<BluePort>(theme, Vec(150.f, 320.f), module, Imagine::GATE_OUT));
    addOutput(createThemeOutput<BluePort>(theme, Vec(175.f, 320.f), module, Imagine::TRIGGER_OUT));

    auto image = new PicWidget(module);
    image->box.pos = Vec(6.f, 17.f);
    image->box.size = Vec(288.f, 162.f);
    addChild(image);

    auto run = createThemeParamCentered<PlayPauseButton>(theme, Vec (150.f, 190.f), module, Imagine::RUN_PARAM);
    //run->momentary = false;
    if (module) {
        run->onClick([module]() {
            module->setPlaying(!module->isPlaying());
        });
    }
    addParam(run);

    addParam(createThemeParamCentered<SmallKnob>(theme, Vec(20.f, 215.f), module, Imagine::SPEED_PARAM));
    auto knob =createThemeParamCentered<SmallKnob>(theme, Vec(50.f, 215.f), module, Imagine::SPEED_MULT_PARAM);
    knob->snap = true;
    addParam(knob);

    knob = createThemeParamCentered<SmallKnob>(theme, Vec(80.f, 215.f), module, Imagine::COMP_PARAM);
    knob->snap = true;
    addParam(knob);

    knob = createThemeParamCentered<SmallKnob>(theme, Vec(110.f, 215.f), module, Imagine::PATH_PARAM);
    knob->minAngle = -1.5;
    knob->maxAngle = 1.5;
    knob->snap = true;
    knob->forceLinear = true;
    addParam(knob);

    auto picButton = new PicButton(theme);
    picButton->center(Vec (285.f, 188.f));
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
    if (children.empty()) {
        makeUi(module, theme);
    } else {
        panel->theme = theme;
        SetChildrenTheme(this, theme);
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
        [=]() {
            module->bright_image = !module->bright_image;
        }));

    AddThemeMenu(menu, this, false, true);
}
