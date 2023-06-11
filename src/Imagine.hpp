#pragma once
#include <rack.hpp>
#include "plugin.hpp"
#include "components.hpp"
#include "dsp.hpp"
#include "pic.hpp"
#include "traversal.hpp"

namespace pachde {
using namespace rack;
using namespace traversal;

const float PANEL_WIDTH = 300.0f; // 20hp
const float PANEL_CENTER = PANEL_WIDTH / 2.0f;
const float PANEL_MARGIN = 5.0f;

const float IMAGE_UNIT = 18.0f; // 18px image unit for 16x9 landscape
const float PANEL_IMAGE_WIDTH = 16.0f * IMAGE_UNIT;
const float PANEL_IMAGE_HEIGHT = 9.0f * IMAGE_UNIT;
const float PANEL_IMAGE_TOP = 17.0f;
const float SECTION_WIDTH = PANEL_WIDTH - PANEL_MARGIN * 2.0f;
const float ORIGIN_X = PANEL_CENTER;
const float ORIGIN_Y = PANEL_IMAGE_TOP + PANEL_IMAGE_HEIGHT;

enum VRange {
    UNIPOLAR,
    BIPOLAR
};
enum ColorComponent {
    LUMINANCE,
    SATURATION,
    HUE,
    R,
    G,
    B,
    ALPHA,
    NUM_COMPONENTS
};

inline const char * ComponentInitial(ColorComponent co) {
    switch (co) {
        default:
        case ColorComponent::LUMINANCE: return "l";
        case ColorComponent::SATURATION: return "s";
        case ColorComponent::HUE: return "h";
        case ColorComponent::R: return "r";
        case ColorComponent::G: return "g";
        case ColorComponent::B: return "b";
        case ColorComponent::ALPHA: return "a";
    }
}

struct Imagine : ThemeModule
{
    enum ParamIds {
        SLEW_PARAM,
        VOLTAGE_RANGE_PARAM,
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
        NUM_OUTPUTS
    };

    Imagine();

    std::atomic<bool> running;
    ITraversal * traversal = nullptr;
    Traversal traversal_id = Traversal::SCANLINE;
    ColorComponent color_component = ColorComponent::LUMINANCE;
    VRange voct_range = VRange::BIPOLAR;
    SlewLimiter x_slew, y_slew, voct_slew;
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
            case ColorComponent::R: return color.r;
            case ColorComponent::G: return color.g;
            case ColorComponent::B: return color.b;
            case ColorComponent::ALPHA: return color.a;
        }
    }
    bool loadImageDialog();

    json_t *dataToJson() override;
    void updateParams();
    void dataFromJson(json_t *root) override;
    void processBypass(const ProcessArgs& args) override;
	void process(const ProcessArgs& args) override;
    void onSampleRateChange() override;
};

struct ImaginePanel : Widget
{
    Theme theme;
    Imagine *module = nullptr;

    ImaginePanel(Imagine *mod, Theme t, Vec size);
    void draw(const DrawArgs &args) override;
};

struct ImagineUi : ModuleWidget, IChangeTheme
{
    ImaginePanel *panel = nullptr;

    ImagineUi(Imagine *module);
    void makeUi(Imagine *module, Theme theme);
    void setTheme(Theme theme) override;
    void appendContextMenu(Menu *menu) override;
};

}