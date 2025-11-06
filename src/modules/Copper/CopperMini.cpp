#include "Copper.hpp"
#include "widgets/color-picker.hpp"
#include "widgets/hamburger.hpp"
#include "widgets/widgetry.hpp"
using namespace widgetry;

namespace pachde {
using namespace rack;

struct CopperMiniSvg {
    static std::string background() {
        return asset::plugin(pluginInstance, "res/Copper-mini.svg");
    }
};

// ----------------------------------------------------------------------------
CopperMiniUI::CopperMiniUI(CopperModule * module)
{
    copper_module = module;
    setModule(module);
    theme_holder = module ? module : new ThemeBase();
    theme_holder->setNotify(this);
    setTheme(GetPreferredTheme(theme_holder));
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

void CopperMiniUI::setColor(NVGcolor color)
{
    setHue(Hue1(color));
    setSaturation(Saturation1(color));
    setLightness(Lightness(color));
    setAlpha(color.a);
}

constexpr const float KNOB_SPACING = 27.5f;
constexpr const float CONTROL_SPACING = 26.f;
constexpr const float GAP1 = 33.f;
constexpr const float GAP2 = 36.f;

void CopperMiniUI::makeUi(Theme theme)
{
    assert(children.empty());
    auto svg_theme = getThemeCache().getTheme(ThemeName(theme));

    setPanel(createSvgThemePanel<CopperMiniSvg>(&my_svgs, svg_theme));

    float center = box.size.x * .5f;
    float y = 30.f;
    auto p = Center(createThemeSvgParam<SmallKnob>(&my_svgs, Vec(center,y), module, CopperModule::H_PARAM));
    p->step_increment_by = 1.f/360.f;
    addParam(p);

    y += KNOB_SPACING;
    p = Center(createThemeSvgParam<SmallKnob>(&my_svgs, Vec(center,y), module, CopperModule::S_PARAM));
    p->step_increment_by = .1f;
    addParam(p);

    y += KNOB_SPACING;
    p = Center(createThemeSvgParam<SmallKnob>(&my_svgs, Vec(center,y), module, CopperModule::L_PARAM));
    p->step_increment_by = .01f;
    addParam(p);

    y += KNOB_SPACING;
    p = Center(createThemeSvgParam<SmallKnob>(&my_svgs, Vec(center,y), module, CopperModule::A_PARAM));
    p->step_increment_by = .1f;
    addParam(p);

    y += GAP1;

    addInput(Center(createColorInput<ColorPort>(theme, colors::PortBlue, Vec(center,y), module, CopperModule::H_INPUT)));
    y += CONTROL_SPACING;
    addInput(Center(createColorInput<ColorPort>(theme, colors::PortOrange, Vec(center,y), module, CopperModule::S_INPUT)));
    y += CONTROL_SPACING;
    addInput(Center(createColorInput<ColorPort>(theme, colors::PortYellow, Vec(center,y), module, CopperModule::L_INPUT)));
    y += CONTROL_SPACING;
    addInput(Center(createColorInput<ColorPort>(theme, colors::PortPink, Vec(center,y), module, CopperModule::A_INPUT)));

    y += GAP2;

    addOutput(Center(createColorOutput<ColorPort>(theme, colors::PortBlue, Vec(center,y), module, CopperModule::H_OUT)));
    y += CONTROL_SPACING;
    addOutput(Center(createColorOutput<ColorPort>(theme, colors::PortOrange, Vec(center,y), module, CopperModule::S_OUT)));
    y += CONTROL_SPACING;
    addOutput(Center(createColorOutput<ColorPort>(theme, colors::PortYellow, Vec(center,y), module, CopperModule::L_OUT)));
    y += CONTROL_SPACING;
    addOutput(Center(createColorOutput<ColorPort>(theme, colors::PortPink, Vec(center,y), module, CopperModule::A_OUT)));

    my_svgs.changeTheme(svg_theme);
}

void CopperMiniUI::drawLayer(const DrawArgs& args, int layer)
{
    if (layer != 1) return;
    auto setcolor = getColor();
    auto modcolor = copper_module ? copper_module->getModulatedColor() : setcolor;
    FillRect(args.vg, 3.5f, box.size.y -20.f, 12.5f, 4.f, setcolor);
    FillRect(args.vg, 15.f, box.size.y -20.f, 12.5f, 4.f, modcolor);
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

void CopperMiniUI::setTheme(Theme theme)
{
    if (children.empty()) {
        makeUi(theme);
    } else {
        my_svgs.changeTheme(getThemeCache().getTheme(ThemeName(theme)));
        sendChildrenThemeColor(this, theme, GetPreferredColor(theme_holder));
        sendDirty(this);
    }
}

void CopperMiniUI::onChangeTheme(ChangedItem item)
{
    switch (item) {
    case ChangedItem::Theme:
    case ChangedItem::MainColor:
        setTheme(GetPreferredTheme(theme_holder));
        break;
    case ChangedItem::Screws:
        break;
    }
}

void CopperMiniUI::step()
{
   bool changed = theme_holder->pollRackThemeChanged();

    if (copper_module) {
        // sync with module for change from presets
        if (!changed && copper_module->isDirty()) {
            setTheme(GetPreferredTheme(theme_holder));
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
        [=]() { self->setColor(fromPacked(color)); }
        ));
}

void CopperMiniUI::appendContextMenu(rack::ui::Menu* menu)
{
    if (!this->module) return;
    menu->addChild(createMenuLabel<HamburgerTitle>("#d Copper Mini"));
    AddThemeMenu(menu, this, theme_holder, false, false);
    menu->addChild(createSubmenuItem("Palette color", "",
        [=](Menu *menu)
        {
            auto current = toPacked(getColor());
            for (auto pco = stock_colors; nullptr != pco->name; ++pco) {
                AddColorItem<CopperMiniUI>(this, menu, pco->name, pco->color, current);
            }
        }));
    menu->addChild(createSubmenuItem("Color", "", [=](Menu* menu) {
        auto picker = new ColorPickerMenu();
        picker->set_color(toPacked(getColor()));
        picker->set_on_new_color([=](PackedColor color) {
            setColor(fromPacked(color));
        });
        menu->addChild(picker);
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
}


}
Model *modelCopperMini = createModel<pachde::CopperModule, pachde::CopperMiniUI>("pachde-copper-mini");