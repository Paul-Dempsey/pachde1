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

struct InfoPanel : Widget
{
    Theme theme;

    InfoPanel(Theme t, Vec size)
    {
        theme = ConcreteTheme(t);
        box.size = size;
    }

    void draw(const DrawArgs &args) override
    {
        NVGcolor outer = GRAY80, inner = GRAY90;
        switch (theme)
        {
        case Theme::Dark:
        case Theme::HighContrast:
            outer = GRAY20;
            inner = GRAY25;
            break;
        case Theme::Light:
        case Theme::Unset:
            outer = GRAY80;
            inner = GRAY90;
            break;
        };

        nvgBeginPath(args.vg);
        nvgRect(args.vg, 0.0, 0.0, box.size.x, box.size.y);
        nvgFillColor(args.vg, outer);
        nvgFill(args.vg);

        nvgBeginPath(args.vg);
        nvgRect(args.vg, 5.0, RACK_GRID_WIDTH, box.size.x - 10.0, box.size.y - RACK_GRID_WIDTH * 2);
        nvgFillColor(args.vg, inner);
        nvgFill(args.vg);

        Widget::draw(args);
    }
};

struct InfoModuleWidget : ModuleWidget, IChangeTheme
{
    Widget *rightHandle = NULL;
    Widget *topRightScrew = NULL;
    Widget *title = NULL;
    Widget *logo = NULL;
    Widget *bottomRightScrew = NULL;
    InfoPanel *panel = NULL;

    InfoModuleWidget(InfoModule *module)
    {
        setModule(module);
        setTheme(ModuleTheme(module));
    }

    void addResizeHandles(InfoModule *module)
    {
        ModuleResizeHandle *leftHandle = new ModuleResizeHandle;
        leftHandle->module = module;
        addChild(leftHandle);

        ModuleResizeHandle *rightHandle = new ModuleResizeHandle;
        rightHandle->right = true;
        this->rightHandle = rightHandle;
        rightHandle->module = module;
        addChild(rightHandle);
    }

    void setTheme(Theme theme) override
    {
        auto module = dynamic_cast<InfoModule *>(this->module);
        // set default size for browser
        box.size = Vec(RACK_GRID_WIDTH * 8, RACK_GRID_HEIGHT);
        if (children.empty()) {
            panel = new InfoPanel(theme, box.size);
            setPanel(panel);
            if (module)
                addResizeHandles(module);

            auto screw = new ScrewCap(theme, ScrewCap::Brightness::Less);
            screw->box.pos = Vec(0, 0);
            addChild(screw);

            title = createThemeWidgetCentered<InfoWidget>(theme, Vec(box.size.x / 2, 7.5f));
            addChild(title);

            topRightScrew = new ScrewCap(theme, ScrewCap::Brightness::Less);
            topRightScrew->box.pos = Vec(box.size.x - RACK_GRID_WIDTH, 0);
            addChild(topRightScrew);

            screw = new ScrewCap(theme, ScrewCap::Brightness::Less);
            screw->box.pos = Vec(0, RACK_GRID_HEIGHT - RACK_GRID_WIDTH);
            addChild(screw);

            logo = createThemeWidgetCentered<LogoOverlayWidget>(theme, Vec(box.size.x / 2, RACK_GRID_HEIGHT - RACK_GRID_WIDTH + 7.5f));
            addChild(logo);

            bottomRightScrew = new ScrewCap(theme, ScrewCap::Brightness::Less);
            bottomRightScrew->box.pos = Vec(box.size.x - RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH);
            addChild(bottomRightScrew);

        } else {
            panel->theme = theme;
            SetThemeChildren(this, theme);
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
        themeModule->addThemeMenu(menu, static_cast<IChangeTheme *>(this));
    }
};

Model *modelInfo = createModel<InfoModule, InfoModuleWidget>("pachde-info");
