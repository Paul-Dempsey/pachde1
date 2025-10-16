#include "Null.hpp"
#include "../IHaveColor.hpp"
#include "../services/json-help.hpp"
#include "../widgets/create-theme-widget.hpp"
#include "../widgets/screws.hpp"

using namespace widgetry;

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

void BlankModule::onReset(const ResetEvent& e) //override
{
    bright = false;
    glow = false;
    branding = true;
    copper = true;
    ResizableModule::onReset(e);
    dirty_settings = true;
}

void BlankModule::onRandomize(const RandomizeEvent& e) //override
{
    ResizableModule::onRandomize(e);

    bright = random::get<bool>();
    glow = random::get<bool>();
    branding = random::get<bool>();
    brand_logo = random::get<bool>();
    copper = random::get<bool>();
    RandomizeTheme(this, false);
}

NVGcolor expanderColor(rack::engine::Module::Expander& expander)
{
    if (expander.module && (expander.module->model == modelCopper || expander.module->model == modelCopperMini)) {
        auto copper = dynamic_cast<IHaveColor*>(expander.module);
        if (copper) {
            return copper->getColor(1);
        }
    }
    return COLOR_NONE;
}

NVGcolor BlankModule::externalcolor()
{
    if (!copper) { return COLOR_NONE; }
    auto color = expanderColor(getRightExpander());
    if (isColorTransparent(color)) {
        color = expanderColor(getLeftExpander());
    }
    return color;
}

json_t* BlankModule::dataToJson()
{
    auto root = ResizableModule::dataToJson();
    set_json(root, "glow", glow);
    set_json(root, "bright", bright);
    set_json(root, "branding", branding);
    set_json(root, "brand-logo", brand_logo);
    set_json(root, "copper", copper);
    return root;

}
void BlankModule::dataFromJson(json_t* root)
{
    ResizableModule::dataFromJson(root);
    glow = get_json_bool(root, "glow", glow);
    bright = get_json_bool(root, "bright", bright);
    branding = get_json_bool(root, "branding", branding);
    brand_logo = get_json_bool(root, "brand-logo", branding);
    copper = get_json_bool(root, "copper", copper);
    dirty_settings = true;
}

// ----------------------------------------------------------------------------

BlankModuleWidget::BlankModuleWidget(BlankModule *module)
{
    my_module = module;
    setModule(module);
    if (my_module) {
        my_module->setNotify(this);
    }
    auto theme = ModuleTheme(module);
    getITheme()->setTheme(theme);
    applyTheme(theme);
}

void BlankModuleWidget::addResizeHandles()
{
    if (!my_module) return;

    auto handle = new ModuleResizeHandle(my_module);
    addChild(handle);

    handle = new ModuleResizeHandle(my_module);
    handle->right = true;
    addChild(handle);
}

void BlankModuleWidget::applyScrews(bool screws)
{
    if (screws) {
        add_screws();
    } else {
        RemoveScrewCaps(this);
    }
}

void BlankModuleWidget::add_screws()
{
    if (HaveScrewChildren(this)) return;
    auto itheme = getITheme();
    AddScrewCaps(this, GetPreferredTheme(itheme), itheme->getMainColor(), ScrewAlign::SCREWS_OUTSIDE, WhichScrew::ALL_SCREWS);
}

void BlankModuleWidget::drawPanel(const DrawArgs &args)
{
    auto vg = args.vg;

    auto theme = GetPreferredTheme(getITheme());
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

    auto panel_color = my_module && my_module->copper ? my_module->externalcolor() : COLOR_NONE;
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

    if (bright() || glowing()) {
        if (isColorTransparent(panel_color)) {
            panel_color = PanelBackground(theme);
        }

        if (glowing()) {
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
        } else {
            nvgBeginPath(vg);
            nvgRect(vg, 0, 0, box.size.x, box.size.y);
            NVGcolor col = panel_color;
            if (flickering) {
                col.r += flicker/255.f;
                col.g += flicker/255.f;
                col.b += flicker/255.f;
            }
            nvgFillColor(vg, col);
            nvgFill(vg);
        }
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
    }

    if (!my_module || my_module->is_branding()) {
        bool skinny = getITheme()->hasScrews() && (3 * RACK_GRID_WIDTH > box.size.x);

        auto y = skinny ? RACK_GRID_WIDTH : 0.0f;
        DrawNull(args.vg, (box.size.x/2.0f) - 7.5f, y, ring, slash);
    }
}

void BlankModuleWidget::draw(const DrawArgs &args) {
    if (bright() || glowing()) return;
    drawPanel(args);
}
void BlankModuleWidget::drawLayer(const DrawArgs &args, int layer) {
    if (bright() || glowing()) {
        drawPanel(args);
    }
}

void BlankModuleWidget::applyTheme(Theme theme)
{
    // set default size for module browser
    box.size = Vec(RACK_GRID_WIDTH * 4, RACK_GRID_HEIGHT);
    auto itheme = getITheme();
    if (children.empty()) {
        panel = new ThemePanel(itheme);
        panel->box.size = box.size;
        setPanel(panel);
        addResizeHandles();
        logo_port = createThemeInputCentered<LogoPort>(theme, Vec(box.size.x/2.f, RACK_GRID_HEIGHT - 7.5f), module, 0);
        addInput(logo_port);
        if (itheme->hasScrews()) {
            add_screws();
        }
    } else {
        SetChildrenTheme(this, theme);
        auto co = itheme->getMainColor();
        if (!isColorTransparent(co)) {
            SetChildrenThemeColor(this, co);
        }
    }

    auto resize = dynamic_cast<const ResizableModule*>(this->module);
    if (resize)
    {
        box.size.x = resize->width * RACK_GRID_WIDTH;
    }
}

void BlankModuleWidget::onChangeTheme(ChangedItem item) // override
{
    auto itheme = getITheme();

    switch (item) {
    case ChangedItem::Theme:
        applyTheme(GetPreferredTheme(itheme));
        break;
    case ChangedItem::FollowDark:
    case ChangedItem::DarkTheme:
        if (itheme->getFollowRack()) {
            applyTheme(GetPreferredTheme(itheme));
        }
        break;
    case ChangedItem::MainColor:
        SetChildrenThemeColor(this, itheme->getMainColor());
        break;
    case ChangedItem::Screws:
        applyScrews(itheme->hasScrews());
        break;
    }
}

void BlankModuleWidget::step()
{
    auto itheme = getITheme();
    bool changed = itheme->pollRackDarkChanged();

    if (my_module) {
        box.size.x = my_module->width * RACK_GRID_WIDTH;
        logo_port->setVisible(branding() && brand_logo());
        auto color = my_module->externalcolor();
        if (isColorVisible(color) && panel->theme_holder)
        {
            panel->theme_holder->setMainColor(color);
        }
        // sync with module for change from presets
        if (!changed && my_module->isDirty()) {
            applyScrews(itheme->hasScrews());
            applyTheme(GetPreferredTheme(itheme));
        }
        my_module->setClean();
    }

    panel->box.size = box.size;

    if (logo_port) {
        logo_port->box.pos.x = box.size.x*.5f - 7.5f;
        bool skinny = getITheme()->hasScrews() && (3 * RACK_GRID_WIDTH > box.size.x);
        logo_port->box.pos.y = skinny ? RACK_GRID_HEIGHT - 2.0f *RACK_GRID_WIDTH : RACK_GRID_HEIGHT - RACK_GRID_WIDTH;
    }
    ModuleWidget::step();
}

void AddColorItem(BlankModuleWidget* self, Menu* menu, const char * name, PackedColor color, PackedColor current) {
    menu->addChild(createColorMenuItem(
        color, name, "",
        [=]() { return current == color; },
        [=]() { self->getITheme()->setMainColor(fromPacked(color)); }
        ));
}

void BlankModuleWidget::appendContextMenu(Menu *menu)
{
    if (!my_module) return;
    AddThemeMenu(menu, getITheme(), true, true);
    menu->addChild(createSubmenuItem("Palette color", "",
        [=](Menu *menu)
        {
            auto current = toPacked(my_module->getMainColor());
            for (auto pco = stock_colors; nullptr != pco->name; ++pco) {
                AddColorItem(this, menu, pco->name, pco->color, current);
            }
        }));
    menu->addChild(createCheckMenuItem(
        "Copper sets panel color", "",
        [=]() { return my_module->copper; },
        [=]() { my_module->copper = !my_module->copper; }));
    menu->addChild(createCheckMenuItem(
        "Show branding", "",
        [=]() { return my_module->is_branding(); },
        [=]() { my_module->set_branding(!my_module->is_branding()); }));
    menu->addChild(createCheckMenuItem(
        "Brand logo", "",
        [=]() { return my_module->is_brand_logo(); },
        [=]() { my_module->set_brand_logo(!my_module->is_brand_logo()); }));
    menu->addChild(createCheckMenuItem(
        "Glow in the dark", "",
        [=]() { return my_module->glow_setting(); },
        [=]() { my_module->setGlow(!my_module->glow_setting()); }));
    menu->addChild(createCheckMenuItem(
        "Bright in a dark room", "",
        [=]() { return my_module->is_bright(); },
        [=]() { my_module->set_bright(!my_module->is_bright()); }));
}

}

Model *modelBlank = createModel<pachde::BlankModule, pachde::BlankModuleWidget>("pachde-null");
