#pragma once
#include <rack.hpp>
using namespace ::rack;

void original_system_svg(const char * sys_asset);
void replace_system_svg(const char * sys_asset, const char *alt);
void add_marker(std::shared_ptr<Svg> svg);
bool is_marked_svg(std::shared_ptr<Svg> svg);

void screw_visibility(::rack::widget::Widget* root_widget, bool visible);
void port_visibility(::rack::widget::Widget* root_widget, bool visible);
void panel_visibility(::rack::widget::Widget* exclude, bool visible);

bool toggle_rail(); // returns visibility after toggling
void set_rail_visible(bool visible);
bool is_rail_visible();

bool widget_order_lrtb(const Widget* a, const Widget* b); // for sorting widgets in English reading order (left->right, top->bottom)
void pack_modules();
void zoom_selected();

