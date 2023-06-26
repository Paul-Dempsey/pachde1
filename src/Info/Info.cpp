#include <rack.hpp>
#include <osdialog.h>
#include "plugin.hpp"
#include "../components.hpp"
#include "../resizable.hpp"
#include "../text.hpp"
#include "info_symbol.hpp"
#include "../Copper/Copper.hpp" // expander

using namespace pachde;

enum HpSizes { Default = 5, Least = 3, };

constexpr const float DEFAULT_FONT_SIZE = 16.f;
constexpr const float MIN_FONT_SIZE = 5.f;
constexpr const float MAX_FONT_SIZE = 60.f;
// ----------------------------------------------------------------------------

enum HAlign {
    Left, Center, Right
};
inline const char* HAlignName(HAlign h){
    switch (h) {
        default:
        case HAlign::Left: return "Left";
        case HAlign::Center: return "Center";
        case HAlign::Right: return "Right";
    }
}
inline char HAlignLetter(HAlign h){
    switch (h) {
        default:
        case HAlign::Left: return 'l';
        case HAlign::Center: return 'c';
        case HAlign::Right: return 'r';
    }
}
inline NVGalign nvgAlignFromHAlign(HAlign h) {
    switch (h) {
        default:
        case HAlign::Left: return NVGalign::NVG_ALIGN_LEFT;
        case HAlign::Center: return NVGalign::NVG_ALIGN_CENTER;
        case HAlign::Right: return NVGalign::NVG_ALIGN_RIGHT;
    }
}
HAlign parseHAlign(std::string text) {
    if (text.empty()) {
        return HAlign::Left;
    }
    switch (*text.begin()) {
        default:
        case 'l': return HAlign::Left;
        case 'c': return HAlign::Center;
        case 'r': return HAlign::Right;
    }
}

// ----------------------------------------------------------------------------

struct InfoTheme : ThemeBase {
    ITheme * module_theme = nullptr;
    // computed from theme
    NVGcolor theme_panel_color = COLOR_NONE;
    NVGcolor theme_text_background;
    NVGcolor theme_text_color;

    // overrides
    NVGcolor user_text_background = COLOR_NONE;
    NVGcolor user_text_color = COLOR_NONE;
    HAlign horizontal_alignment = HAlign::Left;

    bool brilliant = false;

    float font_size = DEFAULT_FONT_SIZE;
    std::string font_file = asset::plugin(pluginInstance, "res/fonts/HankenGrotesk-SemiBold.ttf");
    std::string font_folder = "";

    virtual ~InfoTheme() {}

    InfoTheme(ITheme *module_theme) : module_theme(module_theme) {
        if (!module_theme) {
            ThemeBase::setScrews(true);
        }
    }
    float getFontSize() { return font_size; }
    void setFontSize(float size) { font_size = clamp(size, MIN_FONT_SIZE, MAX_FONT_SIZE); }
    HAlign getHorizontalAlignment() {
        return horizontal_alignment;
    }
    void setHorizontalAlignment(HAlign h) {
        horizontal_alignment = h;
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
        root = ThemeBase::save(root);

        if (isColorVisible(user_text_background)) {
            auto color_string = rack::color::toHexString(user_text_background);
            json_object_set_new(root, "text-background", json_stringn(color_string.c_str(), color_string.size()));
        }
        if (isColorVisible(user_text_color)) {
            auto color_string = rack::color::toHexString(user_text_color);
            json_object_set_new(root, "text-color", json_stringn(color_string.c_str(), color_string.size()));
        }
        if (DEFAULT_FONT_SIZE != font_size) {
            json_object_set_new(root, "text-size", json_real(font_size));
        }
        std::string align_string = { HAlignLetter(horizontal_alignment) };
        json_object_set_new(root, "text-align", json_stringn(align_string.c_str(), align_string.size()));
        json_object_set_new(root, "font", json_string(font_file.c_str()));
        json_object_set_new(root, "font-folder", json_string(font_folder.c_str()));
        json_object_set_new(root, "bright", json_boolean(brilliant));
        return root;
    }

    void load(json_t* root) override
    {
        ThemeBase::load(root);

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
        j = json_object_get(root, "text-size");
        if (j) {
            font_size = clamp(static_cast<float>(json_real_value(j)), MIN_FONT_SIZE, MAX_FONT_SIZE);
            if (isnanf(font_size)) {
                font_size = DEFAULT_FONT_SIZE;
            }
        }
        j = json_object_get(root, "text-align");
        if (j) {
            horizontal_alignment = parseHAlign(json_string_value(j));
        }
        j = json_object_get(root, "font");
        if (j) {
            font_file = json_string_value(j);
        }
        j = json_object_get(root, "font-folder");
        if (j) {
            font_folder = json_string_value(j);
        }
        j = json_object_get(root, "bright");
        if (j) {
            brilliant = json_boolean_value(j);
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

    void setBrilliant(bool brilliant) { this->brilliant = brilliant; }
    bool getBrilliant() { return brilliant; }
    void toggleBrilliant() { brilliant = !brilliant; }

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

    void resetFont() {
        font_file = asset::plugin(pluginInstance, "res/fonts/HankenGrotesk-SemiBold.ttf");
    }

    bool fontDialog()
    {
        osdialog_filters* filters = osdialog_filters_parse("Fonts (.ttf):ttf;Any (*):*");
        DEFER({osdialog_filters_free(filters);});

        std::string dir = font_folder.empty() ? asset::user("") : font_folder;
        DEBUG("Font Open: %s %s", dir.c_str(), font_file.c_str());

        std::string name = system::getFilename(font_file);
        char* pathC = osdialog_file(OSDIALOG_OPEN, dir.c_str(), name.c_str(), filters);
        if (!pathC) {
            return false;
        }
        std::string path = pathC;
        std::free(pathC);
        DEBUG("Selected font (%s)", path.c_str());
        font_file = path;
        font_folder = system::getDirectory(path);
        return true;
    }
};

// ----------------------------------------------------------------------------

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
        info_theme->load(root);

        json_t *j = json_object_get(root, "text");
        if (j) {
            text = json_string_value(j);
        }
    }

    InfoTheme* getInfoTheme() {
        assert(info_theme);
        return info_theme;
    }

    NVGcolor expanderColor(rack::engine::Module::Expander& expander)
    {
        if (expander.module && expander.module->model == modelCopper) {
            CopperModule* copper = dynamic_cast<CopperModule*>(expander.module);
            if (copper) {
                return copper->getModulatedColor();
            }
        }
        return COLOR_NONE;
    }
    NVGcolor leftExpanderColor() {
        return expanderColor(getLeftExpander());
    }
    NVGcolor rightExpanderColor() {
        return expanderColor(getRightExpander());
    }

};

enum CopperTarget { Panel, Interior };

// ----------------------------------------------------------------------------
struct InfoPanel : Widget
{
    InfoModule* module = nullptr;
    InfoTheme* info_theme = nullptr;
    bool preview = false;
    NVGcolor panel;
    NVGcolor background;
    NVGcolor text_color;

    CopperTarget copper_target = CopperTarget::Panel;

    InfoPanel(InfoModule* module, InfoTheme* info, Vec size)
    {
        this->module = module;
        preview = !module;
        info_theme = info;
        box.size = size;
    }
    CopperTarget getCopperTarget() { return copper_target; }
    void setCopperTarget(CopperTarget target) { copper_target = target; }

    void fetchColors() {
        panel = info_theme->getDisplayPanelColor();
        background = info_theme->getDisplayTextBackground();
        text_color = info_theme->getDisplayTextColor();
        if (module) {
            auto left = module->leftExpanderColor();
            auto right = module->rightExpanderColor();
            if (CopperTarget::Interior == copper_target) {
                if (isColorVisible(left)) {
                    background = left;
                    info_theme->setUserTextBackground(background);
                }
                if (isColorVisible(right)) {
                    text_color = right;
                    info_theme->setUserTextColor(text_color);
                }
            } else {
                bool set_panel = false;
                if (isColorVisible(left)) {
                    set_panel = true;
                    panel = left;
                }
                if (isColorVisible(right)) {
                    set_panel = true;
                    panel = right;
                }
                if (set_panel) {
                    info_theme->setPanelColor(panel);
                }
            }
        }
    }
    
    void step() override {
        Widget::step();
        fetchColors();
    }

    void showText(const DrawArgs &args, std::shared_ptr<rack::window::Font> font, std::string text) {
        nvgScissor(args.vg, RECT_ARGS(args.clipBox));
        auto font_size = info_theme->getFontSize();
        SetTextStyle(args.vg, font, text_color, font_size);
        nvgTextAlign(args.vg, nvgAlignFromHAlign(info_theme->getHorizontalAlignment()));
        nvgTextBox(args.vg, box.pos.x + 10.f, box.pos.y + ONE_HP + font_size, box.size.x - 15.f, text.c_str(), nullptr);
    	nvgResetScissor(args.vg);
    }

    void drawError(const DrawArgs &args) {
        auto r = box.size.x /3.;
        auto color = nvgRGB(250,0,0);
        nvgBeginPath(args.vg);
        nvgCircle(args.vg, box.size.x/2., box.size.y/2., r);
        nvgMoveTo(args.vg, box.size.x/2.-r, box.size.y/2 + r);
        nvgLineTo(args.vg, box.size.x/2 + r, box.size.y/2 - r);
        nvgStrokeColor(args.vg, color);
        nvgStrokeWidth(args.vg, 6.);
        nvgStroke(args.vg);
    }

    void drawText(const DrawArgs &args) {

        nvgBeginPath(args.vg);
        nvgRect(args.vg, 5.0, ONE_HP, box.size.x - 10.f, box.size.y - RACK_GRID_WIDTH * 2.f);
        nvgFillColor(args.vg, background);
        nvgFill(args.vg);

        std::string text = module ? module->text : "";
        if (!text.empty()) {
            auto font = APP->window->loadFont(info_theme->font_file);
            if (FontOk(font)) {
                showText(args, font, text);
            } else {
                info_theme->resetFont();
                font = APP->window->loadFont(info_theme->font_file);
                if (FontOk(font)) {
                    showText(args, font, text);
                } else {
                    drawError(args);
                }
            }
        }

    }

    void drawLayer(const DrawArgs &args, int layer) override
    {
        if (layer == 1 && info_theme->getBrilliant()) {
            drawText(args);
        }
    }
    
    void draw(const DrawArgs &args) override
    {
        assert(info_theme);

        nvgBeginPath(args.vg);
        nvgRect(args.vg, 0.f, 0.f, box.size.x, box.size.y);
        nvgFillColor(args.vg, panel);
        nvgFill(args.vg);

        if (!info_theme->getBrilliant()) {
            drawText(args);
        }

        if (preview) {
            DrawLogo(args.vg, box.size.x / 2.f - 30.0f, box.size.y / 2.f - 40.f, Overlay(COLOR_BRAND), 4.f);
        }
        Widget::draw(args);
    }
};

// ----------------------------------------------------------------------------
struct FontSizeQuantity : Quantity {
    InfoTheme* info_theme;
    FontSizeQuantity(InfoTheme* it) {
        info_theme = it;
    }
	void setValue(float value) override { info_theme->setFontSize(value); }
	float getValue() override { return info_theme->getFontSize(); }
	float getMinValue() override { return MIN_FONT_SIZE; }
	float getMaxValue() override { return MAX_FONT_SIZE; }
	float getDefaultValue() override { return DEFAULT_FONT_SIZE; }
    int getDisplayPrecision() override { return 3; }
	std::string getLabel() override { return "Font size"; }
	std::string getUnit() override { return "px"; }
};

// ----------------------------------------------------------------------------
struct FontSizeSlider : ui::Slider {
	FontSizeSlider(InfoTheme* info_theme) {
		quantity = new FontSizeQuantity(info_theme);
	}
	~FontSizeSlider() {
		delete quantity;
	}
};

// ----------------------------------------------------------------------------
struct InfoModuleWidget : ModuleWidget, ITheme
{
    ModuleResizeHandle* rightHandle = nullptr;
    ScrewCap* topRightScrew = nullptr;
    ScrewCap* bottomRightScrew = nullptr;
    Widget* title = nullptr;
    Widget* logo = nullptr;
    InfoPanel* panel = nullptr;
    InfoTheme* info_theme = nullptr;

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
            bottomRightScrew = topRightScrew = nullptr;
        }
    }

    void addScrews() {
        if (HaveScrewChildren(this)) return;

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

            title = createThemeWidgetCentered<InfoSymbol>(theme, Vec(box.size.x / 2, 7.5f));
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
        if (topRightScrew) {
            topRightScrew->box.pos.x = box.size.x - RACK_GRID_WIDTH;
        }
        if (bottomRightScrew) {
            bottomRightScrew->box.pos.x = box.size.x - RACK_GRID_WIDTH;
        }
        title->box.pos.x = box.size.x / 2 - title->box.size.x / 2;
        logo->box.pos.x = box.size.x / 2 - logo->box.size.x / 2;
        if (rightHandle)
        {
            rightHandle->box.pos.x = box.size.x - rightHandle->box.size.x;
        }
        ModuleWidget::step();
    }

    const char * HEXPLACEHOLDER = "#<hexcolor>";

    void appendContextMenu(Menu *menu) override
    {
        if (!this->module)
            return;
        AddThemeMenu(menu, this, true, true);

        menu->addChild(createCheckMenuItem("Bright text in a dark room", "",
            [=]() { return info_theme->getBrilliant(); },
            [=]() { info_theme->toggleBrilliant(); }));

        menu->addChild(new MenuSeparator);
        menu->addChild(createSubmenuItem("Info", "",
            [=](Menu *menu)
            {
                auto mymodule = dynamic_cast<InfoModule*>(module);
                MenuTextField *editField = new MenuTextField();
                editField->box.size.x = 200.f;
                editField->box.size.y = 100.f;
                editField->setText(mymodule->text);
                editField->commitHandler = [=](std::string text) {
                    mymodule->text = text;
                };
                menu->addChild(editField);
             }));

        auto name = system::getStem(info_theme->font_file);
        menu->addChild(construct<MenuLabel>(&MenuLabel::text, name));
        menu->addChild(createMenuItem("Font...", "", [=]() {
            info_theme->fontDialog();
        }));

		FontSizeSlider* slider = new FontSizeSlider(info_theme);
		slider->box.size.x = 250.0;
		menu->addChild(slider);

        menu->addChild(createSubmenuItem("Text alignment", "",
            [=](Menu *menu)
            {
                menu->addChild(createCheckMenuItem(
                    "Left", "",
                    [=]() { return info_theme->getHorizontalAlignment() == HAlign::Left; },
                    [=]() { info_theme->setHorizontalAlignment(HAlign::Left); }
                    ));
                menu->addChild(createCheckMenuItem(
                    "Center", "",
                    [=]() { return info_theme->getHorizontalAlignment() == HAlign::Center; },
                    [=]() { info_theme->setHorizontalAlignment(HAlign::Center); }
                    ));
                menu->addChild(createCheckMenuItem(
                    "Right", "",
                    [=]() { return info_theme->getHorizontalAlignment() == HAlign::Right; },
                    [=]() { info_theme->setHorizontalAlignment(HAlign::Right); }
                    ));
            }));

        menu->addChild(createSubmenuItem("Text color", "",
            [=](Menu *menu)
            {
                MenuTextField *editField = new MenuTextField();
                editField->box.size.x = 100;
                if (isColorVisible(info_theme->user_text_color)) {
                    editField->setText(rack::color::toHexString(info_theme->user_text_color));
                } else {
                    editField->setText(HEXPLACEHOLDER);
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
                MenuTextField *editField = new MenuTextField();
                editField->box.size.x = 100;
                if (isColorVisible(info_theme->user_text_background)) {
                    editField->setText(rack::color::toHexString(info_theme->user_text_background));
                } else {
                    editField->setText(HEXPLACEHOLDER);
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

        menu->addChild(createSubmenuItem("Copper", "",
            [=](Menu *menu)
            {
                menu->addChild(createCheckMenuItem(
                    "Panel", "",
                    [=]() { return panel->getCopperTarget() == CopperTarget::Panel; },
                    [=]() { panel->setCopperTarget(CopperTarget::Panel); }
                    ));
                menu->addChild(createCheckMenuItem(
                    "Interior (L/R B/T)", "",
                    [=]() { return panel->getCopperTarget() == CopperTarget::Interior; },
                    [=]() { panel->setCopperTarget(CopperTarget::Interior); }
                    ));
            }));

    }
};

Model* modelInfo = createModel<InfoModule, InfoModuleWidget>("pachde-info");
