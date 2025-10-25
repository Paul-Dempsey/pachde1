#pragma once
#include <rack.hpp>
#include "services/theme-module.hpp"
#include "widgets/components.hpp"

using namespace ::rack;
namespace pachde {

inline float NearestHp(float x, float minHp = 3) {
    return std::round(std::fmax(x, minHp) / RACK_GRID_WIDTH) * RACK_GRID_WIDTH;
}

struct ResizableModule : ThemeModule {
    int width = 5;
    int minWidth = 1;

    void setWidth(int newWidth);

    json_t *dataToJson() override;
    void dataFromJson(json_t *rootJ) override;
};

struct ModuleResizeHandle : OpaqueWidget
{
    bool right = false;
    bool hovered = false;
    Vec dragPos;
    Rect originalBox;
    ResizableModule* module;

    explicit ModuleResizeHandle(ResizableModule* resizable_module);

    float HandleWidth() { return 10.f; }
    NVGcolor HandleOverlay();

    void onEnter(const EnterEvent &e) override;
    void onLeave(const LeaveEvent &e) override;
    void onDragStart(const DragStartEvent &e) override;
    void onDragMove(const DragMoveEvent &e) override;

    void step() override;
    void draw(const DrawArgs &args) override;
};

} // namespace pachde