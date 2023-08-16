#include "Info.hpp"
#include "../text.hpp"
#include "../Copper/Copper.hpp" // expander
#include "info_symbol.hpp"

namespace pachde {

InfoModule::InfoModule()
{
    info_theme = new InfoTheme(this);
    minWidth = 4;
}

void InfoModule::onReset(const ResetEvent& e) //override
{
    dirty_settings = true;
    setCopperTarget(CopperTarget::Panel);
    if (info_theme) { info_theme->reset(); }
}

void InfoModule::onRandomize(const RandomizeEvent& e) //override
{
    setCopperTarget(static_cast<CopperTarget>(random::get<uint32_t>() % 3));
    if (info_theme) { info_theme->randomize(); }
    dirty_settings = true;
}

json_t* InfoModule::dataToJson() //override
{
    json_t* root = ResizableModule::dataToJson();
    root = info_theme->save(root);
    json_object_set_new(root, "text", json_stringn(text.c_str(), text.size()));
    json_object_set_new(root, "copper-target", json_integer(static_cast<int>(copper_target)));
    return root;
}

void InfoModule::dataFromJson(json_t *root) //override
{
    ResizableModule::dataFromJson(root);
    info_theme->load(root);

    json_t *j = json_object_get(root, "text");
    if (j) {
        text = json_string_value(j);
    }
    j = json_object_get(root, "copper-target");
    if (j) {
        copper_target = static_cast<CopperTarget>(clamp(json_integer_value(j), CopperTarget::First, CopperTarget::Last));
    }

    dirty_settings = true;
}

InfoTheme* InfoModule::getInfoTheme() {
    assert(info_theme);
    return info_theme;
}

NVGcolor InfoModule::expanderColor(rack::engine::Module::Expander& expander)
{
    if (expander.module && expander.module->model == modelCopper) {
        CopperModule* copper = dynamic_cast<CopperModule*>(expander.module);
        if (copper) {
            return copper->getModulatedColor();
        }
    }
    return COLOR_NONE;
}

}

Model* modelInfo = createModel<pachde::InfoModule, pachde::InfoModuleWidget>("pachde-info");
