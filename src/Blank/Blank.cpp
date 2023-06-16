#include "Blank.hpp"
#include "../theme_helpers.hpp"

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
    setModule(module);
    setTheme(ModuleTheme(module));
}

void BlankModuleWidget::addResizeHandles()
{
    auto rmodule = dynamic_cast<ResizableModule*>(module);
    if (!rmodule) return;

    ModuleResizeHandle *leftHandle = new ModuleResizeHandle;
    leftHandle->module = rmodule;
    addChild(leftHandle);

    ModuleResizeHandle *rightHandle = new ModuleResizeHandle;
    rightHandle->right = true;
    rightHandle->box.pos.x = box.size.x - rightHandle->HandleWidth();
    rightHandle->module = rmodule;
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
    auto color = itheme->getPanelColor();
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

void BlankModuleWidget::setPanelColor(NVGcolor color) {
    auto itheme = getITheme();
    itheme->setPanelColor(color);
    //SetScrewColors(this, color);
    SetChildrenThemeColor(this, color);
    // if (panel) {
    //     widget::EventContext cDirty;
    //     widget::Widget::DirtyEvent eDirty;
    //     eDirty.context = &cDirty;
    //     panel->onDirty(eDirty);
    // }
}

void BlankModuleWidget::drawPanel(const DrawArgs &args) {

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
    auto panel_color = getITheme()->getPanelColor();
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

    if (glowing()) {
        auto vg = args.vg;
        const float rad = 60.f;
        float x, y, w, h;
        x = -rad/2.f;
        y = -rad/2.f;
        w = box.size.x + rad; 
        h = box.size.y + rad;
        NVGcolor icol = nvgTransRGBAf(panel_color, rack::settings::rackBrightness);
        NVGcolor ocol = COLOR_NONE;

        NVGpaint paint;
        nvgBeginPath(vg);
        nvgRect(vg, x, y, w, h);
        paint = nvgBoxGradient(vg, 0, 0, box.size.x, box.size.y, 4.f, 28.f, icol, ocol);
        nvgFillPaint(vg, paint);
        nvgFill(vg);

    }

    ModuleWidget::draw(args);

    bool skinny = hasScrews() && (3 * RACK_GRID_WIDTH > box.size.x);

    auto y = skinny ? RACK_GRID_WIDTH : 0.0f;
    DrawNull(args.vg, (box.size.x/2.0f) - 7.5f, y, ring, slash);

    y = skinny ? RACK_GRID_HEIGHT - 2.0f *RACK_GRID_WIDTH : RACK_GRID_HEIGHT - RACK_GRID_WIDTH;
    DrawLogo(args.vg, box.size.x / 2 - 7.5, y, logo);

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
        if (hasScrews()) {
            add_screws();
        }
    } else {
        SetChildrenTheme(this, theme);
        auto themeModule = dynamic_cast<ThemeModule*>(this->module);
        if (ModuleColorOverride(themeModule)) {
            SetChildrenThemeColor(this, itheme->getPanelColor());
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
