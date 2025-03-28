#include "Copper.hpp"

namespace pachde {
using namespace rack;

struct CopperMiniSvg {
    static std::string background(Theme theme)
    {
        const char * asset;
        switch (theme) {
        default:
        case Theme::Unset:
        case Theme::Light:
            asset = "res/Copper-mini.svg";
            break;
        case Theme::Dark:
            asset = "res/Copper-mini-Dark.svg";
            break;
        case Theme::HighContrast:
            asset = "res/Copper-mini-HighContrast.svg";
            break;
        }
        return asset::plugin(pluginInstance, asset);
    }
};

// ----------------------------------------------------------------------------
CopperMiniUI::CopperMiniUI(CopperModule * module)
{
    copper_module = module;
    setModule(module);
    theme_holder = module ? module : new ThemeBase();
    theme_holder->setNotify(this);
    applyTheme(GetPreferredTheme(theme_holder));
}

float CopperMiniUI::getHue()
{
    if (copper_module) {
        return copper_module->getHue();
    }
    return 40.f/360.f;
}
void CopperMiniUI::setHue(float hue) {
    if (copper_module) {
        copper_module->setHue(hue);
    }
}
float CopperMiniUI::getSaturation() {
    if (copper_module) {
        return copper_module->getSaturation();
    }
    return .65f;
}
void CopperMiniUI::setSaturation(float sat) {
    if (copper_module) {
        copper_module->setSaturation(sat);
    }
}
float CopperMiniUI::getLightness() {
    if (copper_module) {
        return copper_module->getLightness();
    }
    return .95f;
}
void CopperMiniUI::setLightness(float light) {
    if (copper_module) {
        copper_module->setLightness(light);
    }
}
float CopperMiniUI::getAlpha() {
    if (copper_module) {
        return copper_module->getAlpha();
    }
    return 1.f;
}
void CopperMiniUI::setAlpha(float alpha) {
    if (copper_module) {
        copper_module->setAlpha(alpha);
    }
}
NVGcolor CopperMiniUI::getColor() {
    if (copper_module) {
        return nvgHSLAf(
            copper_module->getHue(),
            copper_module->getSaturation(),
            copper_module->getLightness(),
            copper_module->getAlpha());
    } else {
        return COPPER;
    }
}

constexpr const float KNOB_SPACING = 27.5f;
constexpr const float CONTROL_SPACING = 26.f;
constexpr const float GAP1 = 33.f;
constexpr const float GAP2 = 36.f;
constexpr const float LABEL_XOFFSET = 11.f;
constexpr const float LABEL_YOFFSET = 5.25f;

void CopperMiniUI::makeUi(Theme theme)
{
    assert(children.empty());

    setPanel(createSvgThemePanel<CopperMiniSvg>(theme));

    float center = box.size.x * .5f;
    float y = 30.f;
    auto p = createThemeParamCentered<SmallKnob>(theme, Vec(center,y), module, CopperModule::H_PARAM);
    p->stepIncrementBy = 1.f/360.f;
    addParam(p);
    y += KNOB_SPACING;
    p = createThemeParamCentered<SmallKnob>(theme, Vec(center,y), module, CopperModule::S_PARAM);
    p->stepIncrementBy = .1f;
    addParam(p);
    y += KNOB_SPACING;
    p = createThemeParamCentered<SmallKnob>(theme, Vec(center,y), module, CopperModule::L_PARAM);
    p->stepIncrementBy = .01f;
    addParam(p);
    y += KNOB_SPACING;
    p = createThemeParamCentered<SmallKnob>(theme, Vec(center,y), module, CopperModule::A_PARAM);
    p->stepIncrementBy = .1f;
    addParam(p);

    y += GAP1;

    addInput(createColorInputCentered<ColorPort>(theme, PORT_BLUE, Vec(center,y), module, CopperModule::H_INPUT));
    y += CONTROL_SPACING;
    addInput(createColorInputCentered<ColorPort>(theme, PORT_ORANGE, Vec(center,y), module, CopperModule::S_INPUT));
    y += CONTROL_SPACING;
    addInput(createColorInputCentered<ColorPort>(theme, PORT_YELLOW, Vec(center,y), module, CopperModule::L_INPUT));
    y += CONTROL_SPACING;
    addInput(createColorInputCentered<ColorPort>(theme, PORT_PINK, Vec(center,y), module, CopperModule::A_INPUT));

    y += GAP2;

    addOutput(createColorOutputCentered<ColorPort>(theme, PORT_BLUE, Vec(center,y), module, CopperModule::H_OUT));
    y += CONTROL_SPACING;
    addOutput(createColorOutputCentered<ColorPort>(theme, PORT_ORANGE, Vec(center,y), module, CopperModule::S_OUT));
    y += CONTROL_SPACING;
    addOutput(createColorOutputCentered<ColorPort>(theme, PORT_YELLOW, Vec(center,y), module, CopperModule::L_OUT));
    y += CONTROL_SPACING;
    addOutput(createColorOutputCentered<ColorPort>(theme, PORT_PINK, Vec(center,y), module, CopperModule::A_OUT));

}

void CopperMiniUI::drawLayer(const DrawArgs& args, int layer) 
{
    if (layer != 1) return;
    auto setcolor = getColor();
    auto modcolor = copper_module ? copper_module->getModulatedColor() : setcolor;
    FillRect(args.vg, 4.f, box.size.y -20.f, 11.f, 3.f, setcolor);
    FillRect(args.vg, 15.f, box.size.y -20.f, 11.f, 3.f, modcolor);
}

void CopperMiniUI::draw(const DrawArgs& args)
{
    ModuleWidget::draw(args);
  
    auto vg = args.vg;
    auto color = copper_module ? copper_module->getModulatedColor() : getColor();
    auto textColor = RampGray(G_WHITE);
    switch (GetPreferredTheme(theme_holder)) {
    case Theme::Light: textColor = COLOR_BRAND; break;
    case Theme::Dark: textColor = RampGray(G_45); break;
    case Theme::HighContrast:
    default: break;
    }
    auto font = GetPluginFontRegular();
  
    nvgSave(vg);
    nvgScissor(vg, 0, 0, box.size.x, box.size.y);
    SetTextStyle(vg, font, textColor, 9.5f);
    auto text = rack::color::toHexString(color);
    CenterText(vg, box.size.x * .5f, box.size.y - 22.f, 1 + text.c_str(), nullptr);
    nvgRestore(vg);
}

void CopperMiniUI::applyTheme(Theme theme)
{
    if (children.empty()) {
        makeUi(theme);
    } else {
        SetChildrenTheme(this, theme);
    }        
}

void CopperMiniUI::onChangeTheme(ChangedItem item)
{
    switch (item) {
    case ChangedItem::Theme:
        applyTheme(GetPreferredTheme(theme_holder));
        break;
    case ChangedItem::DarkTheme:
    case ChangedItem::FollowDark:
        if (theme_holder->getFollowRack()) {
            applyTheme(GetPreferredTheme(theme_holder));
        }
        break;
    case ChangedItem::MainColor:
        break;
    case ChangedItem::Screws:
        break;
    }
}

void CopperMiniUI::step()
{
   bool changed = theme_holder->pollRackDarkChanged();

    if (copper_module) {
        // sync with module for change from presets
        if (!changed && copper_module->isDirty()) {
            applyTheme(GetPreferredTheme(theme_holder));
        }
        copper_module->setClean();
        copper_module->updateCableConnections();
    }
    ModuleWidget::step();
}

template <typename Self>
void AddColorItem(Self* self, Menu* menu, const char * name, PackedColor color, PackedColor current)
{
    menu->addChild(createColorMenuItem(
        color, name, "",
        [=]() { return current == color; },
        [=]() { 
            auto new_color = fromPacked(color);
            self->setHue(Hue1(new_color));
            self->setSaturation(Saturation(new_color));
            self->setLightness(Lightness(new_color));
            self->setAlpha(new_color.a);
        }
        ));
}

void CopperMiniUI::appendContextMenu(rack::ui::Menu* menu)
{
    if (!this->module) return;
    AddThemeMenu(menu, theme_holder, false, false);
    menu->addChild(createSubmenuItem("Palette color", "",
        [=](Menu *menu)
        {
            auto current = toPacked(getColor());
            for (auto pco = stock_colors; nullptr != pco->name; ++pco) {
                AddColorItem<CopperMiniUI>(this, menu, pco->name, pco->color, current);
            }
        })); 
    menu->addChild(createMenuItem("Copy hex color", "", [=]() {
        auto hex = rack::color::toHexString(getColor());
        glfwSetClipboardString(nullptr, hex.c_str());
    }));
    menu->addChild(createMenuItem("Paste hex color", "", [=]() {
        std::string hex = glfwGetClipboardString(nullptr);
        if (!hex.empty()) {
            auto new_color = rack::color::fromHexString(hex);
            setHue(Hue1(new_color));
            setSaturation(Saturation(new_color));
            setLightness(Lightness(new_color));
            setAlpha(new_color.a);
        }
    }));

#ifdef USE_BAD_HEX_INPUT
    // BUGBUG: We don't have working inverses yet for round-tripping nvgHSL(),
    menu->addChild(createSubmenuItem("Color", "",
        [=](Menu *menu)
        {
            MenuTextField *editField = new MenuTextField();
            editField->box.size.x = 100;
            auto color = getColor();
            editField->setText(rack::color::toHexString(color));
            editField->changeHandler = [=](std::string text) {
                auto color = COLOR_NONE;
                if (!text.empty() && text[0] == '#') {
                    color = rack::color::fromHexString(text);
                    setHue(Hue1(color));
                    setSaturation(Saturation(color));
                    setLightness(Lightness(color));
                    setAlpha(color.a);
                }
            };
            menu->addChild(editField);
        }));
#endif
}


}
Model *modelCopperMini = createModel<pachde::CopperModule, pachde::CopperMiniUI>("pachde-copper-mini");