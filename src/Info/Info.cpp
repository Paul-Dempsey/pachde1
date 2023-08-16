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

json_t* InfoModule::dataToJson() //override
{
    json_t* root = ResizableModule::dataToJson();
    root = info_theme->save(root);
    json_object_set_new(root, "text", json_stringn(text.c_str(), text.size()));
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
