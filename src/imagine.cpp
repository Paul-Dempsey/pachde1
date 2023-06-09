#include <osdialog.h>
#include "themehelpers.hpp"
#include "components.hpp"
#include "text.hpp"
#include "dsp.hpp"
#include "Imagine.hpp"

Imagine::Imagine() {
    config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, 0);
    configParam(SLEW_PARAM, 0.0f, 1.0f, 0.5f, "Slew", "%", 0.0f, 100.0f);
    configSwitch(VOLTAGE_RANGE_PARAM, 0.0f, 1.0f, 0.0f, "Voltage Polarity", { "Bipolar -5v — +5v", "Unipolar 0v — +10v" });
    configSwitch(RUN_PARAM, 0.0f, 1.0f, 0.0f, "Play", { "Paused", "Playing" });
    configParam(SPEED_PARAM, 0.0f, 100.0f, 10.0f, "Speed");
    configSwitch(SPEED_MULT_PARAM, 1.0f, 10.0f, 1.0f, "Speed Multiplier", {
        "1x", "2x", "3x", "4x", "5x", "6x", "7x", "8x", "9x", "10x"
    });
    //configInput(SPEED_INPUT, "Speed");
    configSwitch(PATH_PARAM, 0.0f, static_cast<int>(Traversal::NUM_TRAVERSAL)-1, 0.0f, "Path", {
        "Scanline",
        "Bounce",
        "Vinyl",
        "Wander",
    });
    configSwitch(COMP_PARAM, 0.0f, static_cast<int>(ColorComponent::NUM_COMPONENTS)-1, 0.0f, "Component", {
        "Luminance",
        "Saturation",
        "Hue",
        "R",
        "G",
        "B",
        "Alpha"
    });
    configOutput(X_OUT, "x");
    configOutput(Y_OUT, "y");
    configOutput(VOLTAGE_OUT, "Voltage");

    updateParams();
}

bool Imagine::loadImageDialog()
{
    osdialog_filters* filters = osdialog_filters_parse("Images (.png .jpg .gif):png,jpg,jpeg,gif;Any (*):*");
    DEFER({osdialog_filters_free(filters);});

    std::string dir = pic_folder.empty() ? asset::user("") : pic_folder;
    char* pathC = osdialog_file(OSDIALOG_OPEN, dir.c_str(), image.name().c_str(), filters);
    if (!pathC) {
        pause();
        image.close();
        return false;
    }
    auto run = setPlaying(false);
    std::string path = pathC;
    std::free(pathC);
    DEBUG("Selected image (%s)", path.c_str());
    pic_folder = system::getDirectory(path);
    if (image.open(path)) {
        traversal->configure_image(Vec(image.width(), image.height()));
        traversal->reset();
        setPlaying(run);
        return true;
    } else {
        DEBUG("Image load failed: %s", image.reason().c_str());
        //image.close();
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

json_t *Imagine::dataToJson()
{
    json_t *root = ThemeModule::dataToJson();

    auto name = image.name();
    if (!name.empty()) {
        json_object_set_new(root, IMAGE_KEY, json_stringn(name.c_str(), name.size()));
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
            traversal->configure_image(Vec(image.width(), image.height()));
        } else {
            DEBUG("Image load failed: %s", image.reason().c_str());
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
    running = getParam(RUN_PARAM).getValue() > 0.5f;

    float slew = getParam(SLEW_PARAM).getValue();
    float sample_rate = APP->engine->getSampleRate();
    x_slew.configure(sample_rate, slew, .01f);
    y_slew.configure(sample_rate, slew, .01f);
    voct_slew.configure(sample_rate, slew, .01f);

    voct_range = (getParam(VOLTAGE_RANGE_PARAM).getValue() < 0.5)
        ? VRange::BIPOLAR
        : VRange::UNIPOLAR;

    Traversal id = getTraversalId();
    if (id != traversal_id || !traversal) {
        if (traversal) {
            delete traversal;
        }
        traversal = MakeTraversal(id);
        traversal_id = id;
        traversal->configure_image(Vec(image.width(), image.height()));
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
    if (running) {
        traversal->process();
    }
    Vec pos = traversal->get_position();

    int width, height;
    if (image.ok()) {
        width = image.width();
        height = image.height();
    } else {
        width = PANEL_IMAGE_WIDTH;
        height = PANEL_IMAGE_HEIGHT;
    }

    if (outputs[X_OUT].isConnected()) {
        auto v = pos.x / width * 10.0f;
        outputs[X_OUT].setVoltage(x_slew.next(v));
    }

    if (outputs[Y_OUT].isConnected()) {
        auto v = pos.y / height * 10.0f;
        outputs[Y_OUT].setVoltage(y_slew.next(v));
    }

    if (outputs[VOLTAGE_OUT].isConnected()) {
        if (image.ok()) {
            auto pix = image.pixel(pos.x, pos.y);
            auto v = ComponentValue(pix) * 10.0f;
            if (voct_range == VRange::BIPOLAR) {
                v -= 5.0f;
            }
            outputs[VOLTAGE_OUT].setVoltage(voct_slew.next(v));
        } else {
            outputs[VOLTAGE_OUT].setVoltage(0.0f);
        }
    }
}

Model *modelImagine = createModel<Imagine, ImagineUi>("pachde-imagine");
