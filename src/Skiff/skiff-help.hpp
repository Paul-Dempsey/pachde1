#pragma once
#include <rack.hpp>
using namespace ::rack;

bool widget_order_lrtb(const Widget* a, const Widget* b);
void screw_visibility(::rack::widget::Widget* widget, bool visible);
void derail();
void packModules();
