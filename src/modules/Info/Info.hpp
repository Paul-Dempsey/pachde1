#pragma once
#include <rack.hpp>
#include "services/text-align.hpp"
#include "widgets/components.hpp"
#include "widgets/logo-widget.hpp"
#include "resizable.hpp"
#include "info_settings.hpp"
#include "info_symbol.hpp"
#include "info-edit.hpp"

using namespace ::rack;
namespace pachde {

enum CopperTarget { Panel, Text, None, First = Panel, Last = None };
struct InfoModuleWidget;
struct InfoEdit;

struct InfoModule : ResizableModule
{
    using Base = ResizableModule;
    enum Params {
        P_HAlign,
        P_VAlign,
        P_Orientation,
        P_MarginLeft,
        P_MarginRight,
        P_MarginTop,
        P_MarginBottom,
        P_FontSize,
        P_CopperLeft,
        P_CopperRight,
        NUM_PARAMS
    };
    enum Inputs { NUM_INPUTS };
    enum Outputs { NUM_OUTPUTS };
    enum Lights { NUM_LIGHTS };

    std::string text;
    CopperTarget left_copper_target{CopperTarget::Panel};
    CopperTarget right_copper_target{CopperTarget::Text};
    InfoSettings * settings{nullptr};

    virtual ~InfoModule()
    {
        if (settings) { delete settings; }
    }

    InfoModule();

    bool dirty_settings = false;
    bool isDirty() { return dirty_settings; }
    void setClean() { dirty_settings = false; }

    void onReset(const ResetEvent& e) override;
    void onRandomize(const RandomizeEvent& e) override;
    json_t* dataToJson() override;
    void dataFromJson(json_t *root) override;

    InfoSettings* getSettings();
    bool expanderColor(rack::engine::Module::Expander& expander, NVGcolor& result);

    bool leftExpanderColor(NVGcolor& result) {
        return expanderColor(getLeftExpander(), result);
    }
    bool rightExpanderColor(NVGcolor& result) {
        return expanderColor(getRightExpander(), result);
    }
    CopperTarget getLeftCopperTarget() { return left_copper_target; }
    void setLeftCopperTarget(CopperTarget target) { left_copper_target = target; }
    CopperTarget getRightCopperTarget() { return right_copper_target; }
    void setRightCopperTarget(CopperTarget target) { right_copper_target = target; }
    void process(const ProcessArgs& args) override;
};

struct InfoPanel : Widget
{
    using Base = Widget;

    InfoModule* info_module{nullptr};
    InfoModuleWidget* ui{nullptr};
    InfoSettings* settings{nullptr};;
    ThemeBase* theme_holder{nullptr};
    bool preview{false};
    PackedColor panel{colors::G80};
    PackedColor text_color{colors::G20};

    InfoPanel(InfoModuleWidget* info, InfoSettings* settings, ThemeBase* theme, Vec size);

    void fetchColors();
    void showText(const DrawArgs &args, std::shared_ptr<rack::window::Font> font, const std::string& text);
    void drawError(const DrawArgs &args);
    void drawText(const DrawArgs &args);

    void step() override;
    void drawLayer(const DrawArgs &args, int layer) override;
    void draw(const DrawArgs &args) override;
};

struct InfoModuleWidget : ModuleWidget, IThemeChange
{
    using Base = ModuleWidget;

    InfoModule* my_module{nullptr};
    ThemeBase* theme_holder{nullptr};
    InfoSettings* settings{nullptr};

    InfoPanel* panel{nullptr};
    InfoSymbol* info_symbol{nullptr};
    InfoEdit* simple_edit{nullptr};
    widgetry::LogoWidget* logo{nullptr};

    explicit InfoModuleWidget(InfoModule* module);
    virtual ~InfoModuleWidget() {
        if (!module) {
            delete settings;
            delete theme_holder;
        }
    }

    // on-panel editing
    bool editing();
    void addResizeHandles();
    void applyScrews(bool screws);
    void addScrews() ;
    void applyThemeSetting(ThemeSetting setting);
    void onChangeTheme(ChangedItem item) override;
    void onHoverKey(const HoverKeyEvent& e) override;
    void step() override;

    void appendContextMenu(Menu *menu) override;
};

}
