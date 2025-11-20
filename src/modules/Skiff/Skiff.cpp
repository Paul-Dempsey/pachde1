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
    configParam(P_FANCY_FILL_FADE, 0.f, 1.f, 0.f, "FancyBox Fill fade", "%", 0.f, 100.f);

    configSwitch(P_FANCY_LINEAR_ON, 0.f, 1.f, 0.f, "Enable FancyBox Linear gradient", off_on);
    configParam(P_FANCY_LINEAR_START_FADE, 0.f, 10.f, 0.f, "FancyBox Linear start fade", "%", 0.f, 10.f);
    configParam(P_FANCY_LINEAR_X1,         0.f, 10.f, 0.f, "FancyBox Linear x1", "%", 0.f, 10.f);
    configParam(P_FANCY_LINEAR_Y1,         0.f, 10.f, 0.f, "FancyBox Linear y1", "%", 0.f, 10.f);
    configParam(P_FANCY_LINEAR_END_FADE,   0.f, 10.f, 0.f, "FancyBox Linear end fade", "%", 0.f, 10.f);
    configParam(P_FANCY_LINEAR_X2,         0.f, 10.f, 0.f, "FancyBox Linear x2", "%", 0.f, 10.f);
    configParam(P_FANCY_LINEAR_Y2,         0.f, 10.f, 0.f, "FancyBox Linear y2", "%", 0.f, 10.f);

    configSwitch(P_FANCY_RADIAL_ON, 0.f, 1.f, 0.f, "Enable FancyBox Radial gradient", off_on);
    configParam(P_FANCY_RADIAL_INNER_FADE, 0.f, 10.f, 0.f,  "FancyBox Radial inner fade", "%", 0.f, 10.f);
    configParam(P_FANCY_RADIAL_CX,         0.f, 10.f, 0.5f, "FancyBox Radial cx", "%", 0.f, 10.f);
    configParam(P_FANCY_RADIAL_CY,         0.f, 10.f, 0.5f, "FancyBox Radial cy", "%", 0.f, 10.f);
    configParam(P_FANCY_RADIAL_OUTER_FADE, 0.f, 10.f, 0.f,  "FancyBox Radial outer fade", "%", 0.f, 10.f);
    configParam(P_FANCY_RADIAL_RADIUS,     0.f, 10.f, 0.8f, "FancyBox Radial radius", "%", 0.f, 10.f);

    configSwitch(P_FANCY_BOX_ON, 0.f, 1.f, 0.f, "Enable FancyBox Box gradient (vignette)", off_on);
    configParam(P_FANCY_BOX_INNER_FADE,    0.f, 10.f, 0.f,  "FancyBox Box inner fade", "%", 0.f, 10.f);
    configParam(P_FANCY_BOX_SHRINK_X,      0.f, 10.f, 0.f,  "FancyBox Box shrink x", "%", 0.f, 10.f);
    configParam(P_FANCY_BOX_SHRINK_Y,      0.f, 10.f, 0.f,  "FancyBox Box shrink y", "%", 0.f, 10.f);
    configParam(P_FANCY_BOX_OUTER_FADE,    0.f, 10.f, 0.f,  "FancyBox Box outer fade", "%", 0.f, 10.f);
    configParam(P_FANCY_BOX_RADIUS,        0.f, 10.f, 0.2f, "FancyBox Box radius", "%", 0.f, 10.f);
    configParam(P_FANCY_BOX_FEATHER,       0.f, 10.f, 0.1f, "FancyBox Box feather", "%", 0.f, 10.f);
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
    set_json(root, "fancy", fancy);
    set_json(root, "theme", theme_name);
    set_json(root, "shouting", shouting);
    RailThemeSetting_to_json(root, rail_theme);
    return root;
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
    fancy       = get_json_bool(root, "fancy", fancy);
    shouting    = get_json_bool(root, "shouting", shouting);
    rail_theme  = RailThemeSetting_from_json(root);
};

void Skiff::random_settings() {
    unscrewed  = random::uniform() < .5f;
    nojacks    = random::uniform() > .5f;
    calm       = random::uniform() < .5f;
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
inline float onevten(rack::engine::Param& param)  { return param.getValue() * .1; }

void Skiff::process(const ProcessArgs &args)
{
    if (other_skiff || !ui) return;
    if ((0 == ((args.frame + getId()) % PARAM_INTERVAL))) {
        fancy_data.fill.enabled = param_bool(getParam(P_FANCY_FILL_ON));
        if (fancy_data.fill.enabled) {
            fancy_data.fill.fade = onevten(getParam(P_FANCY_FILL_FADE));
        }
        fancy_data.linear.enabled = param_bool(getParam(P_FANCY_LINEAR_ON));
        if (fancy_data.linear.enabled) {
            fancy_data.linear.ifade = onevten(getParam(P_FANCY_LINEAR_START_FADE));
            fancy_data.linear.x1    = onevten(getParam(P_FANCY_LINEAR_X1));
            fancy_data.linear.y1    = onevten(getParam(P_FANCY_LINEAR_Y1));
            fancy_data.linear.ofade = onevten(getParam(P_FANCY_LINEAR_END_FADE));
            fancy_data.linear.x2    = onevten(getParam(P_FANCY_LINEAR_X2));
            fancy_data.linear.y2    = onevten(getParam(P_FANCY_LINEAR_Y2));
        }
        fancy_data.radial.enabled = param_bool(getParam(P_FANCY_RADIAL_ON));
        if (fancy_data.radial.enabled) {
            fancy_data.radial.ifade  = onevten(getParam(P_FANCY_RADIAL_INNER_FADE));
            fancy_data.radial.cx     = onevten(getParam(P_FANCY_RADIAL_CX));
            fancy_data.radial.cy     = onevten(getParam(P_FANCY_RADIAL_CY));
            fancy_data.radial.ofade  = onevten(getParam(P_FANCY_RADIAL_OUTER_FADE));
            fancy_data.radial.radius = onevten(getParam(P_FANCY_RADIAL_RADIUS));
        }
        fancy_data.boxg.enabled = param_bool(getParam(P_FANCY_BOX_ON));
        if (fancy_data.boxg.enabled) {
            fancy_data.boxg.ifade   = onevten(getParam(P_FANCY_BOX_INNER_FADE));
            fancy_data.boxg.xshrink = onevten(getParam(P_FANCY_BOX_SHRINK_X));
            fancy_data.boxg.yshrink = onevten(getParam(P_FANCY_BOX_SHRINK_Y));
            fancy_data.boxg.ofade   = onevten(getParam(P_FANCY_BOX_OUTER_FADE));
            fancy_data.boxg.radius  = onevten(getParam(P_FANCY_BOX_RADIUS));
            fancy_data.boxg.feather = onevten(getParam(P_FANCY_BOX_FEATHER));
        }
    }
}

}
Model* modelSkiff = createModel<pachde::Skiff, pachde::SkiffUi>("pachde-skiff");
