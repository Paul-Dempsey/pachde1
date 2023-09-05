#pragma once
#include <rack.hpp>
#include "../components.hpp"
#include "../dsp.hpp"
#include "../IHaveColor.hpp"
#include "../port.hpp"
#include "../text.hpp"
#include "hue_widget.hpp"
#include "sl_widget.hpp"

using namespace ::rack;

namespace pachde {

extern const NVGcolor COPPER;

struct CopperModule: ThemeModule, IHaveColor {
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
    bool h_uni = true, s_uni = true, l_uni = true, a_uni = true;
    RateTrigger control_rate;
    bool is_copper_input[NUM_INPUTS] = {false};
    bool ports_changed = false;

    CopperModule();

    void onPortChange(const PortChangeEvent& e) override
    {
        if (e.type != engine::Port::Type::INPUT) return;
        ports_changed = true;
    }

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

    // IHaveColor
    NVGcolor getColor(int id) override
    {
        switch (id) {
        case 0: return getChosenColor();
        case 1: return getModulatedColor();
        }
        return IHaveColor::getColor(id);
    }

    bool inputsConnected();
    bool hslInputsConnected();
    bool rgbOutputsConnected();
    bool hslOutputsConnected();
    void updateCableConnections();
    void updateParams();

    bool dirty_settings = false;
    bool isDirty() { return dirty_settings; }
    void setClean() { dirty_settings = false; }
    void onRandomize(const RandomizeEvent& e) override;
    json_t* dataToJson() override;
    void dataFromJson(json_t* root) override;
    void onSampleRateChange() override;
    void process(const ProcessArgs& args) override;
};

struct CopperUi : ModuleWidget, IThemeChange
{
    CopperModule* copper_module = nullptr;
    HueWidget* hue_picker = nullptr;
    SLWidget* sl_picker = nullptr; 
    NVGcolor last_color = COLOR_NONE;
    ThemeBase* theme_holder = nullptr;
    
    explicit CopperUi(CopperModule * module);
    virtual ~CopperUi() {
        if (theme_holder && !module) {
            delete theme_holder;
        }
    }

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

    void applyTheme(Theme theme);
    void applyScrews(bool screws);
    void onChangeTheme(ChangedItem item) override;

    void draw(const DrawArgs& args) override;
    void step() override;
    void appendContextMenu(rack::ui::Menu* menu) override;
};

struct CopperMiniUI  : ModuleWidget, IThemeChange {
    CopperModule* copper_module = nullptr;
    ThemeBase* theme_holder = nullptr;

    explicit CopperMiniUI(CopperModule * module);
    virtual ~CopperMiniUI() {
        if (theme_holder && !module) {
            delete theme_holder;
        }
    }
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
    void applyTheme(Theme theme);
    void onChangeTheme(ChangedItem item) override;
    void drawLayer(const DrawArgs& args, int layer) override;
    void draw(const DrawArgs& args) override;
    void step() override;
    void appendContextMenu(rack::ui::Menu* menu) override;

};


}