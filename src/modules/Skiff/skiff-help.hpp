#pragma once
#include <rack.hpp>

using namespace ::rack;

namespace pachde {

void original_system_svg(const char * sys_asset);
void replace_system_svg(const char * sys_asset, const char *alt);
void add_marker(std::shared_ptr<Svg> svg);
void add_named_marker(std::shared_ptr<Svg> svg, const std::string& name);
bool is_marked_svg(std::shared_ptr<Svg> svg);
const char * marker_name(std::shared_ptr<Svg> svg);

std::string get_rack_rail_filename();
std::shared_ptr<rack::window::Svg> get_rail_svg();
bool is_alt_rail();
const char * alt_rail_name();

void screw_visibility(::rack::widget::Widget* root_widget, bool visible);
void port_visibility(::rack::widget::Widget* root_widget, bool visible);
void light_visibility(::rack::widget::Widget* root_widget, bool visible);
void panel_visibility(::rack::widget::Widget* exclude, bool visible);
void calm_rack(bool calm);

bool toggle_rail(); // returns visibility after toggling
void set_rail_visible(bool visible);
bool is_rail_visible();

void pack_modules();
void zoom_selected();
}