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

const float ONE_HP = 15.0f;
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

struct PixelSample {
    int x;
    int y;
    NVGcolor color;

    PixelSample() : x(-1), y(-1), color(nvgRGBA(0,0,0, 0)) {}
    PixelSample(int a, int b, NVGcolor col) : x(a), y(b), color(col) {}
};

struct Imagine : ThemeModule
{
    enum ParamIds {
        SLEW_PARAM,
        VOCT_RANGE_PARAM,
        RUN_PARAM,
        PATH_PARAM,
        //SPEED_PARAM,
        NUM_PARAMS
    };
    enum InputIds {
        //SPEED_INPUT,
        NUM_INPUTS
    };
    enum OutputIds {
        X_OUT,
        Y_OUT,
        VOCT_OUT,
        NUM_OUTPUTS
    };

    Imagine();

    bool running = false;
    int scan_x;
    int scan_y;
    Traversal traversal_id = Traversal::SCANLINE;
    ITraversal * traversal = nullptr;
    ControlRateTrigger pix_rate;

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

    bool loadImageDialog();

    json_t *dataToJson() override;
    void updateParams();
    void dataFromJson(json_t *root) override;
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