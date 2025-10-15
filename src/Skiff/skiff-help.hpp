#pragma once
#include <rack.hpp>
using namespace ::rack;

void original_system_svg(const char * sys_asset);
void replace_system_svg(const char * sys_asset, const char *alt);

bool widget_order_lrtb(const Widget* a, const Widget* b); // for sorting widgets in English reading order (left->right, top->bottom)
void screw_visibility(::rack::widget::Widget* widget, bool visible);
bool toggle_rail(); // returns visibility after toggling
void set_rail_visible(bool visible);
bool is_rail_visible();
void pack_modules();
void zoom_selected();

void add_marker(std::shared_ptr<Svg> svg);
bool is_marked_svg(std::shared_ptr<Svg> svg);

