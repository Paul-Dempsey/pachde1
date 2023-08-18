#pragma once
#ifndef INFO_HPP_INCLUDED
#define INFO_HPP_INCLUDED
#include <rack.hpp>
#include "text_align.hpp"
#include "../components.hpp"
#include "../resizable.hpp"

using namespace ::rack;
namespace pachde {

enum CopperTarget { Panel, Interior, None, First = Panel, Last = None };
constexpr const float DEFAULT_FONT_SIZE = 16.f;
constexpr const float MIN_FONT_SIZE = 5.f;
constexpr const float MAX_FONT_SIZE = 60.f;

struct InfoTheme : ThemeBase
{
    ThemeBase * module_theme = nullptr;
    // computed from theme
    NVGcolor theme_panel_color = RampGray(G_80);
    NVGcolor theme_text_background = RampGray(G_90);
    NVGcolor theme_text_color = RampGray(G_20);
    // overrides
    NVGcolor user_text_background = COLOR_NONE;
    NVGcolor user_text_color = COLOR_NONE;
    HAlign horizontal_alignment = HAlign::Left;

    bool brilliant = false;

    float font_size = DEFAULT_FONT_SIZE;
    std::string font_file = asset::plugin(pluginInstance, "res/fonts/HankenGrotesk-SemiBold.ttf");
    std::string font_folder = "";

    virtual ~InfoTheme() {}

    explicit InfoTheme(ThemeBase *module_theme) : module_theme(module_theme)
    {
        if (!module_theme) {
            ThemeBase::setScrews(true);
        }
    }

    void reset();
    void randomize();

    json_t* save(json_t* root) override;
    void load(json_t* root) override;

    float getFontSize();
    void setFontSize(float size);
    HAlign getHorizontalAlignment();
    void setHorizontalAlignment(HAlign h);
    NVGcolor getDisplayMainColor();
    NVGcolor getDisplayTextBackground();
    NVGcolor getDisplayTextColor();

    Theme getTheme() override;
    Theme getDarkTheme() override;
    void setDarkTheme(Theme theme) override;
    bool getFollowRack() override;
    void setFollowRack(bool follow) override;
    NVGcolor getMainColor() override;
    void setMainColor(NVGcolor color) override;
    bool hasScrews() override;
    void setScrews(bool screws) override;

    void setUserTextBackground(NVGcolor color);
    NVGcolor getUserTextBackground();

    void setUserTextColor(NVGcolor color);
    NVGcolor getUserTextColor();

    void setBrilliant(bool brilliant);
    bool getBrilliant();
    void toggleBrilliant();

    void applyTheme(Theme theme);
    void setTheme(Theme theme) override;

    void resetFont() {
        font_file = asset::plugin(pluginInstance, "res/fonts/HankenGrotesk-SemiBold.ttf");
    }
    bool fontDialog();
};

struct InfoModule : ResizableModule
{
    std::string text;
    CopperTarget copper_target = CopperTarget::Panel;
    InfoTheme * info_theme = nullptr;
    IDirty dirt;

    virtual ~InfoModule()
    {
        if (info_theme) { delete info_theme; }
    }

    InfoModule();

    bool dirty_settings = false;
    bool isDirty() { return dirty_settings; }
    void setClean() { dirty_settings = false; }

    void onReset(const ResetEvent& e) override;
    void onRandomize(const RandomizeEvent& e) override;
    json_t* dataToJson() override;
    void dataFromJson(json_t *root) override;

    InfoTheme* getInfoTheme();
    NVGcolor expanderColor(rack::engine::Module::Expander& expander);

    NVGcolor leftExpanderColor() {
        return expanderColor(getLeftExpander());
    }
    NVGcolor rightExpanderColor() {
        return expanderColor(getRightExpander());
    }
    CopperTarget getCopperTarget() { return copper_target; }
    void setCopperTarget(CopperTarget target) { copper_target = target; }
};

struct InfoPanel : Widget
{
    InfoModule* module = nullptr;
    InfoTheme* info_theme = nullptr;
    bool preview = false;
    NVGcolor panel = RampGray(G_80);
    NVGcolor background = RampGray(G_90);
    NVGcolor text_color = RampGray(G_20);

    InfoPanel(InfoModule* module, InfoTheme* info, Vec size);


    void fetchColors();
    void showText(const DrawArgs &args, std::shared_ptr<rack::window::Font> font, std::string text);
    void drawError(const DrawArgs &args);
    void drawText(const DrawArgs &args);

    void step() override;
    void drawLayer(const DrawArgs &args, int layer) override;
    void draw(const DrawArgs &args) override;
};

struct InfoModuleWidget : ModuleWidget, IThemeChange
{
    InfoModule * my_module = nullptr;
    Widget* title = nullptr;
    Widget* logo = nullptr;
    InfoPanel* panel = nullptr;
    InfoTheme* info_theme = nullptr;

    virtual ~InfoModuleWidget() { if (!module) { delete info_theme; } }

    explicit InfoModuleWidget(InfoModule* module);
    void addResizeHandles();
    void applyScrews(bool screws);
    void addScrews() ;
    void applyTheme(Theme theme);
    void onChangeTheme(ChangedItem item) override;
    void step() override;
    void appendContextMenu(Menu *menu) override;
};

}
#endif