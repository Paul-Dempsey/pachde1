#include "Info.hpp"
#include "services/text.hpp"
#include "IHaveColor.hpp" // expander
#include "services/json-help.hpp"
#include "services/rack-help.hpp"
#include "info_symbol.hpp"

namespace pachde {

InfoModule::InfoModule()
{
    settings = new InfoSettings();
    minWidth = 2;
    config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
    configSwitch(P_HAlign, 0.f, 2.f, 1.f, "Horizontal alignment", {
        "Left", "Center", "Right"
    });
    configSwitch(P_VAlign, 0.f, 2.f, 1.f, "Horizontal alignment", {
        "Top", "Middle", "Bottom"
    });
    configSwitch(P_Orientation, 0.f, 3.f, 0.f, "Orientation", {
        "Normal", "Down", "Up", "Inverted"
    });
    dp3(configParam(P_MarginLeft, 0.f, 150.f, 0.f, "Left margin", "px"));
    dp3(configParam(P_MarginRight, 0.f, 150.f, 0.f, "Right margin", "px"));
    snap(dp3(configParam(P_FontSize,
        info_constant::MIN_FONT_SIZE,
        info_constant::MAX_FONT_SIZE,
        info_constant::DEFAULT_FONT_SIZE,
        "Font size", "px"
    )));
    configSwitch(P_CopperLeft, 0.f, 2.f, 0.f, "Left Copper target", {
        "Panel", "Text", "None"
    });
    configSwitch(P_CopperRight, 0.f, 2.f, 1.f, "Right Copper target", {
        "Panel", "Text", "None"
    });
}

void InfoModule::onReset(const ResetEvent& e) //override
{
    dirty_settings = true;
    setLeftCopperTarget(CopperTarget::Panel);
    setRightCopperTarget(CopperTarget::Text);
    if (settings) { settings->reset(); }
}

void InfoModule::onRandomize(const RandomizeEvent& e) //override
{
    setLeftCopperTarget(static_cast<CopperTarget>(random::get<uint32_t>() % 3));
    do {
        setRightCopperTarget(static_cast<CopperTarget>(random::get<uint32_t>() % 3));
    } while (left_copper_target == right_copper_target);
    if (settings) { settings->randomize(); }
    dirty_settings = true;
}

json_t* InfoModule::dataToJson() //override
{
    json_t* root = Base::dataToJson();
    settings->save(root);
    set_json(root, "text", text);
    set_json_int(root, "left-copper-target", static_cast<int>(left_copper_target));
    set_json_int(root, "right-copper-target", static_cast<int>(right_copper_target));
    return root;
}

void InfoModule::dataFromJson(json_t *root) //override
{
    Base::dataFromJson(root);
    settings->load(root);
    text = get_json_string(root, "text");
    int i = get_json_int(root, "left-copper-target", static_cast<int>(CopperTarget::Panel));
    left_copper_target = static_cast<CopperTarget>(clamp(i, CopperTarget::First, CopperTarget::Last));

    i = get_json_int(root, "right-copper-target", static_cast<int>(CopperTarget::Text));
    right_copper_target = static_cast<CopperTarget>(clamp(i, CopperTarget::First, CopperTarget::Last));
    dirty_settings = true;

    getParam(P_HAlign).setValue((float)settings->getHorizontalAlignment());
    getParam(P_VAlign).setValue((float)settings->getVerticalAlignment());
    getParam(P_Orientation).setValue((float)settings->getOrientation());
    getParam(P_MarginLeft).setValue(settings->left_margin);
    getParam(P_MarginRight).setValue(settings->right_margin);
    getParam(P_FontSize).setValue(settings->font_size);
    getParam(P_CopperLeft).setValue(left_copper_target);
    getParam(P_CopperRight).setValue(right_copper_target);
}

InfoSettings* InfoModule::getSettings() {
    assert(settings);
    return settings;
}

bool InfoModule::expanderColor(rack::engine::Module::Expander& expander, NVGcolor& result)
{
    if (expander.module && (expander.module->model == modelCopper || expander.module->model == modelCopperMini)) {
        auto copper = dynamic_cast<IHaveColor*>(expander.module);
        if (copper && copper->colorExtenderEnabled()) {
            result = copper->getColor(1);
            return true;
        }
    }
    return false;
}

void InfoModule::process(const ProcessArgs &args)
{
    if (settings && (0 == ((args.frame + getId()) % 53))) {
        settings->setHorizontalAlignment((HAlign)round(getParam(P_HAlign).getValue()));
        settings->setVerticalAlignment((VAlign)round(getParam(P_VAlign).getValue()));
        settings->setOrientation((Orientation)round(getParam(P_Orientation).getValue()));
        settings->left_margin = getParam(P_MarginLeft).getValue();
        settings->right_margin = getParam(P_MarginRight).getValue();
        settings->setFontSize(getParam(P_FontSize).getValue());
        left_copper_target = (CopperTarget)getParam(P_CopperLeft).getValue();
        right_copper_target = (CopperTarget)getParam(P_CopperRight).getValue();
    }
}
}

Model* modelInfo = createModel<pachde::InfoModule, pachde::InfoModuleWidget>("pachde-info");
