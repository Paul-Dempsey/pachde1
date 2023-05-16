#include <rack.hpp>
#include "plugin.hpp"
#include "colors.hpp"
#include "components.hpp"
#include "resizable.hpp"

struct BlankModuleWidget : ModuleWidget, IChangeTheme
{
    Widget *rightHandle = NULL;
    Widget *topRightScrew = NULL;
    Widget *title = NULL;
    Widget *logo = NULL;
    Widget *bottomRightScrew = NULL;
    ThemePanel *panel = NULL;

    BlankModuleWidget(ResizableModule *module)
    {
        setModule(module);
        onChangeTheme();
    }

    void clear()
    {
        panel = NULL;
        setPanel(NULL);
        rightHandle = NULL;
        topRightScrew = NULL;
        title = NULL;
        logo = NULL;
        bottomRightScrew = NULL;
        clearChildren();
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

    void onChangeTheme() override
    {
        auto module = dynamic_cast<ResizableModule *>(this->module);
        auto theme = ModuleTheme(module);
        // set default size for module browser
        box.size = Vec(RACK_GRID_WIDTH * 3, RACK_GRID_HEIGHT);
        clear();
        panel = new ThemePanel();
        panel->theme = theme;
        panel->box.size = box.size;
        setPanel(panel);
        switch (theme)
        {
        case Theme::Dark:
        case Theme::HighContrast:
            if (module)
                addResizeHandles(module);

            addChild(createWidget<ScrewCapDark>(Vec(0, 0)));

            title = createWidgetCentered<NullWidgetDark>(Vec(box.size.x / 2, 7.5f));
            addChild(title);

            topRightScrew = createWidget<ScrewCapDark>(Vec(box.size.x - RACK_GRID_WIDTH, 0));
            addChild(topRightScrew);

            addChild(createWidget<ScrewCapDark>(Vec(0, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

            logo = createWidgetCentered<LogoWidgetBrightOverlay>(Vec(box.size.x / 2, RACK_GRID_HEIGHT - RACK_GRID_WIDTH + 7.5f));
            addChild(logo);

            bottomRightScrew = createWidget<ScrewCapDark>(Vec(box.size.x - RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH));
            addChild(bottomRightScrew);
            break;

        case Theme::Light:
        default:
            if (module)
                addResizeHandles(module);
            addChild(createWidget<ScrewCapMed>(Vec(0, 0)));

            title = createWidgetCentered<NullWidgetBright>(Vec(box.size.x / 2, 7.5f));
            addChild(title);

            topRightScrew = createWidget<ScrewCapMed>(Vec(box.size.x - RACK_GRID_WIDTH, 0));
            addChild(topRightScrew);

            addChild(createWidget<ScrewCapMed>(Vec(0, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

            logo = createWidgetCentered<LogoOverlayWidget>(Vec(box.size.x / 2, RACK_GRID_HEIGHT - RACK_GRID_WIDTH + 7.5f));
            addChild(logo);

            bottomRightScrew = createWidget<ScrewCapMed>(Vec(box.size.x - RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH));
            addChild(bottomRightScrew);
            break;
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

Model *modelBlank = createModel<ResizableModule, BlankModuleWidget>("pachde-blank");
