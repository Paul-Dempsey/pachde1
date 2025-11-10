#pragma once
#include <rack.hpp>

using namespace ::rack;

namespace pachde {

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

void pack_modules();
void zoom_selected();
}