#include "skiff-help.hpp"

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

void derail()
{
    auto rail = APP->scene->rack->getFirstDescendantOfType<RailWidget>();
    if (rail) rail->setVisible(!rail->isVisible());
}

void packModules()
{
    std::vector<ModuleWidget*> module_widgets = APP->scene->rack->getModules();
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

