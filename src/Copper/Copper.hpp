#pragma once
#include "../plugin.hpp"
#include "../components.hpp"
#include "../text.hpp"
#include "../port.hpp"
#include "../dsp.hpp"
#include "hue_widget.hpp"
#include "sl_widget.hpp"

using namespace rack;
namespace pachde {

struct CopperModule: ThemeModule {
    enum ParamIds {
        H_PARAM,
        S_PARAM,
        L_PARAM,
        A_PARAM,
        NUM_PARAMS
    };
    enum InputIds {
        H_INPUT,
        S_INPUT,
        L_INPUT,
        A_INPUT,
        NUM_INPUTS
    };
    enum OutputIds {
        R_OUT,
        G_OUT,
        B_OUT,
        H_OUT,
        S_OUT,
        L_OUT,
        A_OUT,
        POLY_OUT,
        NUM_OUTPUTS
    };
    float hue = .5f;
    float saturation = .5f;
    float lightness = .5f;
    float alpha = 1.0f;
    bool h_uni, s_uni, l_uni, a_uni;
    ControlRateTrigger control_rate;

    CopperModule();

    float getHue() { return hue; }
    float getSaturation() { return saturation; }
    float getLightness() { return lightness; }
    float getAlpha() { return alpha; }

    void setHue(float h);
    void setSaturation(float sat);
    void setLightness(float light);
    void setAlpha(float a);

    NVGcolor getChosenColor();
    NVGcolor getModulatedColor();

    bool inputsConnected();
    bool hslInputsConnected();
    bool rgbOutputsConnected();
    bool hslOutputsConnected();
    void updateParams();

    json_t* dataToJson() override;
    void dataFromJson(json_t* root) override;
    void onSampleRateChange() override;
    void process(const ProcessArgs& args) override;
};

struct CopperUi : ModuleWidget, ThemeBase
{
    CopperModule* copper_module = nullptr;
    HueWidget* hue_picker = nullptr;
    SLWidget* sl_picker = nullptr; 
    NVGcolor last_color = COLOR_NONE;

    CopperUi(CopperModule * module);

    float getHue();
    void setHue(float hue);
    float getSaturation();
    void setSaturation(float sat);
    float getLightness();
    void setLightness(float light);
    float getAlpha();
    void setAlpha(float alpha);
    NVGcolor getColor();
    void makeUi(Theme theme);

    Theme getTheme() override;
    void setTheme(Theme theme) override;
    void setScrews(bool screws) override;
    bool hasScrews() override;

    void draw(const DrawArgs& args) override;
    void step() override;
    void appendContextMenu(rack::ui::Menu* menu) override;
};

}