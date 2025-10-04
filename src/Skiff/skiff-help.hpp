#pragma once
#include <rack.hpp>
using namespace ::rack;

bool widget_order_lrtb(const Widget* a, const Widget* b); // for sorting widgets in English reading order (left->right, top->bottom)
void screw_visibility(::rack::widget::Widget* widget, bool visible);
bool toggle_rail(); // returns visibility after toggling
void set_rail_visible(bool visible);
bool is_rail_visible();
void packModules();
void zoom_selected();