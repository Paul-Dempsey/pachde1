#include "Fancy.hpp"
#include "services/json-help.hpp"
#include "services/rack-help.hpp"

namespace pachde {

constexpr const int PARAM_INTERVAL = 50;

Fancy::Fancy() {
    std::vector<std::string> off_on{"off", "on"};
    config(Params::N_PARAMS, Inputs::N_INPUTS, Outputs::N_OUTPUTS, Lights::N_LIGHTS);

    configSwitch(P_FANCY_IMAGE_ON, 0.f, 1.f, 0.f, "Enable FancyBox Image", off_on);
    configSwitch(P_FANCY_IMAGE_FIT, 0.f, 2.f, 0.f, "FancyBox Image fitting", {
        "Cover", "Fit", "Stretch"
    });
    configSwitch(P_FANCY_IMAGE_GRAY, 0.f, 1.f, 0.f, "FancyBox Image grayscale", off_on);

    configSwitch(P_FANCY_FILL_ON,  0.f, 1.f, 1.f, "Enable FancyBox Fill", off_on);
    configParam(P_FANCY_FILL_FADE, 0.f, 100.f, 100.f, "FancyBox Fill fade", "%");

    configSwitch(P_FANCY_LINEAR_ON, 0.f, 1.f, 0.f, "Enable FancyBox Linear gradient", off_on);
    configParam(P_FANCY_LINEAR_START_FADE, 0.f, 100.f, 100.f, "FancyBox Linear start fade", "%");
    configParam(P_FANCY_LINEAR_X1,         0.f, 100.f, 0.f, "FancyBox Linear x1", "%");
    configParam(P_FANCY_LINEAR_Y1,         0.f, 100.f, 0.f, "FancyBox Linear y1", "%");
    configParam(P_FANCY_LINEAR_END_FADE,   0.f, 100.f, 100.f, "FancyBox Linear end fade", "%");
    configParam(P_FANCY_LINEAR_X2,         0.f, 100.f, 100.f, "FancyBox Linear x2", "%");
    configParam(P_FANCY_LINEAR_Y2,         0.f, 100.f, 100.f, "FancyBox Linear y2", "%");

    configSwitch(P_FANCY_RADIAL_ON, 0.f, 1.f, 0.f, "Enable FancyBox Radial gradient", off_on);
    configParam(P_FANCY_RADIAL_INNER_FADE, 0.f, 100.f, 8.f,  "FancyBox Radial inner fade", "%");
    configParam(P_FANCY_RADIAL_CX,         0.f, 100.f, 50.f, "FancyBox Radial cx", "%");
    configParam(P_FANCY_RADIAL_CY,         0.f, 100.f, 50.f, "FancyBox Radial cy", "%");
    configParam(P_FANCY_RADIAL_OUTER_FADE, 0.f, 100.f, 100.f,  "FancyBox Radial outer fade", "%");
    configParam(P_FANCY_RADIAL_RADIUS,     0.f, 100.f, 50.f, "FancyBox Radial radius", "%");

    configSwitch(P_FANCY_BOX_ON, 0.f, 1.f, 0.f, "Enable FancyBox Box gradient (vignette)", off_on);
    configParam(P_FANCY_BOX_INNER_FADE,    0.f, 100.f, 0.f,  "FancyBox Box inner fade", "%");
    configParam(P_FANCY_BOX_SHRINK_X,      0.f, 100.f, 0.f,  "FancyBox Box shrink x", "%");
    configParam(P_FANCY_BOX_SHRINK_Y,      0.f, 100.f, 0.f,  "FancyBox Box shrink y", "%");
    configParam(P_FANCY_BOX_OUTER_FADE,    0.f, 100.f, 100.f,  "FancyBox Box outer fade", "%");
    configParam(P_FANCY_BOX_RADIUS,        0.f, 100.f, 16.f, "FancyBox Box radius", "%");
    configParam(P_FANCY_BOX_FEATHER,       0.f, 100.f, 12.f, "FancyBox Box feather", "%");

    configInput(IN_FANCY_FILL_H,            "fill Hue");
    configInput(IN_FANCY_FILL_S,            "fill Saturation");
    configInput(IN_FANCY_FILL_L,            "fill Lightness");
    configInput(IN_FANCY_FILL_A,            "fill Alpha");
    configInput(IN_FANCY_FILL_FADE,         "fill fade");
    configInput(IN_FANCY_LINEAR_START_H,    "Linear start Hue");
    configInput(IN_FANCY_LINEAR_START_S,    "Linear start Saturation");
    configInput(IN_FANCY_LINEAR_START_L,    "Linear start Lightness");
    configInput(IN_FANCY_LINEAR_START_A,    "Linear start Alpha");
    configInput(IN_FANCY_LINEAR_START_FADE, "Linear start fade");
    configInput(IN_FANCY_LINEAR_X1,         "Linear x1");
    configInput(IN_FANCY_LINEAR_Y1,         "Linear y1");
    configInput(IN_FANCY_LINEAR_END_H,      "Linear end Hue");
    configInput(IN_FANCY_LINEAR_END_S,      "Linear end Saturation");
    configInput(IN_FANCY_LINEAR_END_L,      "Linear end Lightness");
    configInput(IN_FANCY_LINEAR_END_A,      "Linear end Alpha");
    configInput(IN_FANCY_LINEAR_END_FADE,   "Linear end fade");
    configInput(IN_FANCY_LINEAR_X2,         "Linear x2");
    configInput(IN_FANCY_LINEAR_Y2,         "Linear y2");
    configInput(IN_FANCY_RADIAL_INNER_H,    "Radial inner Hue");
    configInput(IN_FANCY_RADIAL_INNER_S,    "Radial inner Saturation");
    configInput(IN_FANCY_RADIAL_INNER_L,    "Radial inner Lightness");
    configInput(IN_FANCY_RADIAL_INNER_A,    "Radial inner Alpha");
    configInput(IN_FANCY_RADIAL_INNER_FADE, "Radial inner fade");
    configInput(IN_FANCY_RADIAL_CX,         "Radial cx");
    configInput(IN_FANCY_RADIAL_CY,         "Radial cy");
    configInput(IN_FANCY_RADIAL_OUTER_H,    "Radial outer Hue");
    configInput(IN_FANCY_RADIAL_OUTER_S,    "Radial outer Saturation");
    configInput(IN_FANCY_RADIAL_OUTER_L,    "Radial outer Lightness");
    configInput(IN_FANCY_RADIAL_OUTER_A,    "Radial outer Alpha");
    configInput(IN_FANCY_RADIAL_OUTER_FADE, "Radial outer fade");
    configInput(IN_FANCY_RADIAL_RADIUS,     "Radial radius");
    configInput(IN_FANCY_BOX_INNER_H,       "Box inner Hue");
    configInput(IN_FANCY_BOX_INNER_S,       "Box inner Saturation");
    configInput(IN_FANCY_BOX_INNER_L,       "Box inner Lightness");
    configInput(IN_FANCY_BOX_INNER_A,       "Box inner Alpha");
    configInput(IN_FANCY_BOX_INNER_FADE,    "Box inner_fade");
    configInput(IN_FANCY_BOX_SHRINK_X,      "Box shrink x");
    configInput(IN_FANCY_BOX_SHRINK_Y,      "Box shrink y");
    configInput(IN_FANCY_BOX_OUTER_H,       "Box outer Hue");
    configInput(IN_FANCY_BOX_OUTER_S,       "Box outer Saturation");
    configInput(IN_FANCY_BOX_OUTER_L,       "Box outer Lightness");
    configInput(IN_FANCY_BOX_OUTER_A,       "Box outer Alpha");
    configInput(IN_FANCY_BOX_OUTER_FADE,    "Box outer fade");
    configInput(IN_FANCY_BOX_RADIUS,        "Box radius");
    configInput(IN_FANCY_BOX_FEATHER,       "Box feather");
}

json_t* Fancy::dataToJson() {
    json_t *root = Base::dataToJson();
    set_json(root, "fancy", fancy);
    set_json(root, "shouting", shouting);
    set_json(root, "show-ports", show_ports);
    set_json(root, "pic-folder", pic_folder);
    //
    // Serialize fancy_data not expressed as a Param
    //
    set_json(root, "pic-file", fancy_data.image.path);
    char hex[10];
    packed_color::hexFormat(fancy_data.fill.color, 10, hex);  set_json(root, "fill-color", hex);
    packed_color::hexFormat(fancy_data.linear.icol, 10, hex); set_json(root, "linear-icol", hex);
    packed_color::hexFormat(fancy_data.linear.ocol, 10, hex); set_json(root, "linear-ocol", hex);
    packed_color::hexFormat(fancy_data.radial.icol, 10, hex); set_json(root, "radial-icol", hex);
    packed_color::hexFormat(fancy_data.radial.ocol, 10, hex); set_json(root, "radial-ocol", hex);
    packed_color::hexFormat(fancy_data.boxg.icol, 10, hex);   set_json(root, "box-icol", hex);
    packed_color::hexFormat(fancy_data.boxg.ocol, 10, hex);   set_json(root, "box-ocol", hex);
    return root;
}

PackedColor color_from_json(json_t* root, const char * key) {
    PackedColor color = colors::NoColor;
    const char * spec = get_json_cstring(root, key);
    if (spec && *spec) {
        color = parseColor(spec, color);
    }
    return color;
}

void Fancy::dataFromJson(json_t* root) {
    Base::dataFromJson(root);
    shouting   = get_json_bool(root, "shouting", shouting);
    fancy      = get_json_bool(root, "fancy", fancy);
    show_ports = get_json_bool(root, "show-ports", show_ports);
    pic_folder = get_json_string(root, "pic-folder");
    //
    // Serialize fancy_data not expressed as a Param
    //
    fancy_data.image.path = get_json_string(root, "pic-file");
    fancy_data.fill.color = color_from_json(root, "fill-color");
    fancy_data.linear.icol = color_from_json(root, "linear-icol");
    fancy_data.linear.ocol = color_from_json(root, "linear-ocol");
    fancy_data.radial.icol = color_from_json(root, "radial-icol");
    fancy_data.radial.ocol = color_from_json(root, "radial-ocol");
    fancy_data.boxg.icol = color_from_json(root, "box-icol");
    fancy_data.boxg.ocol = color_from_json(root, "box-ocol");
}

inline bool param_bool(rack::engine::Param& param) { return param.getValue() >= .5f; }
inline float fancy_param(rack::engine::Param& param) { return .01f * param.getValue(); }

void Fancy::onPortChange(const PortChangeEvent &e)
{
    if (e.connecting) {
        connection_count++;
    } else {
        connection_count--;
        if ((0 == connection_count) && ui) {
            ui->restore_unmodulated_parameters();
        }
    }
    assert(in_range(connection_count, 0, (int)Inputs::N_INPUTS));
}

PackedColor Fancy::modulate_color(PackedColor base, int input_id_first) {
    bool modulated{false};
    for (int id = input_id_first; id < input_id_first + 4; ++id) {
        if (getInput(id).isConnected()) {
            modulated = true;
            break;
        }
    }
    if (!modulated) return base;

    auto co = fromPacked(base);
    float hue   = clamp(Hue1(co)        + scaled_voltage(input_id_first));
    float sat   = clamp(Saturation1(co) + scaled_voltage(input_id_first));
    float lit   = clamp(Lightness(co)   + scaled_voltage(input_id_first));
    float alpha = clamp(co.a            + scaled_voltage(input_id_first));
    return packHsla(hue, sat, lit, alpha);
}

void Fancy::process_image(const ProcessArgs &args)
{
    image_process_count = (image_process_count + 1) % 25;
    if (image_process_count % 25) return;
    fancy_data.image.enabled = param_bool(getParam(P_FANCY_IMAGE_ON));
    fancy_data.image.fit = static_cast<ImageFit>(getParam(P_FANCY_IMAGE_FIT).getValue());
    fancy_data.image.gray = param_bool(getParam(P_FANCY_IMAGE_GRAY));
    if (ui->my_cloak) {
        ui->my_cloak->data.image.enabled = fancy_data.image.path.empty() ? false : fancy_data.image.enabled;
        ui->my_cloak->data.image.fit = fancy_data.image.fit;
        ui->my_cloak->data.image.gray = fancy_data.image.gray;
        if (ui->my_cloak->data.image.enabled) {
            if (ui->my_cloak->data.image.path != fancy_data.image.path) {
                if (ui->my_cloak->pic) {
                    ui->my_cloak->pic->close();
                }
                ui->my_cloak->data.image.path = fancy_data.image.path;
            }
        }
    }
}

void Fancy::process_fill(const ProcessArgs &args)
{
    fill_process_count = (fill_process_count + 1) % 25;
    if (fill_process_count % 25) return;

    fancy_data.fill.enabled = param_bool(getParam(P_FANCY_FILL_ON));
    fancy_data.fill.fade    = fancy_param(getParam(P_FANCY_FILL_FADE));
    if (ui->my_cloak) {
        ui->my_cloak->data.fill.enabled = fancy_data.fill.enabled;
        if (fancy_data.fill.enabled) {
            ui->my_cloak->data.fill.color = modulate_color(fancy_data.fill.color, IN_FANCY_FILL_H);
            ui->my_cloak->data.fill.fade = clamp(fancy_data.fill.fade + scaled_voltage(IN_FANCY_FILL_FADE));
        }
    }
}

void Fancy::process_linear(const ProcessArgs& args) {
    linear_process_count = (linear_process_count + 1) % 25;
    if (linear_process_count % 25) return;

    fancy_data.linear.enabled = param_bool(getParam(P_FANCY_LINEAR_ON));
    fancy_data.linear.ifade   = fancy_param(getParam(P_FANCY_LINEAR_START_FADE));
    fancy_data.linear.x1      = fancy_param(getParam(P_FANCY_LINEAR_X1));
    fancy_data.linear.y1      = fancy_param(getParam(P_FANCY_LINEAR_Y1));
    fancy_data.linear.ofade   = fancy_param(getParam(P_FANCY_LINEAR_END_FADE));
    fancy_data.linear.x2      = fancy_param(getParam(P_FANCY_LINEAR_X2));
    fancy_data.linear.y2      = fancy_param(getParam(P_FANCY_LINEAR_Y2));

    if (ui->my_cloak) {
        ui->my_cloak->data.linear.enabled = fancy_data.linear.enabled;
        if (fancy_data.linear.enabled) {
            LinearGradientData linear{fancy_data.linear};
            linear.ifade = clamp(linear.ifade + scaled_voltage(IN_FANCY_LINEAR_START_FADE));
            linear.x1    = clamp(linear.x1    + scaled_voltage(IN_FANCY_LINEAR_X1));
            linear.y1    = clamp(linear.y1    + scaled_voltage(IN_FANCY_LINEAR_Y1));
            linear.ofade = clamp(linear.ofade + scaled_voltage(IN_FANCY_LINEAR_END_FADE));
            linear.x2    = clamp(linear.x2    + scaled_voltage(IN_FANCY_LINEAR_X2));
            linear.y2    = clamp(linear.y2    + scaled_voltage(IN_FANCY_LINEAR_Y2));
            if (linear.ifade > 0.f) {
                linear.icol = modulate_color(fancy_data.linear.icol, IN_FANCY_LINEAR_START_H);
            }
            if (linear.ofade > 0.f) {
                linear.ocol = modulate_color(fancy_data.linear.ocol, IN_FANCY_LINEAR_END_H);
            }
            ui->my_cloak->data.linear.init(linear);
        }
    }
}

void Fancy::process_radial(const ProcessArgs& args) {
    radial_process_count = (radial_process_count + 1) % 25;
    if (radial_process_count % 25) return;

    fancy_data.radial.enabled = param_bool(getParam(P_FANCY_RADIAL_ON));
    fancy_data.radial.ifade   = fancy_param(getParam(P_FANCY_RADIAL_INNER_FADE));
    fancy_data.radial.cx      = fancy_param(getParam(P_FANCY_RADIAL_CX));
    fancy_data.radial.cy      = fancy_param(getParam(P_FANCY_RADIAL_CY));
    fancy_data.radial.ofade   = fancy_param(getParam(P_FANCY_RADIAL_OUTER_FADE));
    fancy_data.radial.radius  = fancy_param(getParam(P_FANCY_RADIAL_RADIUS));

    if (ui->my_cloak) {
        ui->my_cloak->data.radial.enabled = fancy_data.radial.enabled;
        if (fancy_data.radial.enabled) {
            RadialGradientData radial{fancy_data.radial};
            radial.ifade  = clamp(radial.ifade  + scaled_voltage(IN_FANCY_RADIAL_INNER_FADE));
            radial.cx     = clamp(radial.cx     + scaled_voltage(IN_FANCY_RADIAL_CX));
            radial.cy     = clamp(radial.cy     + scaled_voltage(IN_FANCY_RADIAL_CY));
            radial.ofade  = clamp(radial.ofade  + scaled_voltage(IN_FANCY_RADIAL_OUTER_FADE));
            radial.radius = clamp(radial.radius + scaled_voltage(IN_FANCY_RADIAL_RADIUS));
            if (radial.ifade > 0.f) {
                radial.icol = modulate_color(fancy_data.radial.icol, IN_FANCY_RADIAL_INNER_H);
            }
            if (radial.ofade > 0.f) {
                radial.ocol = modulate_color(fancy_data.radial.ocol, IN_FANCY_RADIAL_OUTER_H);
            }
            ui->my_cloak->data.radial.init(radial);
        }
    }
}

void Fancy::process_box(const ProcessArgs& args) {
    box_process_count = (box_process_count + 1) % 25;
    if (box_process_count % 25) return;

    fancy_data.boxg.enabled = param_bool(getParam(P_FANCY_BOX_ON));
    fancy_data.boxg.ifade   = fancy_param(getParam(P_FANCY_BOX_INNER_FADE));
    fancy_data.boxg.xshrink = fancy_param(getParam(P_FANCY_BOX_SHRINK_X));
    fancy_data.boxg.yshrink = fancy_param(getParam(P_FANCY_BOX_SHRINK_Y));
    fancy_data.boxg.ofade   = fancy_param(getParam(P_FANCY_BOX_OUTER_FADE));
    fancy_data.boxg.radius  = fancy_param(getParam(P_FANCY_BOX_RADIUS));
    fancy_data.boxg.feather = fancy_param(getParam(P_FANCY_BOX_FEATHER));

    if (ui->my_cloak) {
        ui->my_cloak->data.boxg.enabled = fancy_data.boxg.enabled;
        if (fancy_data.boxg.enabled) {
            BoxGradientData boxg;
            boxg.enabled = fancy_data.boxg.enabled;
            boxg.ifade   = clamp(fancy_data.boxg.ifade   + scaled_voltage(IN_FANCY_BOX_INNER_FADE));
            boxg.xshrink = clamp(fancy_data.boxg.xshrink + scaled_voltage(IN_FANCY_BOX_SHRINK_X));
            boxg.yshrink = clamp(fancy_data.boxg.yshrink + scaled_voltage(IN_FANCY_BOX_SHRINK_Y));
            boxg.ofade   = clamp(fancy_data.boxg.ofade   + scaled_voltage(IN_FANCY_BOX_OUTER_FADE));
            boxg.radius  = clamp(fancy_data.boxg.radius  + scaled_voltage(IN_FANCY_BOX_RADIUS));
            boxg.feather = clamp(fancy_data.boxg.feather + scaled_voltage(IN_FANCY_BOX_FEATHER));
            if (boxg.ifade > 0.f) {
                boxg.icol = modulate_color(fancy_data.boxg.icol, IN_FANCY_BOX_INNER_H);
            } else {
                boxg.icol = fancy_data.boxg.icol;
            }
            if (boxg.ofade > 0.f) {
                boxg.ocol = modulate_color(fancy_data.boxg.ocol, IN_FANCY_BOX_OUTER_H);
            } else {
                boxg.ocol = fancy_data.boxg.ocol;
            }
            ui->my_cloak->data.boxg.init(boxg);
        }
    }
}

void Fancy::process(const ProcessArgs &args)
{
    if (other_fancy || !ui) return;
    if ((0 == ((args.frame + getId()) % PARAM_INTERVAL))) {
        getLight(L_FANCY).setSmoothBrightness(ui->my_cloak ? 1.0 : 0.f, 86.f);
    }

    // amortize parameter updates across frames
    switch (args.frame % 5) {
        case 0: process_image(args); break;
        case 1: process_fill(args); break;
        case 2: process_linear(args); break;
        case 3: process_radial(args); break;
        case 4: process_box(args);  break;
    }
}

}

Model* modelFancyBox = createModel<pachde::Fancy, pachde::FancyUi>("pachde-fancybox");
