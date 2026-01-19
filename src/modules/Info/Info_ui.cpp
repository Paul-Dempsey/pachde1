#include "Info.hpp"
#include "info_symbol.hpp"
#include "widgets/anti-panel.hpp"
#include "widgets/create-theme-widget.hpp"
#include "widgets/draw-logo.hpp"
#include "widgets/color-picker.hpp"
#include "widgets/hamburger.hpp"
#include "widgets/logo-widget.hpp"
#include "widgets/screws.hpp"
#include "settings-dialog.hpp"

using namespace widgetry;

namespace pachde {

InfoModuleWidget::InfoModuleWidget(InfoModule* module) {
    my_module = module;
    if (module) {
        box.size = Vec(module->width * RACK_GRID_WIDTH, RACK_GRID_HEIGHT);
        settings = module->getSettings();
        theme_holder = my_module;
    } else {
        box.size = Vec(RACK_GRID_WIDTH * 8, RACK_GRID_HEIGHT);
        settings = new InfoSettings();
        theme_holder = new ThemeBase();
    }
    setModule(module);
    applyThemeSetting(theme_holder->getThemeSetting());
    theme_holder->setNotify(this);
}

bool InfoModuleWidget::editing() {
    return simple_edit && simple_edit->isVisible();
}

void InfoModuleWidget::addResizeHandles()
{
    if (!my_module) return;

    auto handle = new ModuleResizeHandle(my_module);
    addChild(handle);

    handle = new ModuleResizeHandle(my_module);
    handle->right = true;
    addChild(handle);
}

void InfoModuleWidget::onChangeTheme(ChangedItem item) {
    switch (item) {
    case ChangedItem::Theme:
        settings->setTheme(theme_holder->getTheme());
        sendChildrenThemeColor(this, theme_holder->getTheme(), theme_holder->getMainColor());
        break;
    case ChangedItem::MainColor:
        sendChildrenThemeColor(this, theme_holder->getTheme(), theme_holder->getMainColor());
        break;
    case ChangedItem::Screws:
        applyScrews(theme_holder->hasScrews());
        break;
    }
}

void InfoModuleWidget::applyScrews(bool screws) {
    if (screws) {
        addScrews();
    } else {
        RemoveScrewCaps(this);
    }
}

void InfoModuleWidget::addScrews() {
    if (HaveScrewChildren(this)) return;
    AddScrewCaps(this, theme_holder->getTheme(), theme_holder->getMainColor(), ScrewAlign::SCREWS_OUTSIDE, WhichScrew::ALL_SCREWS);
}

void InfoModuleWidget::applyThemeSetting(ThemeSetting setting) {
    theme_holder->setThemeSetting(setting);
    settings->setTheme(theme_holder->getTheme());
    if (children.empty()) {
        panel = new InfoPanel(this, settings, theme_holder, box.size);
        setPanel(panel);
        addChildBottom(new AntiPanel()); // fake out Skiff dePanel
        addResizeHandles();
        if (theme_holder->hasScrews()) {
            addScrews();
        }

        info_symbol = Center(createThemeWidget<InfoSymbol>(theme_holder->getTheme(), Vec(box.size.x*.5f, 7.5f)));
        info_symbol->set_handler([=](){
            if (editing()) {
                simple_edit->close();
            }
            show_settings_dialog(this);
        });
        addChild(info_symbol);

        if (my_module) {
            addChild(simple_edit = new InfoEdit(this));
        }

        logo = new LogoWidget(theme_holder->getTheme(), .18f);
        logo->box.pos = Vec(box.size.x*.5f, RACK_GRID_HEIGHT - RACK_GRID_WIDTH + 7.5f);
        addChild(widgetry::Center(logo));
    } else {
        sendChildrenThemeColor(this, theme_holder->getTheme(), theme_holder->getMainColor());
    }
}

void InfoModuleWidget::onHoverKey(const HoverKeyEvent &e) {
    if (!module) return;

    auto mods = e.mods & RACK_MOD_MASK;
    switch (e.key) {
    case GLFW_KEY_F2: {
        if (e.action == GLFW_PRESS && (0 == mods)) {
            if (editing()) {
                simple_edit->close();
            }
            show_settings_dialog(this);
        }
    } break;

    }
    Base::onHoverKey(e);
}

void InfoModuleWidget::onButton(const ButtonEvent &e) {
    if (!editing()
        && (e.action == GLFW_PRESS)
        && (e.button == GLFW_MOUSE_BUTTON_LEFT)
        && ((e.mods & RACK_MOD_MASK) == 0)
        && (e.pos.y > 100.f) && (e.pos.y < box.size.y - 100.f) // allow for dragging module
        && (e.pos.x > 10.f) && (e.pos.x < box.size.x - 10.f) // exclude resize handles
    ) {
        if (box.size.x > 45.f) {
            simple_edit->begin_editing();
        } else {
            show_settings_dialog(this);
        }
        e.consume(NULL);
        return;
    }
    Base::onButton(e);
}

void InfoModuleWidget::step() {
    bool changed = theme_holder->pollRackThemeChanged();

    if (my_module) {
        box.size.x = my_module->width * RACK_GRID_WIDTH;
        // sync with module for change from presets
        if (!changed && my_module->isDirty()) {
            applyThemeSetting(theme_holder->getThemeSetting());
            applyScrews(theme_holder->hasScrews());
        }
        my_module->setClean();
    }
    bool size_change = (panel->box.size != box.size);
    if (size_change) {
        panel->box.size = box.size;
        info_symbol->box.pos.x = box.size.x*.5f - info_symbol->box.size.x*.5f;
        logo->box.pos.x = box.size.x*.5f - logo->box.size.x*.5f;
        APP->scene->rack->setModulePosForce(this, box.pos);
    }
    info_symbol->setVisible(settings->getBranding());
    logo->setVisible(settings->getBranding());
    ModuleWidget::step();
}

void InfoModuleWidget::drawLayer(const DrawArgs &args, int layer) {
    if (-1 != layer) {
        Base::drawLayer(args, layer);
        return;
    }
    PackedColor co = settings->getPanelColor();
    if (alpha(co) > .99f) Base::drawLayer(args, layer);
}

// ----  Menu  --------------------------------------------------------------

void InfoModuleWidget::appendContextMenu(Menu *menu) {
    if (!my_module) return;

    menu->addChild(createMenuLabel<HamburgerTitle>("#d Info"));

    menu->addChild(createMenuItem("Text options...", "", [=]() {
        show_settings_dialog(this);
    }));

    menu->addChild(new MenuSeparator);

    AddThemeMenu(menu, this, theme_holder, false, true);

    menu->addChild(createCheckMenuItem("Bright text in a dark room", "",
        [=]() { return settings->getBrilliant(); },
        [=]() { settings->setBrilliant(!settings->getBrilliant()); }));

    menu->addChild(createCheckMenuItem(
        "Show branding", "",
        [=]() { return settings->getBranding(); },
        [=]() { settings->setBranding(!settings->getBranding()); }));

    menu->addChild(new MenuSeparator);

    menu->addChild(createMenuItem("Copy info", "", [=]() {
        if (!my_module->text.empty())
        {
            glfwSetClipboardString(APP->window->win, my_module->text.c_str());
        }
    }, my_module->text.empty()));

    menu->addChild(createMenuItem("Paste info", "", [=]() {
        auto text = glfwGetClipboardString(APP->window->win);
        if (text) my_module->text = text;
    }));
}

}