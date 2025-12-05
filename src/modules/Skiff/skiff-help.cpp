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

void light_visibility(::rack::widget::Widget *root_widget, bool visible)
{
    auto light = dynamic_cast<::rack::app::LightWidget*>(root_widget);
    if (light) {
        light->setVisible(visible);
        return;
    }
    for (auto child: root_widget->children) {
        light_visibility(child, visible);
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

const char *jack_shape_name(JackShape shape)
{
    switch (shape) {
    default:
    case JackShape::Stub:     return "Plain";
    case JackShape::Spades:   return "Spades";
    case JackShape::Diamonds: return "Diamonds";
    case JackShape::Hearts:   return "Hearts";
    case JackShape::Clubs:    return "Clubs";
    case JackShape::Splat:    return "Splat";
    }
}

JackShape parse_jack_shape(const char * text) {
    if (!text || !*text) return JackShape::Stub;
    switch (*text) {
    default:
    case 'C': case 'c': return JackShape::Clubs;
    case 'D': case 'd': return JackShape::Diamonds;
    case 'P': case 'p': return JackShape::Stub;
    case 'H': case 'h': return JackShape::Hearts;
    case 'S': case 's':
        switch (text[1]) {
            default: return JackShape::Stub;
            case 'P': case 'p': {
                switch (text[2]) {
                default: return JackShape::Stub;
                case 'A': case 'a': return JackShape::Spades;
                case 'L': case 'l': return JackShape::Splat;
                }
            } break;
        }
        break;
    }
}

void calm_rack(bool calm, JackShape shape)
{
    if (calm) {
        replace_system_svg("res/ComponentLibrary/RoundBlackKnob_bg.svg", "res/calm/alt-RoundBlackKnob_bg.svg");
        replace_system_svg("res/ComponentLibrary/RoundBlackKnob.svg", "res/calm/alt-RoundBlackKnob.svg");
        replace_system_svg("res/ComponentLibrary/RoundBigBlackKnob_bg.svg", "res/calm/alt-RoundBigBlackKnob_bg.svg");
        replace_system_svg("res/ComponentLibrary/RoundBigBlackKnob.svg", "res/calm/alt-RoundBigBlackKnob.svg");
        replace_system_svg("res/ComponentLibrary/RoundHugeBlackKnob_bg.svg", "res/calm/alt-RoundHugeBlackKnob_bg.svg");
        replace_system_svg("res/ComponentLibrary/RoundHugeBlackKnob.svg", "res/calm/alt-RoundHugeBlackKnob.svg");
        replace_system_svg("res/ComponentLibrary/RoundLargeBlackKnob_bg.svg", "res/calm/alt-RoundLargeBlackKnob_bg.svg");
        replace_system_svg("res/ComponentLibrary/RoundLargeBlackKnob.svg", "res/calm/alt-RoundLargeBlackKnob.svg");
        replace_system_svg("res/ComponentLibrary/RoundSmallBlackKnob_bg.svg", "res/calm/alt-RoundSmallBlackKnob_bg.svg");
        replace_system_svg("res/ComponentLibrary/RoundSmallBlackKnob.svg", "res/calm/alt-RoundSmallBlackKnob.svg");
        replace_system_svg("res/ComponentLibrary/Trimpot_bg.svg", "res/calm/alt-Trimpot_bg.svg");
        replace_system_svg("res/ComponentLibrary/Trimpot.svg", "res/calm/alt-Trimpot.svg");
        // ports
        replace_system_svg("res/ComponentLibrary/CL1362.svg", "res/calm/alt-CL1362.svg");
        replace_system_svg("res/ComponentLibrary/PJ301M-dark.svg", "res/calm/alt-PJ301M-dark.svg");
        replace_system_svg("res/ComponentLibrary/PJ301M.svg", "res/calm/alt-PJ301M.svg");
        replace_system_svg("res/ComponentLibrary/PJ3410.svg", "res/calm/alt-PJ3410.svg");
        // jacks
        switch (shape) {
        default:
        case JackShape::Stub:
            replace_system_svg("res/ComponentLibrary/Plug.svg", "res/calm/alt-Plug.svg");
            break;
        case JackShape::Spades:
            replace_system_svg("res/ComponentLibrary/Plug.svg", "res/calm/alt-Plug-Spades.svg");
            break;
        case JackShape::Diamonds:
            replace_system_svg("res/ComponentLibrary/Plug.svg", "res/calm/alt-Plug-Diamonds.svg");
            break;
        case JackShape::Hearts:
            replace_system_svg("res/ComponentLibrary/Plug.svg", "res/calm/alt-Plug-Hearts.svg");
            break;
        case JackShape::Clubs:
            replace_system_svg("res/ComponentLibrary/Plug.svg", "res/calm/alt-Plug-Clubs.svg");
            break;
        case JackShape::Splat:
            replace_system_svg("res/ComponentLibrary/Plug.svg", "res/calm/alt-Plug-Splat.svg");
            break;
        }
        replace_system_svg("res/ComponentLibrary/PlugPort.svg", "res/calm/alt-PlugPort.svg");
    } else {
        original_system_svg("res/ComponentLibrary/RoundBlackKnob_bg.svg");
        original_system_svg("res/ComponentLibrary/RoundBlackKnob.svg");
        original_system_svg("res/ComponentLibrary/RoundBigBlackKnob_bg.svg");
        original_system_svg("res/ComponentLibrary/RoundBigBlackKnob.svg");
        original_system_svg("res/ComponentLibrary/RoundHugeBlackKnob_bg.svg");
        original_system_svg("res/ComponentLibrary/RoundHugeBlackKnob.svg");
        original_system_svg("res/ComponentLibrary/RoundLargeBlackKnob_bg.svg");
        original_system_svg("res/ComponentLibrary/RoundLargeBlackKnob.svg");
        original_system_svg("res/ComponentLibrary/RoundSmallBlackKnob_bg.svg");
        original_system_svg("res/ComponentLibrary/RoundSmallBlackKnob.svg");
        original_system_svg("res/ComponentLibrary/Trimpot_bg.svg");
        original_system_svg("res/ComponentLibrary/Trimpot.svg");
        // ports
        original_system_svg("res/ComponentLibrary/CL1362.svg");
        original_system_svg("res/ComponentLibrary/PJ301M-dark.svg");
        original_system_svg("res/ComponentLibrary/PJ301M.svg");
        original_system_svg("res/ComponentLibrary/PJ3410.svg");
        // jacks
        original_system_svg("res/ComponentLibrary/Plug.svg");
        original_system_svg("res/ComponentLibrary/PlugPort.svg");
    }
    APP->scene->onDirty(rack::widget::Widget::DirtyEvent{});
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

// ENHANCEMENT: Undo/redo pack
// struct PackAction : ::rack::history::Action {
//     struct ModulePosHistory{ int64_t module_id; Vec oldpos; Vec newpos; };

//     std::vector<ModulePosHistory> positions;

// };

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

std::shared_ptr<rack::window::Svg> get_rail_svg() {
    return window::Svg::load(get_rack_rail_filename());
}

bool is_alt_rail() {
    return is_marked_svg(get_rail_svg());
}

const char * alt_rail_name() {
    auto name = marker_name(get_rail_svg());
    return (*name) ? name : "Rack";
}

}