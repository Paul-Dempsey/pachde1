#include <rack.hpp>
#include "plugin.hpp"
#include "colors.hpp"
#include "components.hpp"
#include "resizable.hpp"
#include "themehelpers.hpp"

using namespace pachde;

struct BlankModule : ResizableModule {
    bool show_screws = true;

    json_t * dataToJson() override {
        json_t *root = ResizableModule::dataToJson();
        json_object_set_new(root, "screws", json_boolean(show_screws));
        return root;
    }
    void dataFromJson(json_t *root) override {
        ResizableModule::dataFromJson(root);

        auto j = json_object_get(root, "screws");
        show_screws = j ? json_is_true(j) : true;
    }
};

struct BlankModuleWidget : ModuleWidget, IChangeTheme
{
    Widget *rightHandle = nullptr;
    Widget *title = nullptr;
    Widget *logo = nullptr;
    Widget *topRightScrew = nullptr;
    Widget *bottomRightScrew = nullptr;
    ThemePanel *panel = nullptr;

    BlankModuleWidget(BlankModule *module)
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

    void remove_screws() {
        if (children.empty()) return;

        std::vector<Widget*> remove;
        for (Widget * child: children) {
            auto screw = dynamic_cast<ScrewCap*>(child);
            if (screw) {
                remove.push_back(child);
            }
        }
        for (Widget * child: remove) {
            removeChild(child);
            delete child;
        }
    }

    void add_screws() {
        auto theme = ModuleTheme(dynamic_cast<ThemeModule*>(module));

        auto screw = new ScrewCap(theme, ScrewCap::Brightness::More);
        screw->box.pos = Vec(0, 0);
        addChild(screw);

        topRightScrew = new ScrewCap(theme, ScrewCap::Brightness::More);
        topRightScrew->box.pos = Vec(box.size.x - RACK_GRID_WIDTH, 0);
        addChild(topRightScrew);

        screw = new ScrewCap(theme, ScrewCap::Brightness::More);
        screw->box.pos = Vec(0, RACK_GRID_HEIGHT - RACK_GRID_WIDTH);
        addChild(screw);

        bottomRightScrew = new ScrewCap(theme, ScrewCap::Brightness::More);
        bottomRightScrew->box.pos = Vec(box.size.x - RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH);
        addChild(bottomRightScrew);

    }

    void setTheme(Theme theme) override
    {
        auto module = dynamic_cast<BlankModule *>(this->module);
        // set default size for module browser
        box.size = Vec(RACK_GRID_WIDTH * 4, RACK_GRID_HEIGHT);
        if (children.empty()) {
            panel = new ThemePanel();
            panel->theme = theme;
            panel->box.size = box.size;
            setPanel(panel);

            bool screws = true;

            if (module) {
                addResizeHandles(module);
                screws = module->show_screws;
            }

            if (screws) {
                 add_screws();
            }

            title = createThemeWidgetCentered<NullWidget>(theme, Vec(box.size.x / 2, 7.5f));
            addChild(title);

            logo = createThemeWidgetCentered<LogoOverlayWidget>(theme, Vec(box.size.x / 2, RACK_GRID_HEIGHT - RACK_GRID_WIDTH + 7.5f));
            addChild(logo);
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
        BlankModule *module = dynamic_cast<BlankModule *>(this->module);
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
        if (topRightScrew) {
            topRightScrew->box.pos.x = box.size.x - RACK_GRID_WIDTH;
        }
        title->box.pos.x = box.size.x / 2 - title->box.size.x / 2;
        if (bottomRightScrew) {
            bottomRightScrew->box.pos.x = box.size.x - RACK_GRID_WIDTH;
        }
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
        auto blankModule = dynamic_cast<BlankModule*>(this->module);
        menu->addChild(createCheckMenuItem("Screws", "",
            [=]() { return blankModule->show_screws; }, 
            [=]() {
                blankModule->show_screws = !blankModule->show_screws;
                if (blankModule->show_screws) {
                    add_screws();
                } else {
                    remove_screws();
                }
            }));
    }
};

Model *modelBlank = createModel<BlankModule, BlankModuleWidget>("pachde-null");
