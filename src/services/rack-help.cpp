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

}