#pragma once
#include <rack.hpp>
#include "widgets/components.hpp"
#include "widgets/logo-widget.hpp"
#include "resizable.hpp"
#include "text_align.hpp"
#include "info_theme.hpp"

using namespace ::rack;
namespace pachde {

enum CopperTarget { Panel, Text, None, First = Panel, Last = None };

struct InfoModule : ResizableModule
{
    std::string text;
    CopperTarget copper_target = CopperTarget::Panel;
    InfoTheme * info_theme = nullptr;

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
    InfoModule* module{nullptr};;
    InfoTheme* info_theme{nullptr};;
    bool preview{false};
    PackedColor panel{colors::G80};
    PackedColor text_color{colors::G20};

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
    InfoModule* my_module{nullptr};
    Widget* title{nullptr};
    InfoPanel* panel{nullptr};
    InfoTheme* info_theme{nullptr};
    widgetry::LogoWidget* logo{nullptr};

    virtual ~InfoModuleWidget() { if (!module) { delete info_theme; } }

    explicit InfoModuleWidget(InfoModule* module);
    void addResizeHandles();
    void applyScrews(bool screws);
    void addScrews() ;
    void applyThemeSetting(ThemeSetting setting);
    void onChangeTheme(ChangedItem item) override;
    void step() override;
    void appendContextMenu(Menu *menu) override;
};

}
