#include "components.hpp"
#include "resizable.hpp"

namespace pachde {

json_t* ResizableModule::dataToJson() 
{
    auto root = ThemeModule::dataToJson();
    json_object_set_new(root, "width", json_integer(width));
    return root;
}

void ResizableModule::dataFromJson(json_t *root)
{
    ThemeModule::dataFromJson(root);

    auto j = json_object_get(root, "width");
    if (j) {
        width = json_integer_value(j);
    }
}

void ResizableModule::setWidth(int newWidth) {
    if (newWidth != width) {
        width = std::max(minWidth, newWidth);
    }
}


ModuleResizeHandle::ModuleResizeHandle()
{
    box.pos = Vec(0, ONE_HP);
    box.size = Vec(HandleWidth(), RACK_GRID_HEIGHT - 2 * ONE_HP);
}

NVGcolor ModuleResizeHandle::HandleOverlay()
{
    if (isColorVisible(module->main_color)) {
        bool hi_contrast = Theme::HighContrast == module->theme;
        NVGcolor overlay;
        auto L = LuminanceLinear(module->main_color);
        if (L < 0.5f) {
            overlay = hi_contrast ? RampGray(G_WHITE) : COLOR_BRAND_HI;
        } else {
            overlay = hi_contrast ? RampGray(G_05) : COLOR_BRAND_LO;
        }
        return nvgTransRGBAf(overlay, 0.35f);
    }
    switch (module->theme) {
        default:
        case Theme::Unset:
        case Theme::Light:
            return Overlay(COLOR_BRAND);
        case Theme::Dark:
            return Overlay(COLOR_BRAND_HI);
        case Theme::HighContrast:
            return nvgTransRGBAf(RampGray(G_WHITE), 0.85f);
    }
}

void ModuleResizeHandle::onEnter(const EnterEvent &e)
{
    OpaqueWidget::onEnter(e);
    glfwSetCursor(APP->window->win, glfwCreateStandardCursor(GLFW_RESIZE_EW_CURSOR));
    hovered = true;
}
void ModuleResizeHandle::onLeave(const LeaveEvent &e)
{
    OpaqueWidget::onLeave(e);

    glfwSetCursor(APP->window->win, NULL);
    hovered = false;
}

void ModuleResizeHandle::onDragStart(const DragStartEvent &e)
{
    OpaqueWidget::onDragStart(e);
    if (e.button != GLFW_MOUSE_BUTTON_LEFT)
        return;

    dragPos = APP->scene->rack->getMousePos();
    ModuleWidget *mw = getAncestorOfType<ModuleWidget>();
    assert(mw);
    originalBox = mw->box;
}

void ModuleResizeHandle::onDragMove(const DragMoveEvent &e)
{
    OpaqueWidget::onDragMove(e);

    assert(module);
    ModuleWidget *moduleWidget = getAncestorOfType<ModuleWidget>();
    assert(moduleWidget);

    Vec newDragPos = APP->scene->rack->getMousePos();
    float deltaX = newDragPos.x - dragPos.x;

    Rect newBox = originalBox;
    Rect oldBox = moduleWidget->box;
    const float minWidth = ONE_HP * module->minWidth;
    if (right)
    {
        newBox.size.x = NearestHp(newBox.size.x + deltaX, minWidth);
    }
    else
    {
        newBox.size.x = NearestHp(newBox.size.x - deltaX, minWidth);
        newBox.pos.x = originalBox.pos.x + originalBox.size.x - newBox.size.x;
    }

    // Set box and test whether it's valid
    moduleWidget->box = newBox;
    if (!APP->scene->rack->requestModulePos(moduleWidget, newBox.pos))
    {
        moduleWidget->box = oldBox;
    }
    module->setWidth(std::round(moduleWidget->box.size.x / ONE_HP));

}

void ModuleResizeHandle::draw(const DrawArgs &args)
{
    OpaqueWidget::draw(args);
 
    if (!hovered)
        return;
    assert(module);

    auto color = HandleOverlay();

    nvgBeginPath(args.vg);
    nvgRect(args.vg, 0.0, 0.0, box.size.x, box.size.y);
    nvgFillColor(args.vg, color);
    nvgFill(args.vg);
}

}