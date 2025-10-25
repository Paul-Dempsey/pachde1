#include "Imagine.hpp"
#include "imagine_layout.hpp"
#include "services/dsp.hpp"
#include "services/json-help.hpp"
#include "services/open-file.hpp"
#include "services/text.hpp"
#include "widgets/components.hpp"

fs::file_time_type LastWriteTime(std::string path)
{
    try {
        return fs::last_write_time(fs::u8path(path));
    } catch (fs::filesystem_error& e) {
        return fs::file_time_type();
    }
}

namespace pachde {

// Make a fake path if under plugin folder, so that presets are portable
std::string MakePluginPath(std::string path)
{
    path = system::getCanonical(path);
    auto plug = pluginInstance->path;
    if (0 == plug.compare(0, plug.size(), path.c_str(), 0, plug.size())) {
        return path.replace(0, plug.size(), "{plug}");
    }
    return path;
}

// Decode fake path as needed to make a real path
std::string MakeUnPluginPath(std::string path)
{
    std::string plug = "{plug}";
    if (0 == plug.compare(0, plug.size(), path, 0, plug.size())) {
        path = path.replace(0, plug.size(), pluginInstance->path);
    }
    return system::getCanonical(path);
}

Imagine::Imagine()
:   min_retrigger(0.f)
{
    config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, 0);

    configParam(SLEW_PARAM, 0.f, 1.f, 0.f,
        "Slew", "%", 0.f, 100.f);

    configParam(GT_PARAM, 0.f, 1.f, .3f,
        "Gate/Trigger spread threshold");

    configParam(MIN_TRIGGER_PARAM, 0.f, 1000.f, 0.f,
        "Minimum t/g time", "ms");

    configSwitch(POLARITY_PARAM, 0.f, 1.0f, 0.0f,
        "Polarity", { "Unipolar (0 - 10v)", "Bipolar (-5 - 5v)" });

    configSwitch(RUN_PARAM, 0.f, 1.f, 0.f,
        "Play", { "Paused", "Playing" });

    configInput(RESET_POS_INPUT, "Reset head position trigger");
    configInput(PLAY_INPUT, "Play/Pause trigger");
    configInput(X_INPUT, "Read head X position");
    configInput(Y_INPUT, "Read head Y position");
    configInput(SPEED_INPUT, "Read head speed");
    configInput(MIN_TRIGGER_INPUT, "Minimum g/t time");

    configParam(SPEED_PARAM, 0.f, 100.f, 10.f,
        "Speed");

    configSwitch(SPEED_MULT_PARAM, 1.f, 10.f, 1.f,
        "Multiplier",
        { "x1", "x2", "x3", "x4", "x5", "x6", "x7", "x8", "x9", "x10" });

    configSwitch(PATH_PARAM, 0.0f, static_cast<int>(Traversal::NUM_TRAVERSAL)-1, 0.0f, "Path", {
        "Scanline", // Laratab
        "Bounce",
        "Vinyl",
        "Wander",
        "X/Y Pad",
        "Tabalar",
        "Ralabat"
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
    setPlaying(false);
    updateParams();
}

bool Imagine::setPlaying(bool play)
{
    play_trigger.reset();
    bool previous = running;
    running = play;
    getParamQuantity(RUN_PARAM)->setValue(running);
    return previous;
}


void Imagine::closeImage() {
    setPlaying(false);
    image.close();
}

bool Imagine::reloadImage()
{
    return image.ok() ? loadImage(image.name()) : false;
}

bool Imagine::loadImage(std::string path)
{
    auto run = setPlaying(false);
    auto last_size = image_size;
    auto last_time = image_time;
    path = system::getCanonical(path);
    image_size = system::getFileSize(path);
    image_time = LastWriteTime(path);
    if (image_size != 0
        && last_size == image_size
        && last_time == image_time
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
        image_size = 0;
        image_time = fs::file_time_type();
        //DEBUG("Image load failed: %s", image.reason().c_str());
        traversal->configure_image(Vec(image.width(), image.height()));
        traversal->reset();
        //image.close();
        return false;
    }
}

bool Imagine::loadImageDialog()
{
    std::string path;
    bool ok = openFileDialog(pic_folder, "Images (.png .jpg .gif):png,jpg,jpeg,gif;Any (*):*", image.name(), path);
    if (ok) {
        return loadImage(path);
    } else {
        pause();
        image.close();
        image_size = 0;
        return false;
    }
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
constexpr static const char * MEDALLION_KEY = "medallion-fill";
constexpr static const char * LABELS_KEY = "labels";

json_t *Imagine::dataToJson()
{
    json_t *root = ThemeModule::dataToJson();

    auto name = image.name();
    if (!name.empty()) {
        auto persist_name = MakePluginPath(name);
        set_json(root, IMAGE_KEY, persist_name);
    }
    if ((reset_pos.x >= 0.f) && (reset_pos.y >= 0.f)) {
        set_json(root, POSX_KEY, reset_pos.x);
        set_json(root, POSY_KEY, reset_pos.y);
    }
    set_json(root, BRIGHT_IMAGE_KEY, bright_image);
    set_json(root, MEDALLION_KEY, medallion_fill);
    set_json(root, LABELS_KEY, labels);

    if (!pic_folder.empty()) {
        set_json(root, IMAGE_FOLDER_KEY, pic_folder);
    }
    return root;
}

void Imagine::dataFromJson(json_t *root)
{
    ThemeModule::dataFromJson(root);

    pic_folder = get_json_string(root, IMAGE_FOLDER_KEY, pic_folder);

    json_t* j = json_object_get(root, IMAGE_KEY);
    if (j) {
        std::string path = MakeUnPluginPath(json_string_value(j));
        loadImage(path);
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

    medallion_fill = get_json_bool(root, MEDALLION_KEY, medallion_fill);
    bright_image = get_json_bool(root, BRIGHT_IMAGE_KEY, bright_image);
    labels = get_json_bool(root, LABELS_KEY, labels);

    dirty_settings = true;
}

void Imagine::updateParams()
{
    // Close gate when not running
    if (!running) {
        gate_high = 0;
        trigger_pulse.reset();
    }
    gt = getParam(GT_PARAM).getValue();

    if (getInput(MIN_TRIGGER_INPUT).isConnected()) {
        float v = getInput(MIN_TRIGGER_INPUT).getVoltage();
        auto pq = getParamQuantity(MIN_TRIGGER_PARAM);
        v = v * (pq->getMaxValue() / 10.f);
        v = getParam(MIN_TRIGGER_PARAM).getValue() + v * .5f;
        min_retrigger.configure(v);
    } else {
        min_retrigger.configure(getParam(MIN_TRIGGER_PARAM).getValue());
    }

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

    if (getInput(SPEED_INPUT).isConnected() && !isXYInput()) {
        getParam(SPEED_PARAM).setValue(clamp(getInput(SPEED_INPUT).getVoltage() * 100.f, 0.f, 100.f));
    }
    traversal->configure_rate(getSpeed(), sample_rate);
    color_component = getColorComponent();
}

void Imagine::processBypass(const ProcessArgs& args)
{
    getOutput(VOLTAGE_OUT).setVoltage(0.0f);
    getOutput(X_OUT).setVoltage(0.0f);
    getOutput(Y_OUT).setVoltage(0.0f);
    getOutput(GATE_OUT).setVoltage(0.0f);
    getOutput(TRIGGER_OUT).setVoltage(0.0f);
    getOutput(RED_OUT).setVoltage(0.0f);
    getOutput(GREEN_OUT).setVoltage(0.0f);
    getOutput(BLUE_OUT).setVoltage(0.0f);
}

void Imagine::process(const ProcessArgs& args)
{
    assert(!isBypassed());

    if (control_rate.process()) {
        updateParams();
    }

    if (getInput(PLAY_INPUT).isConnected()) {
        auto v = getInput(PLAY_INPUT).getVoltage();
        if (play_trigger.process(v, 0.1f, 5.f)) {
            setPlaying(!isPlaying());
        }
    }

    if (getInput(RESET_POS_INPUT).isConnected())
    {
        auto v = getInput(RESET_POS_INPUT).getVoltage();
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
            traversal->set_position(Vec(width * (getInput(X_INPUT).getVoltage() / 10.f), height * (getInput(Y_INPUT).getVoltage() / 10.f)));
        } else {
            traversal->process();
        }
    }
    Vec pos = traversal->get_position();

    if (getOutput(X_OUT).isConnected()) {
        float v;
        if (isBipolar()) {
            v = (pos.x - width/2.f) / width * 10.f;
        } else {
            v = pos.x / width * 10.0f;
        }
#ifdef XYSLEW
        getOutput(X_OUT).setVoltage(x_slew.next(v));
#else
        getOutput(X_OUT).setVoltage(v);
#endif
    }

    if (getOutput(Y_OUT).isConnected()) {
        float v;
        if (isBipolar()) {
            v =  -(pos.y - height/2.f) / height * 10.f;
        } else {
            v = pos.y / height * 10.0f;
        }

#ifdef XYSLEW
        getOutput(Y_OUT).setVoltage(y_slew.next(v));
#else
        getOutput(Y_OUT).setVoltage(v);
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
        getOutput(RED_OUT).setVoltage(pix.r * 10.f);
        getOutput(GREEN_OUT).setVoltage(pix.g * 10.f);
        getOutput(BLUE_OUT).setVoltage(pix.b * 10.f);

        auto v = ComponentValue(pix) * 10.0f;
        lookback.push(v);
        auto spread = lookback.spread();
        if (spread > gt) {
            if (min_retrigger.process()) {
                trigger_pulse.trigger();
                if (lookback.isFilled()) {
                    gate_high = !gate_high;
                }
            }
        }
        //getOutput(TEST_OUT).setVoltage(spread);

        getOutput(VOLTAGE_OUT).setVoltage(voltage_slew.next(isBipolar() ? v - .5f : v));

        getOutput(GATE_OUT).setVoltage(gate_high * 10.f);
        getOutput(TRIGGER_OUT).setVoltage(trigger_pulse.process(args.sampleTime) * 10.f);
    }
}

}

Model *modelImagine = createModel<Imagine, ImagineUi>("pachde-imagine");
