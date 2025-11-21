#include "Skiff.hpp"
#include "services/json-help.hpp"
#include "services/rack-help.hpp"

using namespace svg_theme;

namespace pachde {
constexpr const int PARAM_INTERVAL = 64;

Skiff::Skiff() {
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
    configParam(P_FANCY_RADIAL_INNER_FADE, 0.f, 100.f, 0.f,  "FancyBox Radial inner fade", "%");
    configParam(P_FANCY_RADIAL_CX,         0.f, 100.f, 50.f, "FancyBox Radial cx", "%");
    configParam(P_FANCY_RADIAL_CY,         0.f, 100.f, 50.f, "FancyBox Radial cy", "%");
    configParam(P_FANCY_RADIAL_OUTER_FADE, 0.f, 100.f, 0.f,  "FancyBox Radial outer fade", "%");
    configParam(P_FANCY_RADIAL_RADIUS,     0.f, 100.f, 0.8f, "FancyBox Radial radius", "%");

    configSwitch(P_FANCY_BOX_ON, 0.f, 1.f, 0.f, "Enable FancyBox Box gradient (vignette)", off_on);
    configParam(P_FANCY_BOX_INNER_FADE,    0.f, 100.f, 100.f,  "FancyBox Box inner fade", "%");
    configParam(P_FANCY_BOX_SHRINK_X,      0.f, 100.f, 1.f,  "FancyBox Box shrink x", "%");
    configParam(P_FANCY_BOX_SHRINK_Y,      0.f, 100.f, 1.f,  "FancyBox Box shrink y", "%");
    configParam(P_FANCY_BOX_OUTER_FADE,    0.f, 100.f, 0.f,  "FancyBox Box outer fade", "%");
    configParam(P_FANCY_BOX_RADIUS,        0.f, 100.f, 2.f, "FancyBox Box radius", "%");
    configParam(P_FANCY_BOX_FEATHER,       0.f, 100.f, 1.f, "FancyBox Box feather", "%");
}

void Skiff::set_defaults() {
    unscrewed = nojacks = calm = derailed = depaneled = fancy = false;
    rail = "Rack";
}

json_t* Skiff::dataToJson() {
    json_t *root = Base::dataToJson();
    set_json(root, "rail", rail);
    set_json(root, "rail-folder", rail_folder);
    set_json(root, "unscrewed", unscrewed);
    set_json(root, "no-jacks", nojacks);
    set_json(root, "calm", calm);
    set_json(root, "derailed", derailed);
    set_json(root, "depaneled", depaneled);
    set_json(root, "dark-ages", dark_ages);
    set_json(root, "fancy", fancy);
    set_json(root, "theme", theme_name);
    set_json(root, "shouting", shouting);
    RailThemeSetting_to_json(root, rail_theme);
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

void Skiff::dataFromJson(json_t* root) {
    Base::dataFromJson(root);
    rail        = get_json_string(root, "rail", rail);
    rail_folder = get_json_string(root, "rail-folder", rail_folder);
    unscrewed   = get_json_bool(root, "unscrewed", unscrewed);
    nojacks     = get_json_bool(root, "no-jacks", nojacks);
    calm        = get_json_bool(root, "calm", calm);
    derailed    = get_json_bool(root, "derailed", derailed);
    depaneled   = get_json_bool(root, "depaneled", depaneled);
    dark_ages   = get_json_bool(root, "dark-ages", dark_ages);
    fancy       = get_json_bool(root, "fancy", fancy);
    shouting    = get_json_bool(root, "shouting", shouting);
    rail_theme  = RailThemeSetting_from_json(root);

    color_from_json(root, "fill-color", fancy_data.fill.color);
    color_from_json(root, "linear-icol", fancy_data.linear.icol);
    color_from_json(root, "linear-ocol", fancy_data.linear.ocol);
    color_from_json(root, "radial-icol", fancy_data.radial.icol);
    color_from_json(root, "radial-ocol", fancy_data.radial.ocol);
    color_from_json(root, "box-icol", fancy_data.boxg.icol);
    color_from_json(root, "box-ocol", fancy_data.boxg.ocol);
};

void Skiff::random_settings() {
    unscrewed  = random::uniform() < .5f;
    nojacks    = random::uniform() > .5f;
    calm       = random::uniform() < .5f;
    dark_ages  = random::uniform() > .5f;
    //derailed   = random::uniform() > .5f;
    depaneled  = random::uniform() < .5f;
    //fancy      = random::uniform() > .5f;
    auto n = random::u32() % 7;
    auto it = known_rails.cbegin();
    if (n > 0) std::advance(it, n);
    rail = *it;
}


void Skiff::onRandomize(const RandomizeEvent& e) {
    random_settings();
    if (ui) ui->from_module();
}

inline bool param_bool(rack::engine::Param& param) { return param.getValue() >= .5f; }
inline float fancy_param(rack::engine::Param& param) { return .01f * param.getValue(); }

void Skiff::process(const ProcessArgs &args)
{
    if (other_skiff || !ui) return;
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
Model* modelSkiff = createModel<pachde::Skiff, pachde::SkiffUi>("pachde-skiff");
