#include "Blank.hpp"
#include "../theme_helpers.hpp"
#include "../Copper/Copper.hpp"

namespace pachde {

// Logo for the "Null" module
void DrawNull(NVGcontext*vg, float x, float y, NVGcolor ring_color, NVGcolor slash_color, float scale = 1.0f) {
    bool scaling = scale != 1.0f;
    if (scaling) {
        nvgSave(vg);
        nvgScale(vg, scale, scale);
    }
    nvgStrokeWidth(vg, 1.0f);

    nvgBeginPath(vg);
    nvgMoveTo(vg, x + 5.5f, y + 13.5f);
    nvgLineTo(vg, x + 9.0f, y + 1.5f);
    nvgStrokeColor(vg, slash_color);
    nvgStroke(vg);

    nvgBeginPath(vg);
    nvgCircle(vg, x + 7.5f, y + 7.5f, 4.5f);
    nvgStrokeColor(vg, ring_color);
    nvgStroke(vg);

    if (scaling) {
        nvgRestore(vg);
    }
}


// ----------------------------------------------------------------------------
BlankModule::BlankModule()
{
    config(0,1,0,0);
    configInput(0,"Flicker");
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

NVGcolor BlankModule::externalcolor()
{
    auto color = expanderColor(getRightExpander());
    if (isColorTransparent(color)) {
        color = expanderColor(getLeftExpander());
    }
    return color;        
} 

json_t* BlankModule::dataToJson() {
    auto root = ResizableModule::dataToJson();
    json_object_set_new(root, "glow", json_boolean(glow));
    return root;
}
void BlankModule::dataFromJson(json_t* root) {
    ResizableModule::dataFromJson(root);
    auto j = json_object_get(root, "glow");
    if (j) {
        glow = json_boolean_value(j);
    }
}

// ----------------------------------------------------------------------------

BlankModuleWidget::BlankModuleWidget(BlankModule *module)
{
    my_module = module;
    setModule(module);
    setTheme(ModuleTheme(module));
}

void BlankModuleWidget::addResizeHandles()
{
    if (!my_module) return;

    ModuleResizeHandle *leftHandle = new ModuleResizeHandle;
    leftHandle->module = my_module;
    addChild(leftHandle);

    ModuleResizeHandle *rightHandle = new ModuleResizeHandle;
    rightHandle->right = true;
    rightHandle->box.pos.x = box.size.x - rightHandle->HandleWidth();
    rightHandle->module = my_module;
    this->rightHandle = rightHandle;
    addChild(rightHandle);
}

void BlankModuleWidget::setScrews(bool screws)
{
    getITheme()->setScrews(screws);
    if (screws) {
        add_screws();
    } else {
        RemoveScrewCaps(this);
        topRightScrew = bottomRightScrew = nullptr;
    }
}

void BlankModuleWidget::add_screws() {

    if (HaveScrewChildren(this)) return;

    auto itheme = getITheme();
    auto theme = itheme->getTheme();
    auto color = itheme->getMainColor();
    bool set_color = isColorVisible(color);

    auto screw = new ScrewCap(theme);
    if (set_color) { screw->color = color; }
    screw->box.pos = Vec(0, 0);
    addChild(screw);

    topRightScrew = new ScrewCap(theme);
    if (set_color) { topRightScrew->color = color; }
    topRightScrew->box.pos = Vec(box.size.x - RACK_GRID_WIDTH, 0);
    addChild(topRightScrew);

    screw = new ScrewCap(theme);
    if (set_color) { screw->color = color; }
    screw->box.pos = Vec(0, RACK_GRID_HEIGHT - RACK_GRID_WIDTH);
    addChild(screw);

    bottomRightScrew = new ScrewCap(theme);
    if (set_color) { bottomRightScrew->color = color; }
    bottomRightScrew->box.pos = Vec(box.size.x - RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH);
    addChild(bottomRightScrew);
}

void BlankModuleWidget::setMainColor(NVGcolor color)
{
    auto itheme = getITheme();
    itheme->setMainColor(color);
    SetChildrenThemeColor(this, color);
}

void BlankModuleWidget::drawPanel(const DrawArgs &args)
{
    auto vg = args.vg;

    auto theme = getITheme()->getTheme();
    NVGcolor logo = LogoColor(theme);
    NVGcolor ring, slash;
    switch (theme) {
        default: 
        case Theme::Unset:
        case Theme::Light:
            slash = RampGray(G_25);
            ring = RampGray(G_20);
            logo.a = 0.5f;
            break;
        case Theme::Dark:
            slash = RampGray(G_45);
            ring = RampGray(G_50);
            logo.a = 0.5f;
            break;
        case Theme::HighContrast:
            slash = ring = WHITE;
            break;
    }

    auto panel_color = my_module ? my_module->externalcolor() : COLOR_NONE;
    if (isColorTransparent(panel_color)) {
        panel_color = getITheme()->getMainColor();
    }
    
    if (isColorVisible(panel_color)) {
        auto lum = LuminanceLinear(panel_color);
        if (lum <= 0.5f) {
            ring = Gray(lum + 0.5);
            slash = Gray(lum + 0.4);
        } else {
            ring = Gray(lum - 0.4);
            slash = Gray(lum - 0.5);
        }
        logo = ring;
        if (theme != Theme::HighContrast) {
            logo.a = 0.75;
        }
    }

    bool flickering = my_module ? my_module->flickering() : false;
    float flicker = 0.f;
    if (flickering) {
        flicker = my_module->getFlicker();
        if (flicker < 0.f) {
            flicker_unipolar = false;
        } else if (flicker > 5.25f) {
            flicker_unipolar = true;
        }
        if (flicker_unipolar) {
            flicker = flicker - 5.f;
        }
        flicker *= .2f;
        clamp(flicker, -5.f, 5.f);
    }

    if (glowing()) {
        if (isColorTransparent(panel_color)) {
            panel_color = PanelBackground(theme);
        }

        const float rad = 30.f;
        float x, y, w, h;
        x = -rad;
        y = -rad;
        w = box.size.x + rad + rad;
        h = box.size.y + rad + rad;
        NVGcolor icol = nvgTransRGBAf(panel_color, rack::settings::haloBrightness);
        icol.a += flicker;
        NVGcolor ocol = COLOR_NONE;

        NVGpaint paint;
        nvgBeginPath(vg);
        nvgRect(vg, x, y, w, h);
        if (flickering) {
            ocol.r += flicker/255.f;
            ocol.g += flicker/255.f;
            ocol.b += flicker/255.f;
        }
        paint = nvgBoxGradient(vg, 0, 0, box.size.x, box.size.y, 4.f, 28.f, icol, ocol);
        nvgFillPaint(vg, paint);
        nvgFill(vg);
    }

    ModuleWidget::draw(args);

    if (flickering) {
        nvgBeginPath(args.vg);
        nvgRect(vg, 0.f, 0.f, box.size.x, box.size.y);
        NVGcolor ocol = nvgRGBAf(0.f, 0.f, 0.f, abs(flicker));
        float innr = std::min(box.size.x, box.size.y) * .5f;
        float outr = std::max(box.size.x, box.size.y);
        NVGpaint paint = nvgRadialGradient(vg, box.size.x/2.f, box.size.y/2.f, innr, outr, panel_color, ocol);
        nvgFillPaint(vg, paint);
        nvgFill(vg);
        //logo_port->draw(args);
    }

    bool skinny = hasScrews() && (3 * RACK_GRID_WIDTH > box.size.x);

    auto y = skinny ? RACK_GRID_WIDTH : 0.0f;
    DrawNull(args.vg, (box.size.x/2.0f) - 7.5f, y, ring, slash);

    if (!module) {
        DrawLogo(args.vg, 0, box.size.y / 2.0f - 40, Overlay(COLOR_BRAND), 4.0);
    } 
}

void BlankModuleWidget::draw(const DrawArgs &args) {
    if (glowing()) return;
    drawPanel(args);
}
void BlankModuleWidget::drawLayer(const DrawArgs &args, int layer) {
    if (glowing()) {
        drawPanel(args);
    }
}

void BlankModuleWidget::setTheme(Theme theme)
{
    auto itheme = getITheme();
    itheme->setTheme(theme);

    // set default size for module browser
    box.size = Vec(RACK_GRID_WIDTH * 4, RACK_GRID_HEIGHT);

    if (children.empty()) {
        panel = new ThemePanel(itheme);
        panel->box.size = box.size;
        setPanel(panel);
        addResizeHandles();
        logo_port = createThemeInputCentered<LogoPort>(theme, Vec(box.size.x/2.f, RACK_GRID_HEIGHT - 7.5f), module, 0);
        addInput(logo_port);
        if (hasScrews()) {
            add_screws();
        }
    } else {
        SetChildrenTheme(this, theme);
        auto themeModule = dynamic_cast<ThemeModule*>(this->module);
        if (ModuleColorOverride(themeModule)) {
            SetChildrenThemeColor(this, itheme->getMainColor());
        }
    }

    auto resize = dynamic_cast<ResizableModule*>(this->module);
    if (resize)
    {
        box.size.x = resize->width * RACK_GRID_WIDTH;
    }
}

void BlankModuleWidget::step()
{
    if (my_module) {
        auto color = my_module->externalcolor();
        if (isColorVisible(color) && panel->theme_holder)
        {
            panel->theme_holder->setMainColor(color);
        }
    }

    auto module = dynamic_cast<ResizableModule *>(this->module);
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
    if (rightHandle)
    {
        rightHandle->box.pos.x = box.size.x - rightHandle->box.size.x;
    }
    if (logo_port) {
        logo_port->box.pos.x = box.size.x/2. - 7.5f;
        bool skinny = hasScrews() && (3 * RACK_GRID_WIDTH > box.size.x);
        logo_port->box.pos.y = skinny ? RACK_GRID_HEIGHT - 2.0f *RACK_GRID_WIDTH : RACK_GRID_HEIGHT - RACK_GRID_WIDTH;
    }
    ModuleWidget::step();
}

void BlankModuleWidget::appendContextMenu(Menu *menu)
{
    auto mymodule = dynamic_cast<BlankModule*>(module);
    if (!module) return;
    AddThemeMenu(menu, this, true, true);
    menu->addChild(createCheckMenuItem(
        "Glow in the dark", "",
        [=]() { return mymodule->glowing(); },
        [=]() { mymodule->setGlow(!mymodule->glowing()); }));
}

}

Model *modelBlank = createModel<pachde::BlankModule, pachde::BlankModuleWidget>("pachde-null");
