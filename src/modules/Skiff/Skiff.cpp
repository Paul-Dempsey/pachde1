#include "Skiff.hpp"
#include "services/json-help.hpp"
#include "services/rack-help.hpp"

using namespace svg_theme;

namespace pachde {

Skiff::Skiff() {
}

void Skiff::set_defaults() {
    unscrewed = nojacks = calm = derailed = depaneled = dark_ages = false;
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
    set_json(root, "theme", theme_name);
    set_json(root, "shouting", shouting);
    set_json(root, "jack-shape", jack_shape_name(jack_shape));
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
    dark_ages   = get_json_bool(root, "dark-ages", dark_ages);
    shouting    = get_json_bool(root, "shouting", shouting);
    jack_shape  = parse_jack_shape(get_json_cstring(root, "jack-shape"));
    rail_theme  = RailThemeSetting_from_json(root);
    if (ui) {
        ui->from_module();
    }
};

void Skiff::random_settings() {
    unscrewed  = random::uniform() < .5f;
    nojacks    = random::uniform() > .5f;
    calm       = random::uniform() < .5f;
    dark_ages  = random::uniform() > .5f;
    //derailed   = random::uniform() > .5f;
    depaneled  = random::uniform() < .5f;
    auto n = random::u32() % 7;
    auto it = known_rails.cbegin();
    if (n > 0) std::advance(it, n);
    rail = *it;
}


void Skiff::onRandomize(const RandomizeEvent& e) {
    random_settings();
    if (ui) ui->from_module();
}

}
Model* modelSkiff = createModel<pachde::Skiff, pachde::SkiffUi>("pachde-skiff");
