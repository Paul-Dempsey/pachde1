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

void draw_disabled_panel(Widget* panel, Theme theme, const Widget::DrawArgs& args, float top, float bottom)
{
    auto vg = args.vg;
    NVGcolor co1, co2;

    switch (theme) {
        default:
        case Theme::Light: co1 = nvgRGB(0x80, 0x80, 0x80); co2 = nvgRGB(0x40, 0x40, 0x40); break;
        case Theme::Dark: co1 = nvgRGB(0x50, 0x50, 0x50);  co2 = nvgRGB(0x28, 0x28, 0x28); break;
        case Theme::HighContrast: co1 = nvgRGB(0,0,0); co2 = co1; break;
    }
    nvgBeginPath(vg);
    nvgRect(vg, 0.f, top, panel->box.size.x, 380.f - (top + bottom));
    auto gradient = nvgLinearGradient(vg, 0.f, top, panel->box.size.x*.25, 380.f - (top + bottom), co1, co2);
    nvgFillPaint(vg, gradient);
    nvgFill(vg);

    auto red = nvgRGB(0xff, 0x20, 0x20);
    float cx = panel->box.size.x*.5f;
    nvgBeginPath(vg);
    nvgCircle(vg, cx, 118.5f, 10.f);
    nvgStrokeWidth(vg, 2.5f);
    nvgStrokeColor(vg, red);
    nvgStroke(vg);

    nvgBeginPath(vg);
    float left =  cx - 5.f;
    float right =  cx + 5.f;
    nvgMoveTo(vg, left,114.f);
    nvgLineTo(vg, right,124.f);
    nvgMoveTo(vg, right,114.f);
    nvgLineTo(vg, left,124.f);
    nvgStrokeColor(vg, red);
    nvgStroke(vg);
}

}