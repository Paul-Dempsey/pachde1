#include "Fancy.hpp"
#include "services/json-help.hpp"
#include "services/rack-help.hpp"

namespace pachde {

constexpr const int PARAM_INTERVAL = 64;

Fancy::Fancy() {
    std::vector<std::string> off_on{"off", "on"};
    config(Params::N_PARAMS, Inputs::N_INPUTS, Outputs::N_OUTPUTS, Lights::N_LIGHTS);

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

void color_from_json(json_t* root, const char * key, PackedColor& color) {
    const char * spec = get_json_cstring(root, key);
    if (spec && *spec) {
        color = parseColor(spec, color);
    }
}

void Fancy::dataFromJson(json_t* root) {
    Base::dataFromJson(root);
    shouting   = get_json_bool(root, "shouting", shouting);
    fancy      = get_json_bool(root, "fancy", fancy);
    show_ports = get_json_bool(root, "show-ports", show_ports);
    color_from_json(root, "fill-color", fancy_data.fill.color);
    color_from_json(root, "linear-icol", fancy_data.linear.icol);
    color_from_json(root, "linear-ocol", fancy_data.linear.ocol);
    color_from_json(root, "radial-icol", fancy_data.radial.icol);
    color_from_json(root, "radial-ocol", fancy_data.radial.ocol);
    color_from_json(root, "box-icol", fancy_data.boxg.icol);
    color_from_json(root, "box-ocol", fancy_data.boxg.ocol);
}

inline bool param_bool(rack::engine::Param& param) { return param.getValue() >= .5f; }
inline float fancy_param(rack::engine::Param& param) { return .01f * param.getValue(); }
void Fancy::process(const ProcessArgs &args)
{
    if (other_fancy || !ui) return;
    if ((0 == ((args.frame + getId()) % PARAM_INTERVAL))) {
        getLight(L_FANCY).setSmoothBrightness(ui->my_cloak ? 1.0 : 0.f, 86.f);
        if (ui->my_cloak) {
            ui->my_cloak->data.fill.enabled
                = fancy_data.fill.enabled = param_bool(getParam(P_FANCY_FILL_ON));
            if (fancy_data.fill.enabled) {
                fancy_data.fill.fade = ui->my_cloak->data.fill.fade = fancy_param(getParam(P_FANCY_FILL_FADE));
            }
            fancy_data.linear.enabled = ui->my_cloak->data.linear.enabled = param_bool(getParam(P_FANCY_LINEAR_ON));
            if (fancy_data.linear.enabled) {
                fancy_data.linear.ifade = ui->my_cloak->data.linear.ifade = fancy_param(getParam(P_FANCY_LINEAR_START_FADE));
                fancy_data.linear.x1    = ui->my_cloak->data.linear.x1    = fancy_param(getParam(P_FANCY_LINEAR_X1));
                fancy_data.linear.y1    = ui->my_cloak->data.linear.y1    = fancy_param(getParam(P_FANCY_LINEAR_Y1));
                fancy_data.linear.ofade = ui->my_cloak->data.linear.ofade = fancy_param(getParam(P_FANCY_LINEAR_END_FADE));
                fancy_data.linear.x2    = ui->my_cloak->data.linear.x2    = fancy_param(getParam(P_FANCY_LINEAR_X2));
                fancy_data.linear.y2    = ui->my_cloak->data.linear.y2    = fancy_param(getParam(P_FANCY_LINEAR_Y2));
            }
            fancy_data.radial.enabled = ui->my_cloak->data.radial.enabled = param_bool(getParam(P_FANCY_RADIAL_ON));
            if (fancy_data.radial.enabled) {
                fancy_data.radial.ifade  = ui->my_cloak->data.radial.ifade  = fancy_param(getParam(P_FANCY_RADIAL_INNER_FADE));
                fancy_data.radial.cx     = ui->my_cloak->data.radial.cx     = fancy_param(getParam(P_FANCY_RADIAL_CX));
                fancy_data.radial.cy     = ui->my_cloak->data.radial.cy     = fancy_param(getParam(P_FANCY_RADIAL_CY));
                fancy_data.radial.ofade  = ui->my_cloak->data.radial.ofade  = fancy_param(getParam(P_FANCY_RADIAL_OUTER_FADE));
                fancy_data.radial.radius = ui->my_cloak->data.radial.radius = fancy_param(getParam(P_FANCY_RADIAL_RADIUS));
            }
            fancy_data.boxg.enabled = ui->my_cloak->data.boxg.enabled = param_bool(getParam(P_FANCY_BOX_ON));
            if (fancy_data.boxg.enabled) {
                fancy_data.boxg.ifade   = ui->my_cloak->data.boxg.ifade    = fancy_param(getParam(P_FANCY_BOX_INNER_FADE));
                fancy_data.boxg.xshrink = ui->my_cloak->data.boxg.xshrink  = fancy_param(getParam(P_FANCY_BOX_SHRINK_X));
                fancy_data.boxg.yshrink = ui->my_cloak->data.boxg.yshrink  = fancy_param(getParam(P_FANCY_BOX_SHRINK_Y));
                fancy_data.boxg.ofade   = ui->my_cloak->data.boxg.ofade    = fancy_param(getParam(P_FANCY_BOX_OUTER_FADE));
                fancy_data.boxg.radius  = ui->my_cloak->data.boxg.radius   = fancy_param(getParam(P_FANCY_BOX_RADIUS));
                fancy_data.boxg.feather = ui->my_cloak->data.boxg.feather  = fancy_param(getParam(P_FANCY_BOX_FEATHER));
            }
        } else {
            fancy_data.fill.enabled = param_bool(getParam(P_FANCY_FILL_ON));
            if (fancy_data.fill.enabled) {
                fancy_data.fill.fade = getParam(P_FANCY_FILL_FADE).getValue();
            }
            fancy_data.linear.enabled = param_bool(getParam(P_FANCY_LINEAR_ON));
            if (fancy_data.linear.enabled) {
                fancy_data.linear.ifade = fancy_param(getParam(P_FANCY_LINEAR_START_FADE));
                fancy_data.linear.x1    = fancy_param(getParam(P_FANCY_LINEAR_X1));
                fancy_data.linear.y1    = fancy_param(getParam(P_FANCY_LINEAR_Y1));
                fancy_data.linear.ofade = fancy_param(getParam(P_FANCY_LINEAR_END_FADE));
                fancy_data.linear.x2    = fancy_param(getParam(P_FANCY_LINEAR_X2));
                fancy_data.linear.y2    = fancy_param(getParam(P_FANCY_LINEAR_Y2));
            }
            fancy_data.radial.enabled = param_bool(getParam(P_FANCY_RADIAL_ON));
            if (fancy_data.radial.enabled) {
                fancy_data.radial.ifade  = fancy_param(getParam(P_FANCY_RADIAL_INNER_FADE));
                fancy_data.radial.cx     = fancy_param(getParam(P_FANCY_RADIAL_CX));
                fancy_data.radial.cy     = fancy_param(getParam(P_FANCY_RADIAL_CY));
                fancy_data.radial.ofade  = fancy_param(getParam(P_FANCY_RADIAL_OUTER_FADE));
                fancy_data.radial.radius = fancy_param(getParam(P_FANCY_RADIAL_RADIUS));
            }
            fancy_data.boxg.enabled = param_bool(getParam(P_FANCY_BOX_ON));
            if (fancy_data.boxg.enabled) {
                fancy_data.boxg.ifade   = fancy_param(getParam(P_FANCY_BOX_INNER_FADE));
                fancy_data.boxg.xshrink = fancy_param(getParam(P_FANCY_BOX_SHRINK_X));
                fancy_data.boxg.yshrink = fancy_param(getParam(P_FANCY_BOX_SHRINK_Y));
                fancy_data.boxg.ofade   = fancy_param(getParam(P_FANCY_BOX_OUTER_FADE));
                fancy_data.boxg.radius  = fancy_param(getParam(P_FANCY_BOX_RADIUS));
                fancy_data.boxg.feather = fancy_param(getParam(P_FANCY_BOX_FEATHER));
            }
        }
    }
}

}

Model* modelFancyBox = createModel<pachde::Fancy, pachde::FancyUi>("pachde-fancybox");
