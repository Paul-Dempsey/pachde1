#include <rack.hpp>
#include "plugin.hpp"
#include "components.hpp"
#include "resizable.hpp"
#include "colors.hpp"
#include "themehelpers.hpp"

using namespace pachde;

enum HpSizes
{
    Default = 5,
    Least = 3,
};

struct InfoModule : ResizableModule
{
    std::string text;

    InfoModule()
    {
        minWidth = 4;
    }

    json_t *dataToJson() override
    {
        json_t *rootJ = ResizableModule::dataToJson();
        json_object_set_new(rootJ, "text", json_stringn(text.c_str(), text.size()));
        return rootJ;
    }

    void dataFromJson(json_t *rootJ) override
    {
        json_t *textJ = json_object_get(rootJ, "text");
        if (textJ)
        {
            text = json_string_value(textJ);
        }
        ResizableModule::dataFromJson(rootJ);
    }
};

struct InfoPanel : Widget, IChangeTheme
{
    ThemeModule* module;
    bool preview = false;

    InfoPanel(ThemeModule* module, Vec size)
    {
        this->module = module;
        if (!module) preview = true;
        box.size = size;
    }

    void draw(const DrawArgs &args) override
    {
        NVGcolor outer, inner;
        switch (ModuleTheme(module))
        {
        default:
        case Theme::Unset:
        case Theme::Light:
            outer = RampGray(G_80);
            inner = RampGray(G_90);
            break;
        case Theme::Dark:
            outer = RampGray(G_20);
            inner = RampGray(G_25);
            break;
        case Theme::HighContrast:
            outer = RampGray(G_BLACK);
            inner = RampGray(G_10);
            break;
        };
        if (module && module->isColorOverride()) {
            outer = module->getColor();
        }
        nvgBeginPath(args.vg);
        nvgRect(args.vg, 0.0, 0.0, box.size.x, box.size.y);
        nvgFillColor(args.vg, outer);
        nvgFill(args.vg);

        nvgBeginPath(args.vg);
        nvgRect(args.vg, 5.0, RACK_GRID_WIDTH, box.size.x - 10.0, box.size.y - RACK_GRID_WIDTH * 2);
        nvgFillColor(args.vg, inner);
        nvgFill(args.vg);

        if (preview) {
            DrawLogo(args.vg, box.size.x / 2.0f - 30.0f, box.size.y / 2.0f - 40, Overlay(COLOR_BRAND), 4.0);
        }
        Widget::draw(args);
    }
};

struct InfoModuleWidget : ModuleWidget, IChangeTheme
{
    ModuleResizeHandle *rightHandle = NULL;
    ScrewCap *topRightScrew = NULL;
    ScrewCap *bottomRightScrew = NULL;
    Widget *title = NULL;
    Widget *logo = NULL;
    InfoPanel *panel = NULL;

    InfoModuleWidget(InfoModule *module)
    {
        setModule(module);
        setTheme(ModuleTheme(module));
    }

    void addResizeHandles()
    {
        auto rmodule = dynamic_cast<ResizableModule *>(module);
        if (!rmodule) return;

        ModuleResizeHandle *leftHandle = new ModuleResizeHandle;
        leftHandle->module = rmodule;
        addChild(leftHandle);

        ModuleResizeHandle *rightHandle = new ModuleResizeHandle;
        rightHandle->right = true;
        this->rightHandle = rightHandle;
        rightHandle->module = rmodule;
        addChild(rightHandle);
    }

    void setScrews(bool showScrews) override {
        auto themeModule = dynamic_cast<ThemeModule*>(this->module);
        if (themeModule) {
            themeModule->setScrews(showScrews);
        }
        if (themeModule->hasScrews()) {
            addScrews(themeModule);
        } else {
            RemoveScrewCaps(this);
        }
    }

    void addScrews(ThemeModule* module) {
        bool have_screws = children.end() != std::find_if(children.begin(), children.end(),
            [](Widget* child) { return nullptr != dynamic_cast<ScrewCap*>(child); } );
        if (have_screws) return;

        bool colored = ModuleColorOverride(module);
        auto theme = ModuleTheme(module);
        
        auto screw = new ScrewCap(theme);
        screw->box.pos = Vec(0, 0);
        if (colored) screw->setColor(module->getColor());
        addChild(screw);

        topRightScrew = new ScrewCap(theme);
        topRightScrew->box.pos = Vec(box.size.x - RACK_GRID_WIDTH, 0);
        if (colored) screw->setColor(module->getColor());
        addChild(topRightScrew);

        screw = new ScrewCap(theme);
        screw->box.pos = Vec(0, RACK_GRID_HEIGHT - RACK_GRID_WIDTH);
        if (colored) screw->setColor(module->getColor());
        addChild(screw);

        bottomRightScrew = new ScrewCap(theme);
        bottomRightScrew->box.pos = Vec(box.size.x - RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH);
        if (colored) screw->setColor(module->getColor());
        addChild(bottomRightScrew);
    }

    void setTheme(Theme theme) override
    {
        auto change = dynamic_cast<IChangeTheme*>(this->module);
        if (change) {
            change->setTheme(theme);
        }
        auto module = dynamic_cast<InfoModule *>(this->module);
        // set default size for browser
        box.size = Vec(RACK_GRID_WIDTH * 8, RACK_GRID_HEIGHT);
        if (children.empty()) {
            auto themeModule = dynamic_cast<ThemeModule*>(this->module);
            panel = new InfoPanel(themeModule, box.size);
            setPanel(panel);
            addResizeHandles();
            addScrews(themeModule);

            title = createThemeWidgetCentered<InfoWidget>(theme, Vec(box.size.x / 2, 7.5f));
            addChild(title);

            logo = createThemeWidgetCentered<LogoOverlayWidget>(theme, Vec(box.size.x / 2, RACK_GRID_HEIGHT - RACK_GRID_WIDTH + 7.5f));
            addChild(logo);

        } else {
            SetChildrenTheme(this, theme);
            if (ModuleColorOverride(module)) {
                SetChildrenThemeColor(this, module->getColor());
            }
        }
        // Set box width from loaded Module before adding to the RackWidget, so modules aren't unnecessarily shoved around.
        if (module)
        {
            box.size.x = module->width * RACK_GRID_WIDTH;
        }
    }

    void step() override
    {
        InfoModule *module = dynamic_cast<InfoModule *>(this->module);
        if (module)
        {
            box.size.x = module->width * RACK_GRID_WIDTH;
        }
        panel->box.size = box.size;
        topRightScrew->box.pos.x = box.size.x - RACK_GRID_WIDTH;
        bottomRightScrew->box.pos.x = box.size.x - RACK_GRID_WIDTH;
        title->box.pos.x = box.size.x / 2 - title->box.size.x / 2;
        logo->box.pos.x = box.size.x / 2 - logo->box.size.x / 2;
        if (rightHandle)
        {
            rightHandle->box.pos.x = box.size.x - rightHandle->box.size.x;
        }
        ModuleWidget::step();
    }

    void appendContextMenu(Menu *menu) override
    {
        if (!this->module)
            return;
        ThemeModule *themeModule = dynamic_cast<ThemeModule *>(this->module);
        themeModule->addThemeMenu(menu, static_cast<IChangeTheme *>(this), true, true);
    }
};

Model *modelInfo = createModel<InfoModule, InfoModuleWidget>("pachde-info");
