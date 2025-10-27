#include "Imagine.hpp"
#include "imagine_layout.hpp"
#include "services/theme.hpp"
#include "widgets/create-theme-widget.hpp"
#include "widgets/hamburger.hpp"
#include "widgets/logo-widget.hpp"
#include "widgets/pic_button.hpp"
#include "widgets/port.hpp"
#include "widgets/screws.hpp"
#include "widgets/small_push.hpp"
#include "widgets/switch.hpp"
#include "pic_widget.hpp"
using namespace widgetry;
namespace pachde {

ImagineUi::ImagineUi(Imagine *module) : imagine(module)
{
    setModule(module);
    if (imagine) { image_source = imagine; } else { image_source = this; }
    theme_holder = imagine ? imagine : new ThemeBase();
    setTheme(GetPreferredTheme(theme_holder));
    theme_holder->setNotify(this);
}

void ImagineUi::onChangeTheme(ChangedItem item)
{
    switch (item) {
    case ChangedItem::Theme:
        setTheme(GetPreferredTheme(theme_holder));
        break;
    case ChangedItem::MainColor:
        break;
    case ChangedItem::Screws:
        applyScrews(theme_holder->hasScrews());
        break;
    }
}

void ImagineUi::step()
{
    if (imagine && playButton) {
        playButton->pressed = imagine->isPlaying();
    }

    bool changed = theme_holder->pollRackThemeChanged();
    if (imagine) {
        if (!changed && imagine->isDirty()) {
            // sync to cover preset loading
            applyScrews(theme_holder->hasScrews());
            setTheme(GetPreferredTheme(theme_holder));
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
    auto svg_theme = getThemeCache().getTheme(ThemeName(theme));

    box.size = Vec(300.f, RACK_GRID_HEIGHT);
    panel = new ImaginePanel(imagine, theme_holder, box.size);
    setPanel(panel);

    if (!imagine || imagine->hasScrews()) {
        AddScrewCaps(this, theme, colors::NoColor, SCREWS_OUTSIDE);
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
    addInput(createColorInputCentered<ColorPort>(theme, colors::PortViolet, Vec(x, CONTROL_ROW_2), module, Imagine::SPEED_INPUT));
    x += CONTROL_SPACING;
    addInput(createColorInputCentered<ColorPort>(theme, colors::PortLightOrange, Vec(x, CONTROL_ROW_2), module, Imagine::X_INPUT));
    x += CONTROL_SPACING - TIGHT;
    addInput(createColorInputCentered<ColorPort>(theme, colors::PortLightOrange, Vec(x, CONTROL_ROW_2), module, Imagine::Y_INPUT));

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
    addChild(createColorInputCentered<ColorPort>(theme, colors::PortLightLime, Vec(x, CONTROL_ROW_2), module, Imagine::RESET_POS_INPUT));
    x += CONTROL_SPACING;
    addChild(createColorInputCentered<ColorPort>(theme, colors::PortLightViolet, Vec(x, CONTROL_ROW_2), module, Imagine::PLAY_INPUT));
    x += CONTROL_SPACING;
    addChild(createColorInputCentered<ColorPort>(theme, colors::PortPink, Vec(x, CONTROL_ROW_2), module, Imagine::MIN_TRIGGER_INPUT));

    x = CONTROL_START;
    {
        auto speed = Center(createThemeSvgParam<SmallKnob>(&my_svgs, Vec(x, CONTROL_ROW), imagine, Imagine::SPEED_PARAM));
        speed->stepIncrementBy = 0.1;
        addParam(speed);
    }

    x += CONTROL_SPACING;
    auto knob = Center(createThemeSvgParam<SmallKnob>(&my_svgs, Vec(x, CONTROL_ROW), imagine, Imagine::SPEED_MULT_PARAM));
    knob->snap = true;
    addParam(knob);

    x += CONTROL_SPACING;
    knob = Center(createThemeSvgParam<SmallKnob>(&my_svgs, Vec(x, CONTROL_ROW), imagine, Imagine::PATH_PARAM));
    knob->minAngle = -1.75;
    knob->maxAngle = 1.75;
    knob->snap = true;
    knob->forceLinear = true;
    addParam(knob);

    x = box.size.x - CONTROL_START - 3.f * CONTROL_SPACING;
    knob = Center(createThemeSvgParam<SmallKnob>(&my_svgs, Vec(x, CONTROL_ROW), imagine, Imagine::SLEW_PARAM));
    knob->stepIncrementBy = 0.01f;
    addParam(knob);

    x += CONTROL_SPACING;
    knob = Center(createThemeSvgParam<SmallKnob>(&my_svgs, Vec(x, CONTROL_ROW), imagine, Imagine::COMP_PARAM));
    knob->snap = true;
    addParam(knob);

    x += CONTROL_SPACING;
    knob = Center(createThemeSvgParam<SmallKnob>(&my_svgs, Vec(x, CONTROL_ROW), imagine, Imagine::GT_PARAM));
    knob->stepIncrementBy = 0.05f;
    addParam(knob);

    x += CONTROL_SPACING;
    {
        auto gtrate = Center(createThemeSvgParam<SmallKnob>(&my_svgs, Vec(x, CONTROL_ROW), imagine, Imagine::MIN_TRIGGER_PARAM));
        gtrate->stepIncrementBy = .01f;
        addChild(gtrate);
    }

    addOutput(createThemeOutput<ColorPort>(theme, Vec(21.f, OUTPUT_ROW), imagine, Imagine::X_OUT));
    addOutput(createThemeOutput<ColorPort>(theme, Vec(46.f, OUTPUT_ROW), imagine, Imagine::Y_OUT));
    addOutput(createColorOutput<ColorPort>(theme, colors::PortRed,   Vec(71.f, OUTPUT_ROW - 12.f), imagine, Imagine::RED_OUT));
    addOutput(createColorOutput<ColorPort>(theme, colors::PortGreen, Vec(71.f, OUTPUT_ROW + 13.f), imagine, Imagine::GREEN_OUT));
    addOutput(createColorOutput<ColorPort>(theme, colors::PortBlue,  Vec(95.f, OUTPUT_ROW), imagine, Imagine::BLUE_OUT));
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
    logo->ignore_theme_changes = true;
    addChild(widgetry::Center(logo));

    my_svgs.changeTheme(svg_theme);

}

void ImagineUi::setTheme(Theme theme)
{
    if (children.empty()) {
        makeUi(theme);
    } else {
        my_svgs.changeTheme(getThemeCache().getTheme(ThemeName(theme)));
        sendChildrenThemeColor(this, theme, theme_holder->getMainColor());
        sendDirty(this);
    }
}

void ImagineUi::applyScrews(bool screws)
{
    if (screws) {
        if (HaveScrewChildren(this)) return;
        AddScrewCaps(this, theme_holder->getTheme(), colors::NoColor, SCREWS_OUTSIDE);
    } else {
        RemoveScrewCaps(this);
    }
}

void ImagineUi::appendContextMenu(Menu *menu)
{
    if (!this->module) return;
    menu->addChild(createMenuLabel<HamburgerTitle>("#d Imagine"));
    AddThemeMenu(menu, theme_holder, false, true);

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