#include "Info.hpp"
#include "services/text.hpp"
#include "IHaveColor.hpp" // expander
#include "services/json-help.hpp"
#include "info_symbol.hpp"

namespace pachde {

InfoModule::InfoModule()
{
    info_theme = new InfoTheme;
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
    set_json(root, "text", text);
    set_json_int(root, "copper-target", static_cast<int>(copper_target));
    return root;
}

void InfoModule::dataFromJson(json_t *root) //override
{
    ResizableModule::dataFromJson(root);
    info_theme->load(root);

    text = get_json_string(root, "text");
    int i = get_json_int(root, "copper-target", static_cast<int>(CopperTarget::Panel));
    copper_target = static_cast<CopperTarget>(clamp(i, CopperTarget::First, CopperTarget::Last));
    dirty_settings = true;
}

InfoTheme* InfoModule::getInfoTheme() {
    assert(info_theme);
    return info_theme;
}

NVGcolor InfoModule::expanderColor(rack::engine::Module::Expander& expander)
{
    if (expander.module && (expander.module->model == modelCopper || expander.module->model == modelCopperMini)) {
        auto copper = dynamic_cast<IHaveColor*>(expander.module);
        if (copper) {
            return copper->getColor(1);
        }
    }
    return COLOR_NONE;
}

}

Model* modelInfo = createModel<pachde::InfoModule, pachde::InfoModuleWidget>("pachde-info");
