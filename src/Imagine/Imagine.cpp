#include "Imagine.hpp"
#include <osdialog.h>
#include "imagine_layout.hpp"
#include "../components.hpp"
#include "../text.hpp"
#include "../dsp.hpp"

namespace pachde {

Imagine::Imagine() {
    setPlaying(false);
    config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, 0);

    configParam(SLEW_PARAM, 0.f, 1.f, 0.f,
        "Slew", "%", 0.f, 100.f);

    configParam(GT_PARAM, 0.f, 1.f, .3f,
        "Gate/Trigger spread threshhold");

    // configParam(RESETPOS_PARAM, 0.f, 1.f, 0.f,
    //     "Reset position");

    configSwitch(POLARITY_PARAM, 0.f, 1.0f, 0.0f,
        "Polarity", { "Unipolar (0 - 10v)", "Bipolar (-5 - 5v)" });

    configSwitch(RUN_PARAM, 0.f, 1.f, 0.f,
        "Play", { "Paused", "Playing" });

    configInput(RESET_POS_INPUT, "Reset head position trigger");
    configInput(PLAY_INPUT, "Play/Pause trigger");
    configInput(X_INPUT, "Read head X position");
    configInput(Y_INPUT, "Read head Y position");

    configParam(SPEED_PARAM, 0.f, 100.f, 10.f,
        "Speed");

    configSwitch(SPEED_MULT_PARAM, 1.f, 10.f, 1.f,
        "Multiplier", 
        { "x1", "x2", "x3", "x4", "x5", "x6", "x7", "x8", "x9", "x10" });

    configSwitch(PATH_PARAM, 0.0f, static_cast<int>(Traversal::NUM_TRAVERSAL)-1, 0.0f, "Path", {
        "Scanline",
        "Bounce",
        "Vinyl",
        "Wander",
        "X/Y Pad"
    });

    configSwitch(COMP_PARAM, 0.0f, static_cast<int>(ColorComponent::NUM_COMPONENTS)-1, 0.0f, "Component", {
        "Lightness",
        "Luminance",
        "Saturation (chroma)",
        "Hue",
        "Minimum",
        "Maximum",
        "Average",
        "Red",
        "Green",
        "Blue",
        "Alpha (transparency)"
    });
    configOutput(X_OUT, "x");
    configOutput(Y_OUT, "y");
    configOutput(RED_OUT,     "Red");
    configOutput(GREEN_OUT,   "Green");
    configOutput(BLUE_OUT,    "Blue");
    configOutput(VOLTAGE_OUT, "Voltage");
    configOutput(GATE_OUT,    "Gate");
    configOutput(TRIGGER_OUT, "Trigger");
    //configOutput(TEST_OUT, "Test");
    updateParams();
}

bool Imagine::reloadImage()
{
    if (!image.ok()) return false;
    auto name = image.name();
    // $BUGBUG: Also use last modified time (system currently doesn't expose it)
    auto size = system::getFileSize(name);
    if (size == image_size) return true;
    return loadImage(name);
}

bool Imagine::loadImage(std::string path)
{
    auto run = setPlaying(false);
    auto last_size = image_size;
    // $BUGBUG: Also use last modified time (system currently doesn't expose it)
    image_size = system::getFileSize(path);
    if (image_size != 0
        && last_size == image_size
        && path == image.name()) {
        return true;
    }

    //DEBUG("Selected image (%s)", path.c_str());
    //DEBUG("Image size %d x %d", image.width(), image.height());
    pic_folder = system::getDirectory(path);
    if (image.open(path)) {
        traversal->configure_image(Vec(image.width(), image.height()));
        traversal->reset();
        setPlaying(run);
        return true;
    } else {
        //DEBUG("Image load failed: %s", image.reason().c_str());
        traversal->configure_image(Vec(image.width(), image.height()));
        traversal->reset();
        //image.close();
        image_size = 0;
        return false;
    }
}

bool Imagine::loadImageDialog()
{
    osdialog_filters* filters = osdialog_filters_parse("Images (.png .jpg .gif):png,jpg,jpeg,gif;Any (*):*");
    DEFER({osdialog_filters_free(filters);});

    std::string dir = pic_folder.empty() ? asset::user("") : pic_folder;
    std::string name = system::getFilename(image.name());
    char* pathC = osdialog_file(OSDIALOG_OPEN, dir.c_str(), name.c_str(), filters);
    if (!pathC) {
        pause();
        image.close();
        image_size = 0;
        return false;
    }
    std::string path = pathC;
    std::free(pathC);
    return loadImage(path);
}

void Imagine::onSampleRateChange() {
    control_rate.onSampleRateChanged();
    traversal->configure_rate(getSpeed(), APP->engine->getSampleRate());
    updateParams();
}

constexpr static const char * IMAGE_KEY = "image";
constexpr static const char * IMAGE_FOLDER_KEY = "image-folder";
constexpr static const char * BRIGHT_IMAGE_KEY = "bright-image";
constexpr static const char * POSX_KEY = "x-pos";
constexpr static const char * POSY_KEY = "y-pos";

json_t *Imagine::dataToJson()
{
    json_t *root = ThemeModule::dataToJson();

    auto name = image.name();
    if (!name.empty()) {
        json_object_set_new(root, IMAGE_KEY, json_stringn(name.c_str(), name.size()));
    }
    if ((reset_pos.x >= 0.f) && (reset_pos.y >= 0.f)) {
        json_object_set_new(root, POSX_KEY, json_real(reset_pos.x));
        json_object_set_new(root, POSY_KEY, json_real(reset_pos.y));
    }
    json_object_set_new(root, BRIGHT_IMAGE_KEY, json_boolean(bright_image));

    if (!pic_folder.empty()) {
        json_object_set_new(root, IMAGE_FOLDER_KEY, json_stringn(pic_folder.c_str(), pic_folder.size()));
    }
    return root;
}

void Imagine::dataFromJson(json_t *root)
{
    json_t *j = json_object_get(root, IMAGE_KEY);
    if (j) {
        std::string path = json_string_value(j);
        if (image.open(path)) {
            //DEBUG("Opened image (%s)", path.c_str());
            //DEBUG("Image size %d x %d", image.width(), image.height());
            if (traversal) {
                traversal->configure_image(Vec(image.width(), image.height()));
                traversal->reset();
            }
        } else {
            //DEBUG("Image load failed: %s", image.reason().c_str());
        }
    }
    j = json_object_get(root, POSX_KEY);
    if (j) {
        float x = json_number_value(j);
        j = json_object_get(root, POSY_KEY);
        float y = j ? json_number_value(j) : 0.f;
        reset_pos = Vec(x, y);
        if (traversal) {
            traversal->set_position(reset_pos);
        }
    }

    j = json_object_get(root, BRIGHT_IMAGE_KEY);
    if (j) {
        bright_image = json_is_true(j);
    }

    j = json_object_get(root, IMAGE_FOLDER_KEY);
    if (j) {
        pic_folder = json_string_value(j);
    }

    ThemeModule::dataFromJson(root);
}

void Imagine::updateParams()
{
    gt = getParam(GT_PARAM).getValue();

    float slew = getParam(SLEW_PARAM).getValue();
    float sample_rate = APP->engine->getSampleRate();
#ifdef XYSLEW
    x_slew.configure(sample_rate, slew, .01f);
    y_slew.configure(sample_rate, slew, .01f);
#endif
    voltage_slew.configure(sample_rate, slew, .01f);

    auto p = getParam(POLARITY_PARAM).getValue();
    polarity = p <= 0.5
        ? VRange::UNIPOLAR
        : VRange::BIPOLAR;

    Traversal id = getTraversalId();
    if (id != traversal_id || !traversal) {
        if (traversal) {
            delete traversal;
        }
        traversal = MakeTraversal(id);
        traversal->configure_image(Vec(image.width(), image.height()));
        traversal->reset();
        if (reset_pos.x >= 0 && reset_pos.y >= 0) {
            traversal->set_position(reset_pos);
        }
        resetpos_trigger.reset();
        traversal_id = id;
    }
    traversal->configure_rate(getSpeed(), sample_rate);

    color_component = getColorComponent();
}

void Imagine::processBypass(const ProcessArgs& args)
{
    outputs[VOLTAGE_OUT].setVoltage(0.0f);
    outputs[X_OUT].setVoltage(0.0f);
    outputs[Y_OUT].setVoltage(0.0f);
}

void Imagine::process(const ProcessArgs& args)
{
    assert(!isBypassed());
    
    if (control_rate.process()) {
        updateParams();
    }

    if (inputs[PLAY_INPUT].isConnected()) {
        auto v = inputs[PLAY_INPUT].getVoltage();
        if (play_trigger.process(v, 0.1f, 5.f)) {
            setPlaying(!isPlaying());
        }
    }

    if (inputs[RESET_POS_INPUT].isConnected())
    {
        auto v = inputs[RESET_POS_INPUT].getVoltage();
        if (resetpos_trigger.process(v, 0.1f, 5.f)) {
            resetpos_trigger.reset();
            traversal->reset();
            auto pos = getResetPos();
            if (pos.x >= 0.f && pos.y >= 0.f) {
                traversal->set_position(pos);
            }
        }
    }

    int width, height;
    if (image.ok()) {
        width = std::max(1, image.width());
        height = std::max(1, image.height());
    } else {
        width = PANEL_IMAGE_WIDTH;
        height = PANEL_IMAGE_HEIGHT;
    }

    if (running) {
        if (isXYInput()) {
            traversal->set_position(Vec(width * (inputs[X_INPUT].getVoltage() / 10.f), height * (inputs[Y_INPUT].getVoltage() / 10.f)));
        } else {
            traversal->process();
        }
    }
    Vec pos = traversal->get_position();

    if (outputs[X_OUT].isConnected()) {
        float v;
        if (isBipolar()) {
            v = (pos.x - width/2.f) / width * 10.f;
        } else {
            v = pos.x / width * 10.0f;
        }
#ifdef XYSLEW
        outputs[X_OUT].setVoltage(x_slew.next(v));
#else
        outputs[X_OUT].setVoltage(v);
#endif
    }

    if (outputs[Y_OUT].isConnected()) {
        float v;
        if (isBipolar()) {
            v =  -(pos.y - height/2.f) / height * 10.f;
        } else {
            v = pos.y / height * 10.0f;
        }

#ifdef XYSLEW
        outputs[Y_OUT].setVoltage(y_slew.next(v));
#else
        outputs[Y_OUT].setVoltage(v);
#endif
    }

    if (isPixelOutput()) {
        NVGcolor pix;
        if (image.ok()) {
            pix = image.pixel(pos.x, pos.y);
        } else if (isXYPad()) {
            // r = x, g = y, b = .5, a = 1.
            float hf = height;
            pix = nvgRGBAf(pos.x / width, hf - pos.y / hf, 0.5, 1.);
        } else {
            pix = COLOR_NONE;
        }

        // RGB outputs are unipolar 0-10v
        outputs[RED_OUT].setVoltage(pix.r * 10.f);
        outputs[GREEN_OUT].setVoltage(pix.g * 10.f);
        outputs[BLUE_OUT].setVoltage(pix.b * 10.f);

        auto v = ComponentValue(pix) * 10.0f;
        lookback.push(v);
        auto spread = lookback.spread();
        if (spread > gt) {
            trigger_pulse.trigger();
            if (lookback.isFilled()) {
                gate_high = !gate_high;
            }
        }
        //outputs[TEST_OUT].setVoltage(spread);

        if (isBipolar()) {
            v -= 5.f;
        }
        outputs[VOLTAGE_OUT].setVoltage(voltage_slew.next(v));

        outputs[GATE_OUT].setVoltage(gate_high * 10.f);
        outputs[TRIGGER_OUT].setVoltage(trigger_pulse.process(args.sampleTime) * 10.f);
    }
}

}

Model *modelImagine = createModel<Imagine, ImagineUi>("pachde-imagine");
