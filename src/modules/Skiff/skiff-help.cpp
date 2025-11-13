#include "myplugin.hpp"
#include "skiff-help.hpp"
#include "services/rack-help.hpp"

namespace pachde {

void replace_system_svg(const char * sys_asset, const char *alt) {
    window::Svg::load(asset::system(sys_asset))->loadFile(asset::plugin(pluginInstance, alt));
}

void original_system_svg(const char * sys_asset) {
    auto f = asset::system(sys_asset);
    window::Svg::load(f)->loadFile(f);
}

void screw_visibility(::rack::widget::Widget* root_widget, bool visible) {
    ::rack::widget::Widget* screw = dynamic_cast<SvgScrew*>(root_widget);
    if (screw) {
        screw->setVisible(visible);
        return;
    }
    for (auto child: root_widget->children) {
        screw_visibility(child, visible);
    }
}

void port_visibility(::rack::widget::Widget* root_widget, bool visible) {
    auto port_widget = dynamic_cast<::rack::app::PortWidget*>(root_widget);
    if (port_widget) {
        auto port = port_widget->getPort();
        if (port) {
            if (!port->isConnected()) {
                port_widget->setVisible(visible);
            }
        }
    }
    for (auto child: root_widget->children) {
        port_visibility(child, visible);
    }
}

void panel_visibility(::rack::widget::Widget *exclude, bool visible) {
    ::rack::app::RackWidget* rack = APP->scene->rack;
    std::vector<::rack::app::ModuleWidget*> module_widgets{rack->getModules()};
    if (module_widgets.size() <= 1) return;
    for (auto module_widget: module_widgets) {
        if (module_widget == exclude) continue;
        if (module_widget->children.size())    {
            Widget* first = *module_widget->children.begin();
            first->setVisible(visible);
        }
    }
}

bool toggle_rail() {
    auto rail = APP->scene->rack->getFirstDescendantOfType<RailWidget>();
    if (rail) {
        bool state = !rail->isVisible();
        rail->setVisible(state);
        return state;
    } else {
        return false;
    }
}

bool is_rail_visible() {
    auto rail = APP->scene->rack->getFirstDescendantOfType<RailWidget>();
    return rail ? rail->isVisible() : true;
}

void set_rail_visible(bool visible) {
    auto rail = APP->scene->rack->getFirstDescendantOfType<RailWidget>();
    if (rail && (visible != rail->isVisible())) {
        rail->setVisible(visible);
    }
}

void pack_modules() {
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

void zoom_selected() {
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

NSVGshape* marker_shape() {
    Svg svg;
    svg.loadString("<svg width=\"1\"><circle id=\"%skiff%mark%\" r=\"1\" opacity=\"0\"/></svg>");
    NSVGshape* marker = svg.handle->shapes;
    svg.handle->shapes = nullptr;
    assert(nullptr == marker->next);
    return marker;
}

void add_marker(std::shared_ptr<Svg> svg) {
    if (!svg || !svg->handle || !svg->handle->shapes) return;
    auto marker = marker_shape();
    marker->next = svg->handle->shapes;
    svg->handle->shapes = marker;
}

void add_named_marker(std::shared_ptr<Svg> svg, const std::string& name) {
    if (!svg || !svg->handle || !svg->handle->shapes) return;
    auto marker = marker_shape();
    if (!name.empty()) {
        strncpy(marker->id+12, name.c_str(), 52);
    }
    marker->next = svg->handle->shapes;
    svg->handle->shapes = marker;
}

bool is_marked_svg(std::shared_ptr<Svg> svg) {
    if (!svg || !svg->handle || !svg->handle->shapes) return false;
    return 0 == strncmp(svg->handle->shapes->id, "%skiff%mark%", 12);
}

const char * marker_name(std::shared_ptr<Svg> svg) {
    if (!svg || !svg->handle || !svg->handle->shapes) return "";
    const char * id = &svg->handle->shapes->id[0];
    if (0 == strncmp(id, "%skiff%mark%", 12)) {
        return id + 12;
    }
    return "";
}

std::string get_rack_rail_filename() {
    return asset::system(
        (settings::uiTheme == "light") ? "res/ComponentLibrary/Rail-light.svg" :
        (settings::uiTheme == "hcdark") ? "res/ComponentLibrary/Rail-hcdark.svg" :
        "res/ComponentLibrary/Rail.svg");
}

bool is_alt_rail() {
    auto railSvg = window::Svg::load(get_rack_rail_filename());
    return is_marked_svg(railSvg);
}

const char * alt_rail_name() {
    auto railSvg = window::Svg::load(get_rack_rail_filename());
    auto name = marker_name(railSvg);
    return (*name) ? name :"Rack";
}

}