#pragma once
#include <rack.hpp>
#include "../components.hpp"
#include "../resizable.hpp"

namespace pachde {

struct LogoPort : PortWidget, ThemeBase
{
    NVGcolor logo;

    LogoPort(Theme theme) { 
        box.size.x = box.size.y = 15.f;
        setTheme(theme);
    }
    void setTheme(Theme theme) override {
        ThemeBase::setTheme(theme);
        logo = LogoColor(theme);
        switch (theme) {
            default: 
            case Theme::Unset:
            case Theme::Light:
                logo.a = 0.5f;
                break;
            case Theme::Dark:
                logo.a = 0.5f;
                break;
            case Theme::HighContrast:
                break;
        }
        if (isColorVisible(panel_color)) {
            auto lum = LuminanceLinear(panel_color);
            if (lum <= 0.5f) {
                logo = Gray(lum + 0.5);
            } else {
                logo = Gray(lum - 0.4);
            }
            if (theme != Theme::HighContrast) {
                logo.a = 0.75;
            }
        }
    }
	void draw(const DrawArgs& args) override
    {
        DrawLogo(args.vg, 0.f, 0.f, logo);
    }
};


struct BlankModule : ResizableModule {
    bool glow = false;
    bool glowing() { return glow && rack::settings::rackBrightness < 0.90f; }
    void setGlow(bool g) { glow = g; }

    BlankModule();
    float getFlicker()
    {
        return inputs[0].isConnected() ? inputs[0].getVoltage(0) : 0.f;
    }
    bool flickering() { return inputs[0].isConnected(); }
    NVGcolor externalcolor();

    json_t* dataToJson() override;
    void dataFromJson(json_t* root) override;
};

struct BlankModuleWidget : ModuleWidget, ITheme
{
    ITheme * alternateTheme = nullptr;
    ThemePanel* panel = nullptr;
    BlankModule* my_module = nullptr;
    ModuleResizeHandle* rightHandle = nullptr;
    ScrewCap* topRightScrew = nullptr;
    ScrewCap* bottomRightScrew = nullptr;
    LogoPort* logo_port = nullptr;
    bool flicker_unipolar = true;

    virtual ~BlankModuleWidget() {
        if (alternateTheme) {
            delete alternateTheme;
        }
    }
    ITheme * getITheme() {
        if (alternateTheme) {
            return alternateTheme;
        }
        auto it = dynamic_cast<ITheme*>(module);
        if (it) {
            return it;
        }
        alternateTheme = new ThemeBase();
        alternateTheme->setScrews(true);
        return alternateTheme;
    }

    bool glowing() {
        if (!my_module) return false;
        return my_module->glowing();
    }

    BlankModuleWidget(BlankModule* module);
    void drawPanel(const DrawArgs &args);

    void step() override;
    void draw(const DrawArgs& args) override;
    void drawLayer(const DrawArgs &args, int layer) override;
    void appendContextMenu(Menu *menu) override;
    void setTheme(Theme theme) override;
    void setPanelColor(NVGcolor color) override;
    void setScrews(bool showScrews) override;
    NVGcolor getPanelColor() override { return getITheme()->getPanelColor(); }
    Theme getTheme() override { return getITheme()->getTheme(); }
    bool hasScrews() override { return getITheme()->hasScrews(); }
    void addResizeHandles();
    void add_screws();
};

}