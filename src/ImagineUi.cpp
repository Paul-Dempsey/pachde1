#include "Imagine.hpp"
#include "pic_widget.hpp"
#include "themehelpers.hpp"

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
    CreateScrews(this, theme, ScrewCap::Brightness::Less);
    addChild(createThemeWidgetCentered<LogoWidget>(theme, Vec(box.size.x / 2.0f, RACK_GRID_HEIGHT - RACK_GRID_WIDTH + 7.5f)));

    addOutput(createThemeOutput<BluePort>(theme, Vec(15.0f, 305.0f), module, Imagine::X_OUT));
    addOutput(createThemeOutput<BluePort>(theme, Vec(40.0f, 305.0f), module, Imagine::Y_OUT));

    addParam(createThemeParamCentered<SmallKnob>(theme, Vec(82.5f, 315.0f), module, Imagine::SLEW_PARAM));
    addOutput(createThemeOutput<BluePort>(theme, Vec(100.0f, 305.0f), module, Imagine::VOLTAGE_OUT));

    addParam(createParam<CKSS>(Vec(125.0f, 305.0f), module, Imagine::VOLTAGE_RANGE_PARAM));

    auto image = new PicWidget(module);
    image->box.pos = Vec(6, 17);
    image->box.size = Vec(288, 162);
    addChild(image);


    auto run = createThemeParamCentered<PLayPauseButton>(theme, Vec (150.0f, 190.0f), module, Imagine::RUN_PARAM);
    run->momentary = false;
    if (module) {
        run->onClick([module]() {
            module->setPlaying(!module->isPlaying());
        });
    }
    addParam(run);

    addParam(createThemeParamCentered<SmallKnob>(theme, Vec(20.0f, 215.0f), module, Imagine::SPEED_PARAM));
    auto p =createThemeParamCentered<SmallKnob>(theme, Vec(50.0f, 215.0f), module, Imagine::SPEED_MULT_PARAM);
    p->snap = true;
    addParam(p);

    auto pathKnob = createThemeParamCentered<SmallKnob>(theme, Vec(80.0f, 215.0f), module, Imagine::PATH_PARAM);
    pathKnob->minAngle = -1;
    pathKnob->maxAngle = 1;
    pathKnob->snap = true;
   //pathKnob->forceLinear = true;
    addParam(pathKnob);

    auto picButton = new PicButton(theme);
    picButton->center(Vec (285.0f, 188.0f));
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
        SetThemeChildren(this, theme);
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

    auto themeModule = dynamic_cast<ThemeModule *>(this->module);
    themeModule->addThemeMenu(menu, dynamic_cast<IChangeTheme *>(this));
}
