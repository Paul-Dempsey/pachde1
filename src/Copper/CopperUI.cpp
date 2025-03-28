#include "Copper.hpp"

namespace pachde {
using namespace rack;

// ----------------------------------------------------------------------------
struct CopperSvg {
    static std::string background(Theme theme)
    {
        const char * asset;
        switch (theme) {
        default:
        case Theme::Unset:
        case Theme::Light:
            asset = "res/Copper.svg";
            break;
        case Theme::Dark:
            asset = "res/CopperDark.svg";
            break;
        case Theme::HighContrast:
            asset = "res/CopperHighContrast.svg";
            break;
        }
        return asset::plugin(pluginInstance, asset);
    }
};

// ----------------------------------------------------------------------------
struct CopperColor : OpaqueWidget {
    CopperModule* module;

    explicit CopperColor(CopperModule* module) {
        this->module = module;
    }

    void onEnter(const EnterEvent &e) override
    {
        OpaqueWidget::onEnter(e);
        glfwSetCursor(APP->window->win, glfwCreateStandardCursor(GLFW_POINTING_HAND_CURSOR));
    }

    void onLeave(const LeaveEvent &e) override
    {
        OpaqueWidget::onLeave(e);
        glfwSetCursor(APP->window->win, NULL);
    }

    void onButton(const event::Button& e) override {
        rack::OpaqueWidget::onButton(e);
        if (!(e.action == GLFW_PRESS
            && e.button == GLFW_MOUSE_BUTTON_LEFT
            && (e.mods & RACK_MOD_MASK) == 0)) {
            return;
        }
        if (module) {
            auto hex = rack::color::toHexString(COPPER);
            glfwSetClipboardString(nullptr, hex.c_str());
            module->setHue(40.f/360.f);
            module->setSaturation(.8f);
            module->setLightness(.5f);
            module->setAlpha(1.f);
        }
    }

    void draw(const DrawArgs& args) override {
        OpaqueWidget::draw(args);
        if (!module) return;
        Circle(args.vg, box.size.x * .5f, box.size.y * .5f, std::min(box.size.x, box.size.y), COPPER);
    }

};


// ----------------------------------------------------------------------------
CopperUi::CopperUi(CopperModule * module)
{
    copper_module = module;
    setModule(module);
    theme_holder = module ? module : new ThemeBase();
    theme_holder->setNotify(this);
    applyTheme(GetPreferredTheme(theme_holder));
}

float CopperUi::getHue()
{
    if (copper_module) {
        return copper_module->getHue();
    }
    return 40.f/360.f;
}
void CopperUi::setHue(float hue) {
    if (copper_module) {
        copper_module->setHue(hue);
    }
}
float CopperUi::getSaturation() {
    if (copper_module) {
        return copper_module->getSaturation();
    }
    return .65f;
}
void CopperUi::setSaturation(float sat) {
    if (copper_module) {
        copper_module->setSaturation(sat);
    }
}
float CopperUi::getLightness() {
    if (copper_module) {
        return copper_module->getLightness();
    }
    return .95f;
}
void CopperUi::setLightness(float light) {
    if (copper_module) {
        copper_module->setLightness(light);
    }
}
float CopperUi::getAlpha() {
    if (copper_module) {
        return copper_module->getAlpha();
    }
    return 1.f;
}
void CopperUi::setAlpha(float alpha) {
    if (copper_module) {
        copper_module->setAlpha(alpha);
    }
}
NVGcolor CopperUi::getColor() {
    if (copper_module) {
        return nvgHSLAf(
            copper_module->getHue(),
            copper_module->getSaturation(),
            copper_module->getLightness(),
            copper_module->getAlpha());
    } else {
        return COPPER;
    }
}

const float col1_center = 25.5f;
const float col2_center = 68.f;
const float col3_center = 109.f;
const float col4_center = 153.f;      
const float row1_middle = 273.f;
const float row2_middle = 300.f;
const float knob_baseline = 290.f;
const float output_row1 = 329.f;
const float output_row2 = 354.f;
const float output_col1 = col1_center;
const float output_col2 = output_col1 + 21.5f;
const float output_col3 = 90.f - 15.f;
const float output_col4 = 90.f + 15.f;
const float output_col5 = col4_center;

void CopperUi::makeUi(Theme theme)
{
    assert(children.empty());

    setPanel(createSvgThemePanel<CopperSvg>(theme));

    if (theme_holder->hasScrews()) {
        AddScrewCaps(this, theme, COPPER, SCREWS_OUTSIDE, WhichScrew::TOP_SCREWS);
        AddScrewCaps(this, theme, COLOR_NONE, SCREWS_OUTSIDE, WhichScrew::BOTTOM_SCREWS);
    }

    if (copper_module) {
        auto cc = new CopperColor(copper_module);
        cc->box.pos = Vec(100.f, 373.f);
        cc->box.size = Vec(1.5f, 1.5f);
        addChild(cc);
    }

    hue_picker = new HueWidget();
    hue_picker->hue = getHue();
    hue_picker->box.pos = Vec(12.f,15.f);
    hue_picker->box.size = Vec(15.f, 224);
    hue_picker->onClick([=](float hue) {
        setHue(hue);
    });
    addChild(hue_picker);

    sl_picker = new SLWidget(getHue());
    sl_picker->setSaturation(getSaturation());
    sl_picker->setLightness(getLightness());
    sl_picker->box.pos = Vec(35.5f, 15.f);
    sl_picker->box.size = Vec(134.f, 224.f);
    sl_picker->onClick([=](float sat, float light) {
        setSaturation(sat);
        setLightness(light);
    });
    addChild(sl_picker);

    auto p = createThemeParamCentered<LargeKnob>(theme, Vec(col1_center,row1_middle), module, CopperModule::H_PARAM);
    p->stepIncrementBy = 1.f/360.f;
    addParam(p);
    p = createThemeParamCentered<LargeKnob>(theme, Vec(col2_center,row1_middle), module, CopperModule::S_PARAM);
    p->stepIncrementBy = .1f;
    addParam(p);
    p = createThemeParamCentered<LargeKnob>(theme, Vec(col3_center,row1_middle), module, CopperModule::L_PARAM);
    p->stepIncrementBy = .01f;
    addParam(p);
    p = createThemeParamCentered<LargeKnob>(theme, Vec(col4_center,row1_middle), module, CopperModule::A_PARAM);
    p->stepIncrementBy = .1f;
    addParam(p);

    addInput(createThemeInputCentered<ColorPort>(theme, Vec(col1_center,row2_middle), module, CopperModule::H_INPUT));
    addInput(createThemeInputCentered<ColorPort>(theme, Vec(col2_center,row2_middle), module, CopperModule::S_INPUT));
    addInput(createThemeInputCentered<ColorPort>(theme, Vec(col3_center,row2_middle), module, CopperModule::L_INPUT));
    addInput(createThemeInputCentered<ColorPort>(theme, Vec(col4_center,row2_middle), module, CopperModule::A_INPUT));

    addOutput(createColorOutputCentered<ColorPort>(theme, PORT_RED,   Vec(output_col1,output_row1), module, CopperModule::R_OUT));
    addOutput(createColorOutputCentered<ColorPort>(theme, PORT_GREEN, Vec(output_col1,output_row2), module, CopperModule::G_OUT));
    addOutput(createColorOutputCentered<ColorPort>(theme, PORT_BLUE,  Vec(output_col2,output_row1 + (output_row2 - output_row1)/2.f), module, CopperModule::B_OUT));

    addOutput(createColorOutputCentered<ColorPort>(theme, PORT_YELLOW, Vec(output_col3,output_row1), module, CopperModule::H_OUT));
    addOutput(createColorOutputCentered<ColorPort>(theme, PORT_ORANGE, Vec(output_col4,output_row1), module, CopperModule::S_OUT));
    addOutput(createColorOutputCentered<ColorPort>(theme, RampGray(G_50), Vec(output_col3,output_row2), module, CopperModule::L_OUT));
    addOutput(createColorOutputCentered<ColorPort>(theme, PORT_PINK, Vec(output_col4,output_row2), module, CopperModule::A_OUT));

    addOutput(createColorOutputCentered<ColorPort>(theme, PORT_MAGENTA, Vec(output_col5, output_row1), module, CopperModule::POLY_OUT));
}

const float sample_x = 0.f;
const float sample_y = 244.f;
const float sample_w = 90.f;
const float sample_check_x = sample_x + sample_w;
const float sample_full_h = 12.f;
const float sample_half_h = 6.f;
const float sample_y2 = sample_y + sample_half_h;

void CopperUi::draw(const DrawArgs& args)
{
    ModuleWidget::draw(args);
    auto vg = args.vg;
    auto color = getColor();
    auto opaque_color = nvgTransRGBAf(color, 1.f);
    bool modulated = copper_module ? copper_module->hslInputsConnected() : false;
    if (modulated) {
        FillRect(vg, sample_check_x, sample_y, sample_w, sample_half_h, color);
        FillRect(vg, sample_x, sample_y, sample_w, sample_half_h, opaque_color);

        auto mod_color = copper_module->getModulatedColor();
        FillRect(vg, sample_check_x, sample_y2, sample_w, sample_half_h, mod_color);
        mod_color.a = 1.f;
        FillRect(vg, sample_x,sample_y2, sample_w, sample_half_h, mod_color);
    } else {
        FillRect(vg, sample_check_x, sample_y, sample_w, sample_full_h, color);
        FillRect(vg, sample_x, sample_y, sample_w, sample_full_h, opaque_color);
    }

    auto theme = GetPreferredTheme(theme_holder);
    auto textColor = ThemeTextColor(theme);
    auto font = GetPluginFontSemiBold();
    if (FontOk(font))
    {
        SetTextStyle(vg, font, textColor);
        nvgFontSize(vg, 12.f);
        CenterText(vg, col1_center - 14.f, knob_baseline, "H", nullptr);
        CenterText(vg, col2_center - 14.f, knob_baseline, "S", nullptr);
        CenterText(vg, col3_center - 14.f, knob_baseline, "L", nullptr);
        CenterText(vg, col4_center - 14.f, knob_baseline, "A", nullptr);

        nvgFillColor(vg, RampGray(G_85));
        nvgFontSize(vg, 10.f);
        CenterText(vg, output_col5, output_row1 + 18.f, "poly", nullptr);
        CenterText(vg, output_col3 -14.f, output_row1 -8.f, "h", nullptr);
        CenterText(vg, output_col4 +14.f, output_row1 -8.f, "s", nullptr);
        CenterText(vg, output_col3 -14.f, output_row2 +8.f, "l", nullptr);
        CenterText(vg, output_col4 +14.f, output_row2 +8.f, "a", nullptr);
    }
    font = GetPluginFontRegular();
    if (FontOk(font)) {
        SetTextStyle(vg, font, IsLighter(theme) ? RampGray(G_10) : COLOR_BRAND_HI);
        nvgFontSize(vg, 10.f);
        auto text = rack::color::toHexString(color);
        RightAlignText(vg, 170., output_row2 + 10.f, text.c_str(), nullptr);
    }
}

void CopperUi::applyTheme(Theme theme)
{
    if (children.empty()) {
        makeUi(theme);
    } else {
        SetChildrenTheme(this, theme);
    }        
}

void CopperUi::applyScrews(bool screws)
{
    if (screws) {
        if (HaveScrewChildren(this)) return;
        auto theme = GetPreferredTheme(theme_holder);
        AddScrewCaps(this, theme, COPPER, SCREWS_OUTSIDE, WhichScrew::TOP_SCREWS);
        AddScrewCaps(this, theme, COLOR_NONE, SCREWS_OUTSIDE, WhichScrew::BOTTOM_SCREWS);
    } else {
        RemoveScrewCaps(this);
    }
}

void CopperUi::onChangeTheme(ChangedItem item)
{
    switch (item) {
    case ChangedItem::Theme:
        applyTheme(GetPreferredTheme(theme_holder));
        break;
    case ChangedItem::DarkTheme:
    case ChangedItem::FollowDark:
        if (theme_holder->getFollowRack()) {
            applyTheme(GetPreferredTheme(theme_holder));
        }
        break;
    case ChangedItem::MainColor:
        break;
    case ChangedItem::Screws:
        applyScrews(theme_holder->hasScrews());
        break;
    }
}

void CopperUi::step()
{
    bool changed = theme_holder->pollRackDarkChanged();

    if (copper_module) {
        // sync with module for change from presets
        if (!changed && copper_module->isDirty()) {
            applyScrews(theme_holder->hasScrews());
            applyTheme(GetPreferredTheme(theme_holder));
        }
        copper_module->setClean();
        copper_module->updateCableConnections();
    }

    auto current = getColor();
    if (!IS_SAME_COLOR(current, last_color)) {
        last_color = current;
        auto h = getHue();
        if (hue_picker) {
            hue_picker->setHue(h);
        }
        if (sl_picker) {
            sl_picker->setSaturation(getSaturation());
            sl_picker->setLightness(getLightness());
            sl_picker->setHue(h);
        }

    }
    ModuleWidget::step();
}

template <typename Self>
void AddColorItem(Self* self, Menu* menu, const char * name, PackedColor color, PackedColor current)
{
    menu->addChild(createColorMenuItem(
        color, name, "",
        [=]() { return current == color; },
        [=]() { 
            auto new_color = fromPacked(color);
            self->setHue(Hue1(new_color));
            self->setSaturation(Saturation(new_color));
            self->setLightness(Lightness(new_color));
            self->setAlpha(new_color.a);
        }
        ));
}

void CopperUi::appendContextMenu(rack::ui::Menu* menu)
{
    if (!copper_module) return;
    AddThemeMenu(menu, theme_holder, false, true);
    menu->addChild(createSubmenuItem("Poly jack order", "", [=](Menu* menu) {
        menu->addChild(createCheckMenuItem("HSLARGB", "", 
            [=](){ return !copper_module->poly_out_rgbahsl; },
            [=](){ copper_module->poly_out_rgbahsl = !copper_module->poly_out_rgbahsl; }
        ));
        menu->addChild(createCheckMenuItem("RGBAHSL", "", 
            [=](){ return copper_module->poly_out_rgbahsl; },
            [=](){ copper_module->poly_out_rgbahsl = !copper_module->poly_out_rgbahsl; }
        ));
    }));
    menu->addChild(createSubmenuItem("Palette color", "",
        [=](Menu *menu)
        {
            auto current = toPacked(getColor());
            for (auto pco = stock_colors; nullptr != pco->name; ++pco) {
                AddColorItem<CopperUi>(this, menu, pco->name, pco->color, current);
            }
        })); 
    menu->addChild(createMenuItem("Copy hex color", "", [=]() {
        auto hex = rack::color::toHexString(getColor());
        glfwSetClipboardString(nullptr, hex.c_str());
    }));
    menu->addChild(createMenuItem("Paste hex color", "", [=]() {
        std::string hex = glfwGetClipboardString(nullptr);
        if (!hex.empty()) {
            auto new_color = rack::color::fromHexString(hex);
            setHue(Hue1(new_color));
            setSaturation(Saturation(new_color));
            setLightness(Lightness(new_color));
            setAlpha(new_color.a);
        }
    }));

#ifdef USE_BAD_HEX_INPUT
    // BUGBUG: We don't have working inverses yet for round-tripping nvgHSL(),
    menu->addChild(createSubmenuItem("Color", "",
        [=](Menu *menu)
        {
            MenuTextField *editField = new MenuTextField();
            editField->box.size.x = 100;
            auto color = getColor();
            editField->setText(rack::color::toHexString(color));
            editField->changeHandler = [=](std::string text) {
                auto color = COLOR_NONE;
                if (!text.empty() && text[0] == '#') {
                    color = rack::color::fromHexString(text);
                    setHue(Hue1(color));
                    setSaturation(Saturation(color));
                    setLightness(Lightness(color));
                    setAlpha(color.a);
                }
            };
            menu->addChild(editField);
        }));
#endif
}

}
