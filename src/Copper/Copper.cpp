#include "Copper.hpp"

namespace pachde {
using namespace rack;

const NVGcolor COPPER = nvgHSLAf(40.f/360.f, .85f, .5f, 1.f);

CopperModule::CopperModule()
{
    config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, 0);
    configParam(H_PARAM, 0.f, 1.f, hue, "Hue", "\u00B0", 0.f, 360.0f, 0.f);
    configParam(S_PARAM, 0.f, 1.f, saturation, "Saturation", "");
    configParam(L_PARAM, 0.f, 1.f, lightness, "Lightness", "");
    configParam(A_PARAM, 0.f, 1.f, alpha, "Alpha (transparency)", "");

    configInput(H_INPUT, "Hue");
    configInput(S_INPUT, "Saturation");
    configInput(L_INPUT, "Lightness");
    configInput(A_INPUT, "Alpha (transparency)");

    configOutput(R_OUT, "Red");
    configOutput(G_OUT, "Green");
    configOutput(B_OUT, "Blue");
    configOutput(H_OUT, "Hue");
    configOutput(S_OUT, "Saturation");
    configOutput(L_OUT, "Lightness");
    configOutput(A_OUT, "Alpha (transparency)");
    configOutput(POLY_OUT, "Polyphonic hsla,rgb");

    configBypass(H_INPUT, H_OUT);
    configBypass(S_INPUT, S_OUT);
    configBypass(L_INPUT, L_OUT);
    configBypass(A_INPUT, A_OUT);
}

NVGcolor CopperModule::getChosenColor() {
    return nvgHSLAf(hue, saturation, lightness, alpha);
}

NVGcolor CopperModule::getModulatedColor() {
    return hslInputsConnected()
        ? nvgHSLAf(
            outputs[H_OUT].getVoltage() / 10.f,
            outputs[S_OUT].getVoltage() / 10.f,
            outputs[L_OUT].getVoltage() / 10.f,
            outputs[A_OUT].getVoltage() / 10.f)
        : getChosenColor();
}

void CopperModule::setHue(float h)
{
    if (hue != h) {
        hue = h;
        getParam(H_PARAM).setValue(hue);
    }
}

void CopperModule::setSaturation(float sat) {
    sat = rack::math::clamp(sat);
    if (saturation != sat) {
        saturation = sat;
        getParam(S_PARAM).setValue(saturation);
    }
}

void CopperModule::setLightness(float light) {
    light = rack::math::clamp(light);
    if (lightness != light) {
        lightness = light;
        getParam(L_PARAM).setValue(lightness);
    }
}

void CopperModule::setAlpha(float a) {
    a = rack::math::clamp(a);
    if (a != alpha) {
        alpha = a;
        getParam(A_PARAM).setValue(alpha);
    }
}

void CopperModule::onRandomize(const RandomizeEvent& e) //override
{
    setHue(random::uniform());
    setSaturation(random::uniform());
    setLightness(random::uniform());
    setAlpha(random::uniform());
}

json_t* CopperModule::dataToJson()
{
    json_t *root = ThemeModule::dataToJson();
    json_object_set_new(root, "hue", json_real(hue));
    json_object_set_new(root, "sat", json_real(saturation));
    json_object_set_new(root, "light", json_real(lightness));
    json_object_set_new(root, "alpha", json_real(alpha));
    return root;
}

void CopperModule::dataFromJson(json_t* root)
{
    hue = GetFloat(root, "hue", hue);
    saturation = GetFloat(root, "sat", saturation);
    lightness = GetFloat(root, "light", lightness);
    alpha = GetFloat(root, "alpha", alpha);
    ThemeModule::dataFromJson(root);
    dirty_settings = true;
}

void CopperModule::onSampleRateChange()
{
    control_rate.onSampleRateChanged();
    updateParams();
}

bool CopperModule::hslInputsConnected() {
    return inputs[H_INPUT].isConnected()
        || inputs[S_INPUT].isConnected()
        || inputs[L_INPUT].isConnected();
}
bool CopperModule::inputsConnected() {
    return inputs[A_INPUT].isConnected() || hslInputsConnected();
}
bool CopperModule::rgbOutputsConnected() {
    return outputs[POLY_OUT].isConnected()
        || outputs[R_OUT].isConnected()
        || outputs[G_OUT].isConnected()
        || outputs[B_OUT].isConnected();
}
bool CopperModule::hslOutputsConnected() {
    return outputs[POLY_OUT].isConnected()
        || outputs[H_OUT].isConnected()
        || outputs[S_OUT].isConnected()
        || outputs[L_OUT].isConnected();
}

void CopperModule::updateParams()
{
    hue = getParam(H_PARAM).getValue();
    saturation = getParam(S_PARAM).getValue();
    lightness = getParam(L_PARAM).getValue();
    alpha = getParam(A_PARAM).getValue();
}

static inline bool detectUnipolar(bool state, float v)
{
    if (v < 0.0f) { return false; }
    if (v > 5.f) { return true; }
    return state;
}

void CopperModule::process(const ProcessArgs& args)
{
    if (control_rate.process()) {
        updateParams();
    }
    if (isBypassed()) return;

    float v,h,s,l,a;
    if (inputs[H_INPUT].isConnected()) {
        v = inputs[H_INPUT].getVoltage();
        h_uni = detectUnipolar(h_uni, v);
        if (h_uni) { v -= 5.f; }
        v = hue + v * .1f;
        // wrap if out of range
        if (v < 0) {
            v = 1.f - v;
        } else if (v > 1.0f) {
            v = v - 1.f;
        }
        h = rack::math::clamp(v);
    } else {
        h = hue;
    }
    if (inputs[S_INPUT].isConnected()) {
        v = inputs[H_INPUT].getVoltage();
        s_uni = detectUnipolar(s_uni, v);
        if (s_uni) { v -= 5.f; }
        s = rack::math::clamp(saturation + v * .1f);
    } else {
        s = saturation;
    }
    if (inputs[L_INPUT].isConnected()) {
        v = inputs[L_INPUT].getVoltage();
        l_uni = detectUnipolar(l_uni, v);
        if (l_uni) { v -= 5.f; }
        l = rack::math::clamp(lightness + v * .1f);
    } else {
        l = lightness;
    }
    if (inputs[A_INPUT].isConnected()) {
        v = inputs[A_INPUT].getVoltage();
        a_uni = detectUnipolar(a_uni, v);
        if (a_uni) { v -= 5.f; }
        a = rack::math::clamp(alpha + v * .1f);
    } else {
        a = alpha;
    }

    NVGcolor rgb;
    rgb.a = 0.f;
    if (rgbOutputsConnected()) {
        rgb = nvgHSLAf(h,s,l,a);
        outputs[R_OUT].setVoltage(rgb.r * 10.f);
        outputs[G_OUT].setVoltage(rgb.g * 10.f);
        outputs[B_OUT].setVoltage(rgb.b * 10.f);
    }

    outputs[H_OUT].setVoltage(h * 10.f);
    outputs[S_OUT].setVoltage(s * 10.f);
    outputs[L_OUT].setVoltage(l * 10.f);
    outputs[A_OUT].setVoltage(a * 10.f);

    if (outputs[POLY_OUT].isConnected()) {
        if (rgb.a == 0.f) {
            rgb = nvgHSLAf(h,s,l,a);
        }
        outputs[POLY_OUT].voltages[0] = h * 10.f;
        outputs[POLY_OUT].voltages[1] = s * 10.f;
        outputs[POLY_OUT].voltages[2] = l * 10.f;
        outputs[POLY_OUT].voltages[3] = a * 10.f;
        outputs[POLY_OUT].voltages[4] = rgb.r * 10.f;
        outputs[POLY_OUT].voltages[5] = rgb.g * 10.f;
        outputs[POLY_OUT].voltages[6] = rgb.b * 10.f;
        outputs[POLY_OUT].setChannels(7);
    }
    // if (inputs_connected) {
    //     hue = h;
    //     saturation = s;
    //     lightness = l;
    //     alpha = a;
    // }
}

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

    CopperColor(CopperModule* module) {
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

void AddColorItem(CopperUi* self, Menu* menu, const char * name, PackedColor color, PackedColor current)
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
    if (!this->module) return;
    AddThemeMenu(menu, theme_holder, false, true);
    menu->addChild(createSubmenuItem("Palette color", "",
        [=](Menu *menu)
        {
            auto current = toPacked(getColor());
            for (auto pco = stock_colors; nullptr != pco->name; ++pco) {
                AddColorItem(this, menu, pco->name, pco->color, current);
            }
        })); 
    menu->addChild(createMenuItem("Copy hex color", "", [=]() {
        auto hex = rack::color::toHexString(getColor());
        glfwSetClipboardString(nullptr, hex.c_str());
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

Model *modelCopper = createModel<pachde::CopperModule, pachde::CopperUi>("pachde-copper");
