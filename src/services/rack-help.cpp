#include "rack-help.hpp"
namespace pachde {

bool is_singleton(Module*my_module, ModuleWidget* me)
{
    if (!my_module) return true;
    auto module_widgets = APP->scene->rack->getModules();
    auto my_model = my_module->getModel();
    if (module_widgets.size() > 1) {
        for (auto module_widget: module_widgets) {
            if ((module_widget != me) && (module_widget->getModel() == my_model)) {
                return false;
            }
        }
    }
    return true;
}

void add_layered_child(Widget* widget, Widget* child, OverlayPosition position)
{
    if (widget->children.size()) {
        switch (position) {
        default:
        case OverlayPosition::OnPanel: {
            Widget* panel = widget->getFirstDescendantOfType<SvgPanel>();
            if (panel) {
                widget->addChildAbove(child, panel);
            } else {
                Widget* border = widget->getFirstDescendantOfType<PanelBorder>();
                if (border) {
                    widget->addChildAbove(child, border->getParent());;
                } else {
                    widget->addChildAbove(child,*widget->children.begin());
                }
            }
        } break;

        case OverlayPosition::OnTop:
            widget->addChild(child);
            break;
        }
    } else {
        widget->addChild(child);
    }
}

bool widget_order_lrtb(const Widget* a, const Widget* b) {
    if (a->box.pos.y < b->box.pos.y) return true;
    if (a->box.pos.y > b->box.pos.y) return false;
    return a->box.pos.x < b->box.pos.x;
}

ModuleWidget* moduleWidgetAtPos(Vec pos) {
    auto container = APP->scene->rack->getModuleContainer();
    for (Widget* w: container->children) {
        if (w->box.contains(pos)) {
            ModuleWidget* mw = dynamic_cast<ModuleWidget*>(w);
            if (mw) {
                return mw;
            } else {
                assert(false);
            }
        }
    }
    return nullptr;
}

std::vector<ModuleWidget*> getModuleWidgets(ModuleWidget* self, AppliesTo which)
{
    switch (which) {
    case AppliesTo::All:
        return APP->scene->rack->getModules();

    case AppliesTo::Selected: {
        ::rack::app::RackWidget* rack = APP->scene->rack;
        std::vector<::rack::app::ModuleWidget*> module_widgets;
        if (rack->hasSelection()) {
            auto sel = rack->getSelected();
            for (auto m : sel) {
                module_widgets.push_back(m);
            }
        }
        return module_widgets;
    } break;

    case AppliesTo::Row:
    case AppliesTo::RowLeft:
    case AppliesTo::ContinuousLeft:
    case AppliesTo::RowRight:
    case AppliesTo::ContinuousRight: {
        auto all = APP->scene->rack->getModules();
        std::sort(all.begin(), all.end(), widget_order_lrtb);
        std::vector<ModuleWidget *> result;
        for (auto mw : all) {
            if (mw->box.pos.y < self->box.pos.y) continue;
            if (mw->box.pos.y > self->box.pos.y) break;

            bool match{false};

            switch (which) {
            default: assert(false); break;
            case AppliesTo::Row:
                match = true;
                break;

            case AppliesTo::RowLeft:
                match = mw->box.pos.x < self->box.pos.x;
                break;

            case AppliesTo::ContinuousLeft: {
                for (auto module = mw->module->getRightExpander().module;
                    !match && (nullptr != module);
                    module = module->getRightExpander().module)
                {
                    match = module == self->module;
                }
                if (!match) goto MODULE_SCAN_COMPETE;
            } break;

            case AppliesTo::RowRight:
                match = mw->box.pos.x > (self->box.pos.x + self->box.size.x);
                break;

            case AppliesTo::ContinuousRight: {
                for (auto module = self->module->getRightExpander().module;
                    !match && (nullptr != module);
                    module = module->getRightExpander().module)
                {
                    match = module == mw->module;
                }
                if (!match) goto MODULE_SCAN_COMPETE;
            } break;
            }
            if (match) {
                result.push_back(mw);
            }
        } MODULE_SCAN_COMPETE:
        return result;
    } break;

    case AppliesTo::Left:
    case AppliesTo::Right: {
        std::vector<ModuleWidget*> result;
        Vec pos = (AppliesTo::Left == which)
            ? self->box.pos.plus(Vec(-7.5f, 7.5f))
            : self->box.pos.plus(Vec(self->box.size.x + 7.5, 7.5f));
        auto mw = moduleWidgetAtPos(pos);
        if (mw) {
            result.push_back(mw);
        }
        return result;
    } break;

    default: assert(false); break;
    }
    return std::vector<ModuleWidget*>{};
}

}