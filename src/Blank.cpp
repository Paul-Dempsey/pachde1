#include "Blank.hpp"
#include "themehelpers.hpp"

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
void BlankModuleWidget::setScrews(bool showScrews)
{
    auto change = dynamic_cast<IChangeTheme*>(module);
    if (change) change->setScrews(showScrews);
    if (showScrews) {
        add_screws();
    } else {
        RemoveScrewCaps(this);
    }
}

void BlankModuleWidget::add_screws() {
    auto tm = dynamic_cast<ThemeModule*>(module);
    auto theme = ModuleTheme(tm);
    auto color = ModuleColor(tm);
    bool set_color = color.a > 0.f;
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

void BlankModuleWidget::setScrewColors(NVGcolor color) {
    SetScrewColors(this, color);
}

void BlankModuleWidget::setColor(NVGcolor color) {
    auto change = dynamic_cast<IChangeTheme*>(this->module);
    if (change) {
        change->setColor(color);
    }
    if (panel) {
        panel->setColor(color);
        setScrewColors(color);
        widget::EventContext cDirty;
        widget::Widget::DirtyEvent eDirty;
        eDirty.context = &cDirty;
        panel->onDirty(eDirty);
    }
}

void BlankModuleWidget::draw(const DrawArgs &args) {
    ModuleWidget::draw(args);
    auto theme = panel->getTheme();
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
    auto panel_color = panel->getColor();
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

    BlankModule *module = dynamic_cast<BlankModule *>(this->module);
    bool skinny = (module ? module->show_screws : true) 
        && (3 * RACK_GRID_WIDTH > box.size.x);

    auto y = skinny ? RACK_GRID_WIDTH : 0.0f;
    DrawNull(args.vg, (box.size.x/2.0f) - 7.5f, y, ring, slash);

    y = skinny ? RACK_GRID_HEIGHT - 2.0f *RACK_GRID_WIDTH : RACK_GRID_HEIGHT - RACK_GRID_WIDTH;
    DrawLogo(args.vg, box.size.x / 2 - 7.5, y, logo);

    if (!module) {
        DrawLogo(args.vg, 0, box.size.y / 2.0f - 40, Overlay(COLOR_BRAND), 4.0);
    } 
}

void BlankModuleWidget::setTheme(Theme theme)
{
    auto change = dynamic_cast<IChangeTheme*>(this->module);
    if (change) {
        change->setTheme(theme);
    }
    auto themeModule = dynamic_cast<ThemeModule*>(this->module);
    // set default size for module browser
    box.size = Vec(RACK_GRID_WIDTH * 4, RACK_GRID_HEIGHT);
    if (children.empty()) {
        panel = new ThemePanel(themeModule);
        panel->box.size = box.size;
        setPanel(panel);
        addResizeHandles();
        bool screws = ModuleHasScrews(themeModule);
        if (screws) {
            add_screws();
        }
    } else {
        SetChildrenTheme(this, theme);
        if (ModuleColorOverride(themeModule)) {
            SetChildrenThemeColor(this, themeModule->getColor());
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
    BlankModule *module = dynamic_cast<BlankModule *>(this->module);
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
    if (!this->module)
        return;
    ThemeModule *themeModule = dynamic_cast<ThemeModule *>(this->module);
    themeModule->addThemeMenu(menu, dynamic_cast<IChangeTheme*>(this), true, true);
}

}
Model *modelBlank = createModel<pachde::BlankModule, pachde::BlankModuleWidget>("pachde-null");
