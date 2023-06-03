#include "components.hpp"

namespace pachde {

void ThemeModule::onReset() {
    dirty = true;
}

json_t *ThemeModule::dataToJson()
{
    // derived classes call base (ThemeModule::)dataToJson to get json root
    json_t *rootJ = json_object();

    std::string value = ToString(theme);
    json_object_set_new(rootJ, "theme", json_stringn(value.c_str(), value.size()));

    return rootJ;
}

void ThemeModule::dataFromJson(json_t *rootJ)
{
    // derived classes call base
    theme = ThemeFromJson(rootJ);
    dirty = true;
}

void ThemeModule::addThemeMenu(Menu *menu, IChangeTheme* change) {
    menu->addChild(new MenuSeparator);
    menu->addChild(createSubmenuItem("Theme", "",
        [=](Menu *menu)
        {
            menu->addChild(createCheckMenuItem(
                "Light", "",
                [=]() { return getTheme() == Theme::Light; },
                [=]() {
                    setTheme(Theme::Light);
                    if (change) { change->setTheme(Theme::Light); }
                }));
            menu->addChild(createCheckMenuItem(
                "Dark", "",
                [=]() { return getTheme() == Theme::Dark; },
                [=]() {
                    setTheme(Theme::Dark);
                    if (change) { change->setTheme(Theme::Dark); }
                }));
        }));
}

void ThemePanel::draw(const DrawArgs &args)
{
    nvgBeginPath(args.vg);
    nvgRect(args.vg, 0.0, 0.0, box.size.x, box.size.y);
    nvgFillColor(args.vg, PanelBackground(theme));
    nvgFill(args.vg);

    Widget::draw(args);
}

void SetThemeChildren(Widget * widget, Theme theme, bool top)
{
    for (Widget* child : widget->children) {
        auto change = dynamic_cast<IChangeTheme*>(child);
        if (change) {
            change->setTheme(theme);
        }
        if (!child->children.empty()) {
            SetThemeChildren(child, theme, false);
        }
    }
    if (top) {
        widget::EventContext cDirty;
        widget::Widget::DirtyEvent eDirty;
        eDirty.context = &cDirty;
        widget->onDirty(eDirty);
    }
}

void CreateScrews(ModuleWidget *me, Theme theme, ScrewCap::Brightness bright)
{
    auto screw = new ScrewCap(theme, bright);
    screw->box.pos = Vec(RACK_GRID_WIDTH, 0);
    me->addChild(screw);

    screw = new ScrewCap(theme, bright);
    screw->box.pos = Vec(me->box.size.x - RACK_GRID_WIDTH * 2, 0);
    me->addChild(screw);

    screw = new ScrewCap(theme, bright);
    screw->box.pos = Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH);
    me->addChild(screw);

    screw = new ScrewCap(theme, bright);
    screw->box.pos = Vec(me->box.size.x - RACK_GRID_WIDTH * 2, RACK_GRID_HEIGHT - RACK_GRID_WIDTH);
    me->addChild(screw);
}

} // namespace pachde