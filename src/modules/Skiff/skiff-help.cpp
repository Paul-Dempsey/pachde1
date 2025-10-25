#include "skiff-help.hpp"
#include "myplugin.hpp"

void replace_system_svg(const char * sys_asset, const char *alt)
{
    window::Svg::load(asset::system(sys_asset))->loadFile(asset::plugin(pluginInstance, alt));
}

void original_system_svg(const char * sys_asset)
{
    auto f = asset::system(sys_asset);
    window::Svg::load(f)->loadFile(f);
}

bool widget_order_lrtb(const Widget* a, const Widget* b) {
    if (a->box.pos.y < b->box.pos.y) return true;
    if (a->box.pos.y > b->box.pos.y) return false;
    return a->box.pos.x < b->box.pos.x;
}

void screw_visibility(::rack::widget::Widget* widget, bool visible)
{
    ::rack::widget::Widget* screw = dynamic_cast<SvgScrew*>(widget);
    if (screw) {
        screw->setVisible(visible);
        return;
    }
    for (auto child: widget->children) {
        screw_visibility(child, visible);
    }
}

void port_visibility(::rack::widget::Widget* widget, bool visible)
{
    auto port_widget = dynamic_cast<::rack::app::PortWidget*>(widget);
    if (port_widget) {
        auto port = port_widget->getPort();
        if (port) {
            if (!port->isConnected()) {
                widget->setVisible(visible);
            }
        }
    }
    for (auto child: widget->children) {
        port_visibility(child, visible);
    }
}

bool toggle_rail()
{
    auto rail = APP->scene->rack->getFirstDescendantOfType<RailWidget>();
    if (rail) {
        bool state = !rail->isVisible();
        rail->setVisible(state);
        return state;
    } else {
        return false;
    }
}

bool is_rail_visible()
{
    auto rail = APP->scene->rack->getFirstDescendantOfType<RailWidget>();
    return rail ? rail->isVisible() : false;
}

void set_rail_visible(bool visible) {
    auto rail = APP->scene->rack->getFirstDescendantOfType<RailWidget>();
    if (rail && (visible != rail->isVisible())) {
        rail->setVisible(visible);
    }
}

void pack_modules()
{
    ::rack::app::RackWidget* rack = APP->scene->rack;
    std::vector<::rack::app::ModuleWidget*> module_widgets;
    if (rack->hasSelection()) {
        auto sel = rack->getSelected();
        for (auto m : sel) {
            module_widgets.push_back(m);
        }
    } else {
        module_widgets = rack->getModules();
    }
    if (module_widgets.size() <= 1) return;
    std::sort(module_widgets.begin(), module_widgets.end(), widget_order_lrtb);

    // leftmost x;
    float left = module_widgets[0]->box.pos.x;
    float offset = module_widgets[0]->box.pos.x + module_widgets[0]->box.size.x;
    for (auto w: module_widgets) {
        left = std::min(left, w->box.pos.x);
        offset = std::max(offset, w->box.pos.x + w->box.size.x);
    }
    offset += 15; // mitigate unanticipated extending during enterim moves

    float row_top = module_widgets[0]->box.pos.y;
    Vec pos{left, row_top};

    std::vector<Vec> new_positions;
    for (auto w: module_widgets) {
        if (w->box.pos.y != row_top) {
            row_top = w->box.pos.y;
            pos.x = left;
            pos.y += 380;
        }
        new_positions.push_back(pos);
        pos.x += w->box.size.x;
    }
    new_positions.push_back(pos);

    // move modules out of the space we'll put them back in
    for (auto w: module_widgets) {
        auto newpos = Vec(w->box.pos.x + offset, w->box.pos.y);
        APP->scene->rack->setModulePosNearest(w, newpos);
    }
    // move modules to final position
    auto pit = new_positions.cbegin();
    for (auto w: module_widgets) {
        APP->scene->rack->setModulePosNearest(w, *pit++);
    }
}

void zoom_selected()
{
    auto rack = APP->scene->rack;
    if (!rack->hasSelection()) return;
    auto sel = rack->getSelected();

    math::Vec min = math::Vec(INFINITY, INFINITY);
    math::Vec max = math::Vec(-INFINITY, -INFINITY);
    for (auto mw: sel) {
        if (!mw->isVisible()) continue;
        min = min.min(mw->box.getTopLeft());
        max = max.max(mw->box.getBottomRight());
    }
    APP->scene->rackScroll->zoomToBound(math::Rect::fromMinMax(min, max));
}

NSVGshape* marker_shape()
{
    Svg svg;
    svg.loadString("<svg width=\"1\"><circle id=\"%skiff%marker%\" r=\"1\" opacity=\"0\"/></svg>");
    NSVGshape* marker = svg.handle->shapes;
    svg.handle->shapes = nullptr;
    assert(nullptr == marker->next);
    return marker;
}

void add_marker(std::shared_ptr<Svg> svg)
{
    if (!svg || !svg->handle || !svg->handle->shapes) return;
    auto marker = marker_shape();
    marker->next = svg->handle->shapes;
    svg->handle->shapes = marker;
}

bool is_marked_svg(std::shared_ptr<Svg> svg)
{
    if (!svg || !svg->handle || !svg->handle->shapes) return false;
    const char * id = &svg->handle->shapes->id[0];
    if (!*id) return false;
    return 0 == strncmp(id, "%skiff%marker%", 9);
}

