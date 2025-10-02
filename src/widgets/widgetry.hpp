#pragma once
#include "../plugin.hpp"
#include "../components.hpp"
using namespace pachde;

namespace widgetry {

// Center any widget
// We use this instead of Rack's style of defining a lot of create<widget>Centered templates
template <class TWidget>
TWidget* Center(TWidget * widget) {
	widget->box.pos = widget->box.pos.minus(widget->box.size.div(2));
	return widget;
}

// dispatch a Dirty event to the widget
void dirtyWidget(Widget* widget);


}