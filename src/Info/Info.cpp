#include <rack.hpp>
#include "plugin.hpp"
#include "../colors.hpp"
#include "../components.hpp"
#include "../resizable.hpp"
#include "../text.hpp"

using namespace pachde;

enum HpSizes
{
    Default = 5,
    Least = 3,
};

struct InfoTheme : ThemeBase {
    ITheme * module_theme = nullptr;
    // computed from theme
    NVGcolor theme_panel_color = COLOR_NONE;
    NVGcolor theme_text_background;
    NVGcolor theme_text_color;

    // overrides
    NVGcolor user_text_background = COLOR_NONE;
    NVGcolor user_text_color = COLOR_NONE;;

    virtual ~InfoTheme() {}

    InfoTheme(ITheme *module_theme) : module_theme(module_theme) {
        if (!module_theme) {
            ThemeBase::setScrews(true);
        }
    }

    NVGcolor getDisplayPanelColor() {
        return isColorTransparent(panel_color) ? theme_panel_color : panel_color;
    }
    NVGcolor getDisplayTextBackground() {
        return isColorTransparent(user_text_background) ? theme_text_background : user_text_background;
    }
    NVGcolor getDisplayTextColor() {
        return isColorTransparent(user_text_color) ? theme_text_color : user_text_color;
    }

    json_t* save(json_t* root) override
    {
        if (isColorVisible(user_text_background)) {
            auto color_string = rack::color::toHexString(user_text_background);
            json_object_set_new(root, "text-background", json_stringn(color_string.c_str(), color_string.size()));
        }
        if (isColorVisible(user_text_color)) {
            auto color_string = rack::color::toHexString(user_text_color);
            json_object_set_new(root, "text-color", json_stringn(color_string.c_str(), color_string.size()));
        }
        return root;
    }

    void load(json_t* root) override
    {
        auto j = json_object_get(root, "text-background");
        if (j) {
            auto color_string = json_string_value(j);
            user_text_background = rack::color::fromHexString(color_string);
        }
        j = json_object_get(root, "text-color");
        if (j) {
            auto color_string = json_string_value(j);
            user_text_color = rack::color::fromHexString(color_string);
        }
    }

    Theme getTheme() override { 
        return module_theme ? module_theme->getTheme() : ThemeBase::getTheme();
    }
    NVGcolor getPanelColor() override {
        return module_theme ? module_theme->getPanelColor() : ThemeBase::getPanelColor();
    }
    bool hasScrews() override {
        return module_theme ? module_theme->hasScrews() : ThemeBase::hasScrews();
    }
    void setScrews(bool screws) override {
        module_theme ? module_theme->setScrews(screws) : ThemeBase::setScrews(screws);
    }

    void setUserTextBackground(NVGcolor color) { user_text_background = color; }
    NVGcolor getUserTextBackground() { return user_text_background; }

    void setUserTextColor(NVGcolor color) { user_text_color = color; }
    NVGcolor getUserTextColor() { return user_text_color; }

    void setTheme(Theme theme) override
    {
        if (isColorVisible(theme_panel_color) && theme == getTheme()) {
            return;
        }
        ThemeBase::setTheme(theme);
        if (module_theme) module_theme->setTheme(theme);
        switch (theme) {
            default:
            case Theme::Unset:
            case Theme::Light:
                theme_panel_color = RampGray(G_80);
                theme_text_background = RampGray(G_90);
                theme_text_color = RampGray(G_20);
                break;
            case Theme::Dark:
                theme_panel_color = RampGray(G_25);
                theme_text_background = RampGray(G_20);
                theme_text_color = RampGray(G_85);
                break;
            case Theme::HighContrast:
                theme_panel_color = RampGray(G_10);
                theme_text_background = RampGray(G_BLACK);
                theme_text_color = RampGray(G_WHITE);
                break;
        };
    }
};

struct InfoModule : ResizableModule
{
    std::string text;
    InfoTheme * info_theme = nullptr;

    virtual ~InfoModule() {
        delete info_theme;
    }

    InfoModule()
    {
        info_theme = new InfoTheme(this);
        minWidth = 4;
    }

    json_t* dataToJson() override
    {
        json_t* root = ResizableModule::dataToJson();
        root = info_theme->save(root);
        json_object_set_new(root, "text", json_stringn(text.c_str(), text.size()));
        return root;
    }

    void dataFromJson(json_t *root) override
    {
        ResizableModule::dataFromJson(root);
        json_t *j = json_object_get(root, "text");
        if (j) {
            text = json_string_value(j);
        }
        info_theme->load(root);
    }

    InfoTheme* getInfoTheme() {
        assert(info_theme);
        return info_theme;
    }
};

struct InfoPanel : Widget
{
    InfoModule* module = nullptr;
    InfoTheme* info_theme = nullptr;
    bool preview = false;

    InfoPanel(InfoModule* module, InfoTheme* info, Vec size)
    {
        this->module = module;
        preview = !module;
        info_theme = info;
        box.size = size;
    }

    void draw(const DrawArgs &args) override
    {
        assert(info_theme);
        
        NVGcolor outer = info_theme->getDisplayPanelColor();
        NVGcolor inner = info_theme->getDisplayTextBackground();

        nvgBeginPath(args.vg);
        nvgRect(args.vg, 0.f, 0.f, box.size.x, box.size.y);
        nvgFillColor(args.vg, outer);
        nvgFill(args.vg);

        nvgBeginPath(args.vg);
        nvgRect(args.vg, 5.0, ONE_HP, box.size.x - 10.f, box.size.y - RACK_GRID_WIDTH * 2.f);
        nvgFillColor(args.vg, inner);
        nvgFill(args.vg);

        std::string text = module ? module->text : "";
        if (!text.empty()) {
            auto font = GetPluginFontRegular();
            if (FontOk(font)) {
                SetTextStyle(args.vg, font, info_theme->getDisplayTextColor(), 16.f);
                nvgTextBox(args.vg, box.pos.x + 10.f, box.pos.y + ONE_HP + 20.f, box.size.x - 10.f, text.c_str(), nullptr);
            } else {

            }
        }
        if (preview) {
            DrawLogo(args.vg, box.size.x / 2.f - 30.0f, box.size.y / 2.f - 40.f, Overlay(COLOR_BRAND), 4.f);
        }
        Widget::draw(args);
    }
};


struct InfoModuleWidget : ModuleWidget, ITheme
{
    ModuleResizeHandle *rightHandle = NULL;
    ScrewCap *topRightScrew = NULL;
    ScrewCap *bottomRightScrew = NULL;
    Widget *title = NULL;
    Widget *logo = NULL;
    InfoPanel *panel = NULL;
    InfoTheme * info_theme = nullptr;

    virtual ~InfoModuleWidget() {
        if (!module) { delete info_theme; }
    }

    InfoModuleWidget(InfoModule *module)
    {
        if (module) {
            box.size = Vec(module->width * RACK_GRID_WIDTH, RACK_GRID_HEIGHT);
            info_theme = module->getInfoTheme();
        } else {
            box.size = Vec(RACK_GRID_WIDTH * 8, RACK_GRID_HEIGHT);
            info_theme = new InfoTheme(nullptr);
        }
        setModule(module);
        setTheme(info_theme->getTheme());
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

    Theme getTheme() override { return info_theme->getTheme(); }
    NVGcolor getPanelColor() override { return info_theme->getPanelColor(); }
    bool hasScrews() override { return info_theme->hasScrews(); }

    void setScrews(bool screws) override {
        info_theme->setScrews(screws);
        if (screws) {
            addScrews();
        } else {
            RemoveScrewCaps(this);
        }
    }

    void addScrews() {
        bool have_screws = children.end() != 
            std::find_if(children.begin(), children.end(),
                [](Widget* child) { return nullptr != dynamic_cast<ScrewCap*>(child); } );
        if (have_screws) return;

        bool colored = isColorVisible(info_theme->panel_color);
        auto theme = info_theme->theme;
        
        // left screws
        auto screw = new ScrewCap(theme);
        screw->box.pos = Vec(0, 0);
        if (colored) screw->setPanelColor(info_theme->panel_color);
        addChild(screw);

        screw = new ScrewCap(theme);
        screw->box.pos = Vec(0, RACK_GRID_HEIGHT - RACK_GRID_WIDTH);
        if (colored) screw->setPanelColor(info_theme->panel_color);
        addChild(screw);

        // right screws
        screw = new ScrewCap(theme);
        screw->box.pos = Vec(box.size.x - RACK_GRID_WIDTH, 0);
        if (colored) screw->setPanelColor(info_theme->panel_color);
        addChild(screw);
        topRightScrew = screw;

        screw = new ScrewCap(theme);
        screw->box.pos = Vec(box.size.x - RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH);
        if (colored) screw->setPanelColor(info_theme->panel_color);
        addChild(screw);
        bottomRightScrew = screw;
    }

    void setPanelColor(NVGcolor color) override {
        info_theme->setPanelColor(color);
        SetChildrenThemeColor(this, color);
    }

    void setTheme(Theme theme) override
    {
        info_theme->setTheme(theme);
        
        if (children.empty()) {
            panel = new InfoPanel(dynamic_cast<InfoModule *>(module), info_theme, box.size);
            setPanel(panel);
            addResizeHandles();
            if (hasScrews()) {
                addScrews();
            }

            title = createThemeWidgetCentered<InfoWidget>(theme, Vec(box.size.x / 2, 7.5f));
            addChild(title);

            logo = createThemeWidgetCentered<LogoOverlayWidget>(theme, Vec(box.size.x / 2, RACK_GRID_HEIGHT - RACK_GRID_WIDTH + 7.5f));
            addChild(logo);

        } else {
            SetChildrenTheme(this, theme);
            if (isColorVisible(info_theme->panel_color)) {
                SetChildrenThemeColor(this, info_theme->panel_color);
            }
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
        AddThemeMenu(menu, this, true, true);

        menu->addChild(createSubmenuItem("Text color", "",
            [=](Menu *menu)
            {
                EventParamField *editField = new EventParamField();
                editField->box.size.x = 100;
                if (isColorVisible(info_theme->user_text_color)) {
                    editField->setText(rack::color::toHexString(info_theme->user_text_color));
                } else {
                    editField->setText("[#<hex>]");
                }
                editField->changeHandler = [=](std::string text) {
                    auto color = COLOR_NONE;
                    if (!text.empty() && text[0] == '#') {
                        color = rack::color::fromHexString(text);
                    }
                    info_theme->setUserTextColor(color);
                };
                menu->addChild(editField);
            }));

        menu->addChild(createSubmenuItem("Text background color", "",
            [=](Menu *menu)
            {
                EventParamField *editField = new EventParamField();
                editField->box.size.x = 100;
                if (isColorVisible(info_theme->user_text_background)) {
                    editField->setText(rack::color::toHexString(info_theme->user_text_background));
                } else {
                    editField->setText("[#<hex>]");
                }
                editField->changeHandler = [=](std::string text) {
                    auto color = COLOR_NONE;
                    if (!text.empty() && text[0] == '#') {
                        color = rack::color::fromHexString(text);
                    }
                    info_theme->setUserTextBackground(color);
                };
                menu->addChild(editField);
            }));

        menu->addChild(createSubmenuItem("Info", "",
            [=](Menu *menu)
            {
                auto mymodule = dynamic_cast<InfoModule*>(module);
                EventParamField *editField = new EventParamField();
                editField->box.size.x = 200.f;
                editField->box.size.y = 100.f;
                editField->setText(mymodule->text);
                editField->commitHandler = [=](std::string text) {
                    mymodule->text = text;
                };
                menu->addChild(editField);
             }));
    }
};

Model *modelInfo = createModel<InfoModule, InfoModuleWidget>("pachde-info");
