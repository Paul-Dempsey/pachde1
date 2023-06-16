#include "Imagine.hpp"
#include <osdialog.h>
#include "imagine_layout.hpp"
#include "../components.hpp"
#include "../text.hpp"
#include "../dsp.hpp"

namespace pachde {
    
Imagine::Imagine() {
    config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, 0);

    configParam(SLEW_PARAM, 0.f, 1.f, 0.f,
        "Slew", "%", 0.f, 100.f);

    configSwitch(POLARITY_PARAM, 0.f, 1.0f, 0.0f,
        "Polarity", { "Bipolar (-5 - 5v)", "Unipolar (0 - 10v)" });

    configSwitch(RUN_PARAM, 0.f, 1.f, 0.f,
        "Play", { "Paused", "Playing" });

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
    });

    configSwitch(COMP_PARAM, 0.0f, static_cast<int>(ColorComponent::NUM_COMPONENTS)-1, 0.0f, "Component", {
        "Luminance",
        "Saturation",
        "Hue",
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
    updateParams();
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
        return false;
    }
    auto run = setPlaying(false);
    std::string path = pathC;
    std::free(pathC);

    if (path == image.name()) return true;

    DEBUG("Selected image (%s)", path.c_str());
    DEBUG("Image size %d x %d", image.width(), image.height());
    pic_folder = system::getDirectory(path);
    if (image.open(path)) {
        traversal->configure_image(Vec(image.width(), image.height()));
        traversal->reset();
        setPlaying(run);
        return true;
    } else {
        DEBUG("Image load failed: %s", image.reason().c_str());
        traversal->configure_image(Vec(image.width(), image.height()));
        traversal->reset();
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
            DEBUG("Opened image (%s)", path.c_str());
            DEBUG("Image size %d x %d", image.width(), image.height());
            if (traversal) {
                traversal->configure_image(Vec(image.width(), image.height()));
                traversal->reset();
            }
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
#ifdef XYSLEW
    x_slew.configure(sample_rate, slew, .01f);
    y_slew.configure(sample_rate, slew, .01f);
#endif
    voltage_slew.configure(sample_rate, slew, .01f);
    polarity = (getParam(POLARITY_PARAM).getValue() < 0.5)
        ? VRange::BIPOLAR
        : VRange::UNIPOLAR;

    Traversal id = getTraversalId();
    if (id != traversal_id || !traversal) {
        if (traversal) {
            delete traversal;
        }
        traversal = MakeTraversal(id);
        traversal->configure_image(Vec(image.width(), image.height()));
        traversal->reset();
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
    if (running) {
        traversal->process();
    }
    Vec pos = traversal->get_position();

    int width, height;
    if (image.ok()) {
        width = std::max(1, image.width());
        height = std::max(1, image.height());
    } else {
        width = PANEL_IMAGE_WIDTH;
        height = PANEL_IMAGE_HEIGHT;
    }

    if (outputs[X_OUT].isConnected()) {
        auto v = pos.x / width * 10.0f;
#ifdef XYSLEW
        outputs[X_OUT].setVoltage(x_slew.next(v));
#else
        outputs[X_OUT].setVoltage(v);
#endif
    }

    if (outputs[Y_OUT].isConnected()) {
        auto v = pos.y / height * 10.0f;
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
        if (outputs[RED_OUT].isConnected()) {
            outputs[RED_OUT].setVoltage(pix.r * 10.f);
        }
        if (outputs[GREEN_OUT].isConnected()) {
            outputs[GREEN_OUT].setVoltage(pix.g * 10.f);
        }
        if (outputs[BLUE_OUT].isConnected()) {
            outputs[BLUE_OUT].setVoltage(pix.b * 10.f);
        }

        if (outputs[VOLTAGE_OUT].isConnected()) {
            auto v = ComponentValue(pix) * 10.0f;
            if (polarity == VRange::BIPOLAR) {
                v -= 0.5;
            }
            outputs[VOLTAGE_OUT].setVoltage(voltage_slew.next(v));
        }
        if (outputs[GATE_OUT].isConnected()) {
            //outputs[GATE_OUT].setVoltage(v);
        }
        if (outputs[TRIGGER_OUT].isConnected()) {
            //outputs[TRIGGER_OUT].setVoltage(v);
        }
    }
}

}

Model *modelImagine = createModel<Imagine, ImagineUi>("pachde-imagine");
