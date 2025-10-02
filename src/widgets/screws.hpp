#pragma once
#include "widgetry.hpp"
using namespace pachde;

namespace widgetry {

enum ScrewAlign : uint8_t {
    TL_INSET = 0x01,
    TR_INSET = 0x02,
    BL_INSET = 0x04,
    BR_INSET = 0x08,
    UNKNOWN_ALIGNMENT = 0x80,
    SCREWS_OUTSIDE      = 0,
    SCREWS_INSET        = TL_INSET|TR_INSET|BL_INSET|BR_INSET,
    TOP_SCREWS_INSET    = TL_INSET|TR_INSET,
    BOTTOM_SCREWS_INSET = BL_INSET|BR_INSET,
};

enum WhichScrew : uint8_t {
    TL = 0x01,
    TR = 0x02,
    BL = 0x04,
    BR = 0x08,
    UNKNOWN_WHICH = 0x80,
    TOP_SCREWS      = TL|TR,
    BOTTOM_SCREWS   = BL|BR,
    UP_SCREWS       = TR|BL,  // [/]
    DOWN_SCREWS     = TL|BR,  // [\]
    ALL_SCREWS      = TL|TR|BL|BR,
    RIGHT_SCREWS    = TR|BR,
    LEFT_SCREWS     = TL|BL,
};

inline bool isUnknown(WhichScrew which) { return which & WhichScrew::UNKNOWN_WHICH; }
inline bool isApplicable(WhichScrew which_screw, WhichScrew mask) { return mask & which_screw; }
inline float tl_screw_inset(ScrewAlign align) { return ONE_HP * static_cast<bool>(align & ScrewAlign::TL_INSET); }
inline float tr_screw_inset(ScrewAlign align) { return ONE_HP * static_cast<bool>(align & ScrewAlign::TR_INSET); }
inline float bl_screw_inset(ScrewAlign align) { return ONE_HP * static_cast<bool>(align & ScrewAlign::BL_INSET); }
inline float br_screw_inset(ScrewAlign align) { return ONE_HP * static_cast<bool>(align & ScrewAlign::BR_INSET); }
void AddScrewCaps(Widget *widget, Theme theme, NVGcolor color, ScrewAlign positions = ScrewAlign::SCREWS_INSET, WhichScrew which = WhichScrew::ALL_SCREWS);
void RemoveScrewCaps(Widget* widget, WhichScrew which = WhichScrew::ALL_SCREWS);
void SetScrewColors(Widget* widget, NVGcolor color, WhichScrew which = WhichScrew::ALL_SCREWS);
void DrawScrewCap(NVGcontext * vg, float x, float y, Theme theme, NVGcolor color = COLOR_NONE);

struct ScrewCap : rack::TransparentWidget, IBasicTheme
{
    WhichScrew which;
    ScrewAlign align;

    ScrewCap (Theme theme, WhichScrew position, ScrewAlign alignment)
    : which(position), align(alignment)
    {
        setTheme(theme);
        box.size.x = box.size.y = 15.f;
    }

    void draw(const DrawArgs &args) override {
        rack::TransparentWidget::draw(args);
        DrawScrewCap(args.vg, 0, 0, getTheme(), getMainColor());
    }

    void step() override {
        rack::TransparentWidget::step();
        if (align & ScrewAlign::UNKNOWN_ALIGNMENT) return;
        if (parent) {
            switch (which) {
            default: break;
            case WhichScrew::TL:
                box.pos = Vec(tl_screw_inset(align), 0);
                break;
            case WhichScrew::TR:
                box.pos = Vec(parent->box.size.x - ONE_HP - tr_screw_inset(align), 0);
                break;
            case WhichScrew::BL:
                box.pos = Vec(bl_screw_inset(align), RACK_GRID_HEIGHT - ONE_HP);
                break;
            case WhichScrew::BR:
                box.pos = Vec(parent->box.size.x - ONE_HP - br_screw_inset(align), RACK_GRID_HEIGHT - ONE_HP);
                break;
            }
        }
    }
};

WhichScrew GetScrewPosition(const ScrewCap* screw);
WhichScrew SetScrewPosition(ScrewCap* screw, WhichScrew which);

inline bool HaveScrewChildren(Widget* widget)
{
    return widget->children.end() !=
        std::find_if(widget->children.begin(), widget->children.end(),
            [](Widget* child) { return nullptr != dynamic_cast<ScrewCap*>(child); } );
}


}