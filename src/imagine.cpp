#include <osdialog.h>
#include "themehelpers.hpp"
#include "components.hpp"
#include "text.hpp"
#include "dsp.hpp"
#include "Imagine.hpp"

Imagine::Imagine() {
    pix_rate.configure(10.0f);
    scan_x = 0;
    scan_y = 0;
    config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, 0);
    configParam(SLEW_PARAM, 0.0f, 1.0f, 0.5f, "Slew", "%", 0.0f, 100.0f);
    configSwitch(VOCT_RANGE_PARAM, 0.0f, 1.0f, 0.0f, "V/Oct Polarity", { "Bipolar -5v — +5v", "Unipolar 0v — +10v" });
    configSwitch(RUN_PARAM, 0.0f, 1.0f, 0.0f, "Play", { "Paused", "Playing" });
    //configParam(SPEED_PARAM, 0.0f, 100.0f, 15.0f, "Speed", "%");
    //configInput(SPEED_INPUT, "Speed");
    configSwitch(PATH_PARAM, 0.0f, static_cast<int>(Traversal::LAST_TRAVERSAL), 0.0f, "Path", 
    {
        "Scanline",
        "TBLR",
        "Diagonal",
    });
    configOutput(X_OUT, "x");
    configOutput(Y_OUT, "y");
    configOutput(VOCT_OUT, "V/Oct");

    updateParams();
}

bool Imagine::loadImageDialog()
{
    scan_x = 0;
    scan_y = 0;
    osdialog_filters* filters = osdialog_filters_parse("Images (.png .jpg .gif):png,jpg,jpeg,gif;Any (*):*");
    DEFER({osdialog_filters_free(filters);});

    std::string dir = pic_folder.empty() ? asset::user("") : pic_folder;
    char* pathC = osdialog_file(OSDIALOG_OPEN, dir.c_str(), image.name().c_str(), filters);
    if (!pathC) {
        pause();
        image.close();
        return false;
    }
    std::string path = pathC;
    std::free(pathC);
    DEBUG("Selected image (%s)", path.c_str());
    pic_folder = system::getDirectory(path);
    auto run = setPlaying(false);
    if (image.open(path)) {
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
    pix_rate.onSampleRateChanged();
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
    float sampleRate = APP->engine->getSampleRate();
    x_slew.configure(sampleRate, slew, .01f);
    y_slew.configure(sampleRate, slew, .01f);
    voct_slew.configure(sampleRate, slew, .01f);

    voct_range = (getParam(VOCT_RANGE_PARAM).getValue() < 0.5)
        ? VRange::BIPOLAR
        : VRange::UNIPOLAR;

    Traversal path = static_cast<Traversal>(std::floor(getParam(PATH_PARAM).getValue()));
    if (path != traversal_id || !traversal) {
        traversal = MakeTraversal(path);
        traversal_id = path;
    }

    // auto pmax = getParamQuantity(SPEED_PARAM)->getMaxValue();
    // if (getInput(SPEED_INPUT).isConnected()) {
    //     pix_rate.configure(std::max(0.0f, pmax - getInput(SPEED_INPUT).getVoltage() * pmax/10.0f));
    // } else {
    //     pix_rate.configure(std::max(0.0f, pmax - getParam(SPEED_PARAM).getValue()));
    // }
}

void Imagine::process(const ProcessArgs& args)
{
    if (control_rate.process()) {
        updateParams();
    }

    int width, height;
    if (image.ok()) {
        width = image.width();
        height = image.height();
    } else {
        width = PANEL_IMAGE_WIDTH;
        height = PANEL_IMAGE_HEIGHT;
    }

    auto x = scan_x;
    auto y = scan_y;
    if (outputs[X_OUT].isConnected()) {
        auto v = (float)x / width * 10.0f;
        outputs[X_OUT].setVoltage(x_slew.next(v));
    }

    if (outputs[Y_OUT].isConnected()) {
        auto v = (float)y / height * 10.0f;
        outputs[Y_OUT].setVoltage(y_slew.next(v));
    }

    if (outputs[VOCT_OUT].isConnected()) {
        auto pix = image.pixel(x, y);
        auto lum = LuminanceLinear(pix) * 10.0f;
        if (voct_range == VRange::BIPOLAR) {
            lum -= 5.0f;
        } 
        outputs[VOCT_OUT].setVoltage(voct_slew.next(lum));
    }

    // TODO: triggers and gates
    // TODO: progress at scaled param rate, rather than sample rate and interpolate
    if (running && pix_rate.process()) {
        auto next = traversal->next(image, scan_x, scan_y, 1);
        scan_x = next.x;
        scan_y = next.y;
    }
}

Model *modelImagine = createModel<Imagine, ImagineUi>("pachde-imagine");
