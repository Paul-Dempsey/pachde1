#include "widgetry.hpp"

namespace widgetry {

void dirtyWidget(Widget* widget) {
    widget::EventContext cDirty;
    widget::Widget::DirtyEvent eDirty;
    eDirty.context = &cDirty;
    widget->onDirty(eDirty);
}

}