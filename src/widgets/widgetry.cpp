#include "widgetry.hpp"

namespace widgetry {

void dirtyWidget(Widget* widget) {
    if (!widget) return;
    widget::EventContext cDirty;
    widget::Widget::DirtyEvent eDirty;
    eDirty.context = &cDirty;
    widget->onDirty(eDirty);
}

}