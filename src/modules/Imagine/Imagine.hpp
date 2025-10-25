#pragma once
#include <ghc/filesystem.hpp>
#include "../myplugin.hpp"
#include "../services/dsp.hpp"
#include "../services/theme-module.hpp"
#include "../widgets/components.hpp"
#include "../widgets/pic.hpp"
#include "traversal.hpp"
#include "play_pause.hpp"

namespace fs = ghc::filesystem;
//using namespace widgetry;

// feature flags
// Slew XY: apply slew to X/Y outputs
#undef XYSLEW // XY are raw outputs

namespace pachde {

using namespace ::rack;
using namespace ::traversal;

std::string MakeUnPluginPath(std::string path);

struct IProvideImage {
     virtual Pic* getImage() = 0;
};

enum VRange {
    UNIPOLAR = 0,
    BIPOLAR = 1
};
enum ColorComponent {
    LIGHTNESS, LUMINANCE, SATURATION, HUE, MIN, MAX, AVE, R, G, B, ALPHA,
    NUM_COMPONENTS
};

inline const char * ComponentShortName(ColorComponent co) {
    switch (co) {
        default:
        case ColorComponent::LIGHTNESS: return "L";
        case ColorComponent::LUMINANCE: return "Lu";
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

struct Imagine : ThemeModule, IProvideImage
{
    enum ParamIds {
        SLEW_PARAM,
        POLARITY_PARAM,
        RUN_PARAM,
        PATH_PARAM,
        SPEED_PARAM,
        SPEED_MULT_PARAM,
        COMP_PARAM,
        GT_PARAM,
        MIN_TRIGGER_PARAM,
        NUM_PARAMS
    };
    enum InputIds {
        PLAY_INPUT,
        RESET_POS_INPUT,
        X_INPUT,
        Y_INPUT,
        SPEED_INPUT,
        MIN_TRIGGER_INPUT,
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
        TEST_OUT,
        NUM_OUTPUTS
    };

    Imagine();

    std::atomic<bool> running;
    Vec reset_pos = Vec(-1.f, -1.f);
    ITraversal * traversal = nullptr;
    Traversal traversal_id = Traversal::SCANLINE;
    ColorComponent color_component = ColorComponent::LIGHTNESS;
    VRange polarity = VRange::BIPOLAR;
    SlewLimiter voltage_slew;
#ifdef XYSLEW
    SlewLimiter x_slew, y_slew;
#endif
    RateTrigger control_rate;

    rack::dsp::SchmittTrigger play_trigger;
    rack::dsp::SchmittTrigger resetpos_trigger;
    rack::dsp::PulseGenerator trigger_pulse;
    LookbackBuffer<float, 6> lookback;
    bool gate_high = false;
    float gt = 0;
    RateTrigger min_retrigger;

    bool medallion_fill = true;
    bool labels = true;
    bool bright_image = false;

    Pic image;
    std::string pic_folder;
    uint64_t image_size = 0.f;
    fs::file_time_type image_time;

    Pic* getImage() override { return &image; }

    void play() { running = true;}
    void pause() { running = false; }
    bool setPlaying(bool play);
    bool isPlaying() { return running; }

    const Vec & getResetPos() { return reset_pos; }
    void setResetPos(Vec pos) {
        resetpos_trigger.reset();
        reset_pos = pos;
    }
    void setResetPos(float x, float y) {
        reset_pos = Vec(x, y);
    }

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
            case ColorComponent::LIGHTNESS: return Lightness(color);
            case ColorComponent::LUMINANCE: return LuminanceLinear(color);
            case ColorComponent::SATURATION: return Saturation(color);
            case ColorComponent::HUE: return Hue1(color);
            case ColorComponent::MIN: return std::min(color.r, std::min(color.g, color.b));
            case ColorComponent::MAX: return std::max(color.r, std::max(color.g, color.b));
            case ColorComponent::AVE: return (color.r + color.g + color.b) / 3.f;
            case ColorComponent::R: return color.r;
            case ColorComponent::G: return color.g;
            case ColorComponent::B: return color.b;
            case ColorComponent::ALPHA: return color.a;
        }
    }
    bool isXYInput() {
        return inputs[X_INPUT].isConnected() && inputs[Y_INPUT].isConnected();
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
    bool loadImage(std::string path);
    bool reloadImage();
    void closeImage();

    bool dirty_settings = false;
    bool isDirty() { return dirty_settings; }
    void setClean() { dirty_settings = false; }

    json_t* dataToJson() override;
    void dataFromJson(json_t* root) override;
    void updateParams();
    void processBypass(const ProcessArgs& args) override;
	void process(const ProcessArgs& args) override;
    void onSampleRateChange() override;
};

struct ImaginePanel : Widget
{
    Imagine* module{nullptr};
    ThemeBase* theme_holder{nullptr};

    ImaginePanel(Imagine* mod, ThemeBase* theme, Vec size);

    enum TraversalDrawOptions { Frame = 1, Text = 2, Both = 3 };
    void drawTraversal(const DrawArgs &args, TraversalDrawOptions options);
    void drawLayer(const DrawArgs &args, int layer) override;
    void draw(const DrawArgs &args) override;
};

struct ImagineUi : ModuleWidget, IThemeChange, IProvideImage
{
    ImaginePanel* panel{nullptr};
    Imagine* imagine{nullptr};
    ThemeBase* theme_holder{nullptr};
    PlayPauseButton* playButton{nullptr};
    Pic* preview_image{nullptr};
    IProvideImage* image_source{nullptr};
    SvgCache my_svgs;

    explicit ImagineUi(Imagine *module);
    virtual ~ImagineUi() {
        if (preview_image) {
            delete preview_image;
        }
        if (!imagine && theme_holder) {
            delete theme_holder;
        }
    }
    void resetHeadPosition(bool ctrl, bool shift);
    void makeUi(Theme theme);
    void setTheme(Theme theme);
    void applyScrews(bool screws);
    Pic * getImage() override;
    void onChangeTheme(ChangedItem item) override;
    void step() override;
    void appendContextMenu(rack::ui::Menu* menu) override;
};

}