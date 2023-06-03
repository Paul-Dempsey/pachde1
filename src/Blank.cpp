#include <rack.hpp>
#include "plugin.hpp"
#include "colors.hpp"
#include "components.hpp"
#include "resizable.hpp"
#include "themehelpers.hpp"

using namespace pachde;

struct BlankModuleWidget : ModuleWidget, IChangeTheme
{
    Widget *rightHandle = NULL;
    Widget *topRightScrew = NULL;
    Widget *title = NULL;
    Widget *logo = NULL;
    Widget *bottomRightScrew = NULL;
    pachde::ThemePanel *panel = NULL;

    BlankModuleWidget(ResizableModule *module)
    {
        setModule(module);
        setTheme(ModuleTheme(module));
    }

    void addResizeHandles(ResizableModule *module)
    {
        ModuleResizeHandle *leftHandle = new ModuleResizeHandle;
        leftHandle->module = module;
        addChild(leftHandle);

        ModuleResizeHandle *rightHandle = new ModuleResizeHandle;
        rightHandle->right = true;
        rightHandle->box.pos.x = box.size.x - rightHandle->HandleWidth();
        rightHandle->module = module;
        this->rightHandle = rightHandle;
        addChild(rightHandle);
    }

    void setTheme(Theme theme) override
    {
        auto module = dynamic_cast<ResizableModule *>(this->module);
        // set default size for module browser
        box.size = Vec(RACK_GRID_WIDTH * 4, RACK_GRID_HEIGHT);
        if (children.empty()) {
            panel = new ThemePanel();
            panel->theme = theme;
            panel->box.size = box.size;
            setPanel(panel);

            if (module)
                addResizeHandles(module);

            auto screw = new ScrewCap(theme, ScrewCap::Brightness::More);
            screw->box.pos = Vec(0, 0);
            addChild(screw);

            title = createThemeWidgetCentered<NullWidget>(theme, Vec(box.size.x / 2, 7.5f));
            addChild(title);

            topRightScrew = new ScrewCap(theme, ScrewCap::Brightness::More);
            topRightScrew->box.pos = Vec(box.size.x - RACK_GRID_WIDTH, 0);
            addChild(topRightScrew);

            screw = new ScrewCap(theme, ScrewCap::Brightness::More);
            screw->box.pos = Vec(0, RACK_GRID_HEIGHT - RACK_GRID_WIDTH);
            addChild(screw);

            logo = createThemeWidgetCentered<LogoOverlayWidget>(theme, Vec(box.size.x / 2, RACK_GRID_HEIGHT - RACK_GRID_WIDTH + 7.5f));
            addChild(logo);

            bottomRightScrew = new ScrewCap(theme, ScrewCap::Brightness::More);
            bottomRightScrew->box.pos = Vec(box.size.x - RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH);
            addChild(bottomRightScrew);

        } else {
            panel->theme = theme;
            SetThemeChildren(this, theme);
        }

        if (module)
        {
            box.size.x = module->width * RACK_GRID_WIDTH;
        }
    }

    void step() override
    {
        ResizableModule *module = dynamic_cast<ResizableModule *>(this->module);
        if (module)
        {
            box.size.x = module->width * RACK_GRID_WIDTH;
        }
        panel->box.size = box.size;
        if (3 * RACK_GRID_WIDTH > box.size.x)
        {
            title->box.pos.y = RACK_GRID_WIDTH;
            logo->box.pos.y = RACK_GRID_HEIGHT - 2 * RACK_GRID_WIDTH;
        }
        else
        {
            title->box.pos.y = 0;
            logo->box.pos.y = RACK_GRID_HEIGHT - RACK_GRID_WIDTH;
        }
        topRightScrew->box.pos.x = box.size.x - RACK_GRID_WIDTH;
        title->box.pos.x = box.size.x / 2 - title->box.size.x / 2;
        bottomRightScrew->box.pos.x = box.size.x - RACK_GRID_WIDTH;
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
        themeModule->addThemeMenu(menu, dynamic_cast<IChangeTheme*>(this));
    }
};

Model *modelBlank = createModel<ResizableModule, BlankModuleWidget>("pachde-null");
