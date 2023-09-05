#pragma once
#ifndef NULL_HPP_INCLUDED
#define NULL_HPP_INCLUDED
#include <rack.hpp>
#include "../colors.hpp"
#include "../resizable.hpp"
#include "logo_port.hpp"

namespace pachde {

struct BlankModule : ResizableModule
{
    bool bright = false;
    bool glow = false;
    bool branding = true;
    bool copper = true;
    bool brand_logo = false;
    bool dirty_settings = false;

    bool is_bright() { return bright; }
    void set_bright(bool b) { bright = b; }
    bool glowing() { return glow && rack::settings::rackBrightness < 0.90f; }
    bool glow_setting() { return glow; }
    void setGlow(bool g) { glow = g; }

    bool is_branding() { return branding; }
    void set_branding(bool b) { branding = b; }
    bool is_brand_logo() { return brand_logo; }
    void set_brand_logo(bool b) { brand_logo = b; }

    BlankModule();
    void onReset(const ResetEvent& e) override;
    void onRandomize(const RandomizeEvent& e) override;

    bool isDirty() { return dirty_settings; }
    void setClean() { dirty_settings = false; }

    float getFlicker()
    {
        return getInput(0).isConnected() ? getInput(0).getVoltage(0) : 0.f;
    }
    bool flickering() { return getInput(0).isConnected(); }

    NVGcolor externalcolor();

    json_t* dataToJson() override;
    void dataFromJson(json_t* root) override;
};

struct BlankModuleWidget : ModuleWidget, IThemeChange
{
    ThemeBase * alternateTheme = nullptr;
    ThemePanel* panel = nullptr;
    BlankModule* my_module = nullptr;
    LogoPort* logo_port = nullptr;
    bool flicker_unipolar = true;

    virtual ~BlankModuleWidget() {
        if (alternateTheme) {
            delete alternateTheme;
        }
    }
    ThemeBase * getITheme() {
        if (alternateTheme) {
            return alternateTheme;
        }
        auto it = dynamic_cast<ThemeBase*>(module);
        if (it) {
            return it;
        }
        alternateTheme = new ThemeBase();
        alternateTheme->setScrews(true);
        alternateTheme->setNotify(this);
        return alternateTheme;
    }

    bool glowing() {
        return my_module ? my_module->glowing() : false;
    }
    bool bright() {
        return my_module ? my_module->is_bright() : false;
    }
    bool branding() {
        return my_module ? my_module->is_branding() : true;
    }
    bool brand_logo () {
        return my_module ? my_module->is_brand_logo() : true;
    }

    explicit BlankModuleWidget(BlankModule* module);

    void applyTheme(Theme theme);
    void applyScrews(bool screws);
    void drawPanel(const DrawArgs &args);

    void step() override;
    void draw(const DrawArgs& args) override;
    void drawLayer(const DrawArgs &args, int layer) override;

    void appendContextMenu(Menu *menu) override;
    void onChangeTheme(ChangedItem item) override;

    void addResizeHandles();
    void add_screws();
};

}
#endif