#include "components.hpp"
#include "resizable.hpp"

json_t *ResizableModule::dataToJson() 
{
    json_t *rootJ = ThemeModule::dataToJson();

    json_object_set_new(rootJ, "width", json_integer(width));

    return rootJ;
}

void ResizableModule::dataFromJson(json_t *rootJ)
{
    ThemeModule::dataFromJson(rootJ);

    json_t *widthJ = json_object_get(rootJ, "width");
    if (widthJ)
    {
        width = json_integer_value(widthJ);
    }

}

void ResizableModule::setWidth(int newWidth) {
    if (newWidth != width) {
        width = std::max(minWidth, newWidth);
        dirty = true;
    }
}


NVGcolor HandleOverlayForTheme(Theme theme) {
    switch (theme) {
        default:
        case Theme::Unset:
        case Theme::Light:
            return Overlay(COLOR_BRAND);
        case Theme::Dark:
        case Theme::HighContrast:
            return Overlay(COLOR_BRAND_HI);
    }
}

ModuleResizeHandle::ModuleResizeHandle()
{
    box.pos = Vec(0, RACK_GRID_WIDTH);
    box.size = Vec(HandleWidth(), RACK_GRID_HEIGHT - 2 * RACK_GRID_WIDTH);
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
    if (e.button != GLFW_MOUSE_BUTTON_LEFT)
        return;

    dragPos = APP->scene->rack->getMousePos();
    ModuleWidget *mw = getAncestorOfType<ModuleWidget>();
    assert(mw);
    originalBox = mw->box;
}

void ModuleResizeHandle::onDragMove(const DragMoveEvent &e)
{
    assert(module);
    ModuleWidget *moduleWidget = getAncestorOfType<ModuleWidget>();
    assert(moduleWidget);

    Vec newDragPos = APP->scene->rack->getMousePos();
    float deltaX = newDragPos.x - dragPos.x;

    Rect newBox = originalBox;
    Rect oldBox = moduleWidget->box;
    const float minWidth = RACK_GRID_WIDTH * module->minWidth;
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
    module->setWidth(std::round(moduleWidget->box.size.x / RACK_GRID_WIDTH));
}

void ModuleResizeHandle::draw(const DrawArgs &args)
{
    if (!hovered)
        return;
    assert(module);

    auto color = HandleOverlayForTheme(module->theme);

    nvgBeginPath(args.vg);
    nvgRect(args.vg, 0.0, 0.0, box.size.x, box.size.y);
    nvgFillColor(args.vg, color);
    nvgFill(args.vg);
}