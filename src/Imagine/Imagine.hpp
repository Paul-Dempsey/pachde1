#pragma once
#include "../components.hpp"
#include "../plugin.hpp"
#include "../dsp.hpp"
#include "pic.hpp"
#include "traversal.hpp"

// feature flags
// Slew XY: apply slew to X/Y outputs
#undef XYSLEW // XY are raw outputs

namespace pachde {

using namespace rack;
using namespace traversal;


enum VRange {
    UNIPOLAR = 0,
    BIPOLAR = 1
};
enum ColorComponent {
    LUMINANCE, SATURATION, HUE, MIN, MAX, AVE, R, G, B, ALPHA,
    NUM_COMPONENTS
};

inline const char * ComponentInitial(ColorComponent co) {
    switch (co) {
        default:
        case ColorComponent::LUMINANCE: return "L";
        case ColorComponent::SATURATION: return "S";
        case ColorComponent::HUE: return "H";
        case ColorComponent::MIN: return "min";
        case ColorComponent::MAX: return "max";
        case ColorComponent::AVE: return "ave";
        case ColorComponent::R: return "R";
        case ColorComponent::G: return "G";
        case ColorComponent::B: return "B";
        case ColorComponent::ALPHA: return "a";
    }
}

struct Imagine : ThemeModule
{
    enum ParamIds {
        SLEW_PARAM,
        POLARITY_PARAM,
        RUN_PARAM,
        PATH_PARAM,
        SPEED_PARAM,
        SPEED_MULT_PARAM,
        COMP_PARAM,
        NUM_PARAMS
    };
    enum InputIds {
        //SPEED_INPUT,
        NUM_INPUTS
    };
    enum OutputIds {
        X_OUT,
        Y_OUT,
        VOLTAGE_OUT,
        GATE_OUT,
        TRIGGER_OUT,
        RED_OUT,
        GREEN_OUT,
        BLUE_OUT,
        NUM_OUTPUTS
    };

    Imagine();

    std::atomic<bool> running;
    ITraversal * traversal = nullptr;
    Traversal traversal_id = Traversal::SCANLINE;
    ColorComponent color_component = ColorComponent::LUMINANCE;
    VRange polarity = VRange::BIPOLAR;
    SlewLimiter voltage_slew;
#ifdef XYSLEW
    SlewLimiter x_slew, y_slew;
#endif
    ControlRateTrigger control_rate;

    bool bright_image = false;
    Pic image;
    std::string pic_folder;

    Pic* getImage() { return &image; }

    void play() { running = true;}
    void pause() { running = false; }
    bool setPlaying(bool play) {
        bool previous = running;
        running = play;
        return previous;
    }
    bool isPlaying() { return running; }

    float getSpeed() {
        return getParam(SPEED_PARAM).getValue() * getParam(SPEED_MULT_PARAM).getValue();
    }
    Traversal getTraversalId() {
        auto pp = getParamQuantity(PATH_PARAM);
        return static_cast<Traversal>(static_cast<int>(std::floor(pp->getValue() - pp->getMinValue())));
    }
    bool isXYPad() {
        return Traversal::XYPAD == getTraversalId();
    }
    bool isBipolar() {
        return polarity == VRange::BIPOLAR;
    }
    bool isUnipolar() {
        return polarity == VRange::UNIPOLAR;
    }
    ColorComponent getColorComponent() {
        auto pp = getParamQuantity(COMP_PARAM);
        return static_cast<ColorComponent>(static_cast<int>(std::floor(pp->getValue() - pp->getMinValue())));
    }
    float ComponentValue(NVGcolor color) {
        switch (color_component) {
            default:
            case ColorComponent::LUMINANCE: return LuminanceLinear(color);
            case ColorComponent::SATURATION: return Saturation(color);
            case ColorComponent::HUE: return Hue(color);
            case ColorComponent::MIN: return std::min(color.r, std::min(color.g, color.b));
            case ColorComponent::MAX: return std::max(color.r, std::max(color.g, color.b));
            case ColorComponent::AVE: return (color.r + color.g + color.b) / 3.f;
            case ColorComponent::R: return color.r;
            case ColorComponent::G: return color.g;
            case ColorComponent::B: return color.b;
            case ColorComponent::ALPHA: return color.a;
        }
    }
    bool isPixelOutput() {
        return 0 < outputs[VOLTAGE_OUT].isConnected()
            + outputs[GATE_OUT].isConnected()
            + outputs[TRIGGER_OUT].isConnected()
            + outputs[RED_OUT].isConnected()
            + outputs[GREEN_OUT].isConnected()
            + outputs[BLUE_OUT].isConnected();
    }
    bool loadImageDialog();

    json_t* dataToJson() override;
    void dataFromJson(json_t* root) override;
    void updateParams();
    void processBypass(const ProcessArgs& args) override;
	void process(const ProcessArgs& args) override;
    void onSampleRateChange() override;
};

struct ImaginePanel : Widget
{
    Imagine* module = nullptr;

    ImaginePanel(Imagine* mod, Vec size);
    void draw(const DrawArgs &args) override;
};

struct ImagineUi : ModuleWidget, ThemeBase
{
    ImaginePanel *panel = nullptr;
    Imagine* imagine = nullptr;

    ImagineUi(Imagine *module);

    void makeUi(Imagine* module, Theme theme);
    void setTheme(Theme theme) override;
    void setScrews(bool screws) override;
    Theme getTheme() override {
        if (imagine) return imagine->getTheme();
        return ThemeBase::getTheme();
    }
    bool hasScrews() override {
        if (imagine) return imagine->hasScrews();
        return ThemeBase::hasScrews();
    }
    void appendContextMenu(rack::ui::Menu* menu) override;
};

}