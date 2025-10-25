#include "Copper.hpp"
#include "services/json-help.hpp"

namespace pachde {
using namespace rack;

const NVGcolor COPPER = nvgHSLAf(40.f/360.f, .85f, .5f, 1.f);
const PackedColor COPPER_PACKED = toPacked(COPPER);

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
    configOutput(POLY_OUT, "Polyphonic color component");

    configBypass(H_INPUT, H_OUT);
    configBypass(S_INPUT, S_OUT);
    configBypass(L_INPUT, L_OUT);
    configBypass(A_INPUT, A_OUT);
}

NVGcolor CopperModule::getChosenColor() {
    return nvgHSLAf(hue, saturation, lightness, alpha);
}

NVGcolor CopperModule::getModulatedColor() {
    return inputsConnected()
        ? nvgHSLAf(
            getOutput(H_OUT).getVoltage() / 10.f,
            getOutput(S_OUT).getVoltage() / 10.f,
            getOutput(L_OUT).getVoltage() / 10.f,
            getOutput(A_OUT).getVoltage() / 10.f)
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
    set_json(root, "hue", hue);
    set_json(root, "sat", saturation);
    set_json(root, "light", lightness);
    set_json(root, "alpha", alpha);
    return root;
}

void CopperModule::dataFromJson(json_t* root)
{
    hue = get_json_float(root, "hue", hue);
    saturation = get_json_float(root, "sat", saturation);
    lightness = get_json_float(root, "light", lightness);
    alpha = get_json_float(root, "alpha", alpha);
    ThemeModule::dataFromJson(root);
    dirty_settings = true;
}

void CopperModule::onSampleRateChange()
{
    control_rate.onSampleRateChanged();
    updateParams();
}

bool CopperModule::hslInputsConnected() {
    return getInput(H_INPUT).isConnected()
        || getInput(S_INPUT).isConnected()
        || getInput(L_INPUT).isConnected();
}
bool CopperModule::inputsConnected() {
    return getInput(A_INPUT).isConnected() || hslInputsConnected();
}
bool CopperModule::rgbOutputsConnected() {
    return getOutput(POLY_OUT).isConnected()
        || getOutput(R_OUT).isConnected()
        || getOutput(G_OUT).isConnected()
        || getOutput(B_OUT).isConnected();
}
bool CopperModule::hslOutputsConnected() {
    return getOutput(POLY_OUT).isConnected()
        || getOutput(H_OUT).isConnected()
        || getOutput(S_OUT).isConnected()
        || getOutput(L_OUT).isConnected();
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

void CopperModule::updateCableConnections()
{
    if (!ports_changed) return;

    ports_changed = false;
    for (auto i = 0; i < NUM_INPUTS; ++i) {
        is_copper_input[i] = false;
    }
    int64_t ids[200];
    size_t n = APP->engine->getCableIds(ids,200);
    for (size_t i = 0; i < n; ++i) {
        auto cable = APP->engine->getCable(ids[i]);
        if (cable->inputModule == this) {
            auto model = cable->outputModule->getModel();
            if (model == modelCopper || model == modelCopperMini) {
                is_copper_input[cable->inputId] = true;
            }
        }
    }
}

void CopperModule::process(const ProcessArgs& args)
{
    if (control_rate.process()) {
        updateParams();
    }
    if (isBypassed()) return;

    float v,h,s,l,a;
    if (getInput(H_INPUT).isConnected()) {
        v = getInput(H_INPUT).getVoltage();
        if (is_copper_input[H_INPUT]) {
            h = v * .1f;
        } else {
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
        }
    } else {
        h = hue;
    }
    if (getInput(S_INPUT).isConnected()) {
        v = getInput(S_INPUT).getVoltage();
        if (is_copper_input[S_INPUT]) {
            s = v * .1f;
        } else {
            s_uni = detectUnipolar(s_uni, v);
            if (s_uni) { v -= 5.f; }
            s = rack::math::clamp(saturation + v * .1f);
        }
    } else {
        s = saturation;
    }
    if (getInput(L_INPUT).isConnected()) {
        v = getInput(L_INPUT).getVoltage();
        if (is_copper_input[L_INPUT]) {
            l = v * .1f;
        } else {
            l_uni = detectUnipolar(l_uni, v);
            if (l_uni) { v -= 5.f; }
            l = rack::math::clamp(lightness + v * .1f);
        }
    } else {
        l = lightness;
    }
    if (getInput(A_INPUT).isConnected()) {
        v = getInput(A_INPUT).getVoltage();
        if (is_copper_input[A_INPUT]) {
            a = v * .1f;
        } else {
            a_uni = detectUnipolar(a_uni, v);
            if (a_uni) { v -= 5.f; }
            a = rack::math::clamp(alpha + v * .1f);
        }
    } else {
        a = alpha;
    }

    NVGcolor rgb;
    rgb.a = 0.f;
    if (rgbOutputsConnected()) {
        rgb = nvgHSLAf(h,s,l,a);
        getOutput(R_OUT).setVoltage(rgb.r * 10.f);
        getOutput(G_OUT).setVoltage(rgb.g * 10.f);
        getOutput(B_OUT).setVoltage(rgb.b * 10.f);
    }

    getOutput(H_OUT).setVoltage(h * 10.f);
    getOutput(S_OUT).setVoltage(s * 10.f);
    getOutput(L_OUT).setVoltage(l * 10.f);
    getOutput(A_OUT).setVoltage(a * 10.f);

    if (getOutput(POLY_OUT).isConnected()) {
        if (rgb.a == 0.f) {
            rgb = nvgHSLAf(h,s,l,a);
        }
        auto out = getOutput(POLY_OUT);
        out.setChannels(7);
        if (poly_out_rgbahsl) {
            out.voltages[0] = rgb.r * 10.f;
            out.voltages[1] = rgb.g * 10.f;
            out.voltages[2] = rgb.b * 10.f;
            out.voltages[3] = a * 10.f;
            out.voltages[4] = h * 10.f;
            out.voltages[5] = s * 10.f;
            out.voltages[6] = l * 10.f;
        } else {
            out.voltages[0] = h * 10.f;
            out.voltages[1] = s * 10.f;
            out.voltages[2] = l * 10.f;
            out.voltages[3] = a * 10.f;
            out.voltages[4] = rgb.r * 10.f;
            out.voltages[5] = rgb.g * 10.f;
            out.voltages[6] = rgb.b * 10.f;
        }
    }
    // if (inputs_connected) {
    //     hue = h;
    //     saturation = s;
    //     lightness = l;
    //     alpha = a;
    // }
}

}

Model *modelCopper = createModel<pachde::CopperModule, pachde::CopperUi>("pachde-copper");
