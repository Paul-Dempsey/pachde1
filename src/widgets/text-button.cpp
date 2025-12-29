#include "text-button.hpp"

namespace widgetry {
TextButton::TextButton() {
    using namespace text_button_constants;
    set_style_keys(punch_key, bevel_key, face_key, text_key, text_down_key, hover_key);
    box.size = Vec(18.f, 100.f);
}

TextButton::~TextButton() {
    if (tip_holder) {
        delete tip_holder;
        tip_holder = nullptr;
    }
}

void TextButton::describe(std::string text) {
    if (!tip_holder) {
        tip_holder = new TipHolder();
    }
    tip_holder->setText(text);
}

void TextButton::onEnter(const EnterEvent& e) {
    Base::onEnter(e);
    createTip();
    hovered = true;
}
void TextButton::onLeave(const LeaveEvent& e) {
    destroyTip();
    Base::onLeave(e);
    hovered = false;
}
void TextButton::onDragStart(const DragStartEvent& e) {
    destroyTip();
    if (!sticky) {
        Base::onDragStart(e);
        latched = true;
    }
}
void TextButton::onDragLeave(const DragLeaveEvent& e) {
    destroyTip();
    if (!sticky) {
        Base::onDragLeave(e);
        latched = false;
    }
}
void TextButton::onDragEnd(const DragEndEvent& e) {
    if (!sticky) Base::onDragEnd(e);
    destroyTip();
}

void TextButton::onButton(const ButtonEvent &e) {
	Base::onButton(e);
    if (e.button != GLFW_MOUSE_BUTTON_LEFT) return;

    if (sticky) {
        if (e.action == GLFW_PRESS) {
            ActionEvent eAction;
            onAction(eAction);
        }
    } else {
        if (e.action == GLFW_RELEASE) {
            ActionEvent eAction;
            onAction(eAction);
        }
    }
}

void TextButton::onAction(const ActionEvent& e) {
    destroyTip();
    e.consume(this) ;
    if (handler) {
        handler(key_ctrl, key_shift);
    }
    if (sticky) {
        latched = !latched;
    } else {
        Base::onAction(e);
    }
}

void TextButton::onHoverKey(const HoverKeyEvent& e) {
    Base::onHoverKey(e);
    key_ctrl = (e.mods & RACK_MOD_MASK) & RACK_MOD_CTRL;
    key_shift = (e.mods & RACK_MOD_MASK) & GLFW_MOD_SHIFT;
}

void TextButton::set_style_keys(const char *punch_key, const char *bevel_key, const char *face_key, const char *text_key, const char * text_down_key, const char * hover_key)
{
    style_key[tbtn_Punch] = punch.key = punch_key;
    style_key[tbtn_Bevel] = bevel_key;
    style_key[tbtn_Face] = face_key;
    style_key[tbtn_Text] = text_key;
    style_key[tbtn_TextDown] = text_down_key;
    style_key[tbtn_Hover] = hover.key = hover_key;
}

bool TextButton::applyTheme(std::shared_ptr<SvgTheme> theme) {
    punch.apply_theme(theme);
    hover.apply_theme(theme);

    auto style = theme->getStyle(style_key[tbtn_Bevel]);
    if (style && style->isApplyFill() && style->fill.isGradient()) {
        auto it = style->fill.getGradient()->stops.cbegin();
        co_bevel_hi = it->color; it++;
        co_bevel_lo = it->color;
    } else {
        co_bevel_hi = colors::G75;
        co_bevel_lo = colors::G40;
    }

    style = theme->getStyle(style_key[tbtn_Face]);
    if (style && style->isApplyFill() && style->fill.isGradient()) {
        auto it = style->fill.getGradient()->stops.cbegin();
        co_face_hi = it->color; it++;
        co_face_lo = it->color;
    } else {
        co_face_hi = colors::G65;
        co_face_lo = colors::G45;
    }

    style = theme->getStyle(style_key[tbtn_Text]);
    if (style && style->isApplyFill() && style->fill.isColor()) {
        co_text = style->fill.getColor();
    } else {
        co_text = colors::G0;
    }

    style = theme->getStyle(style_key[tbtn_TextDown]);
    if (style && style->isApplyFill() && style->fill.isColor()) {
        co_text_down = style->fill.getColor();
    } else {
        co_text_down = colors::G40;
    }
    return false;
}

void TextButton::draw(const DrawArgs& args) {
    using namespace pachde;

    auto font = bold ? GetPluginFontSemiBold() : GetPluginFontRegular();
    if (!FontOk(font)) return;

    auto vg = args.vg;
    float stroke_width = punch.width();
    if (packed_color::isVisible(punch.stroke_color)) {
        FittedBoxRect(vg, 0.f, 0.f, box.size.x, box.size.y, punch.nvg_stroke_color(), Fit::Inside, stroke_width);
    }

    NVGcolor co_start{fromPacked(latched ? co_bevel_lo : co_bevel_hi)};
    NVGcolor co_end{fromPacked(latched ? co_bevel_hi : co_bevel_lo)};
    Rect r{Vec{0.f, 0.f}, box.size};
    r = r.shrink(Vec(stroke_width, stroke_width));
    auto g_bevel = nvgLinearGradient(vg, r.pos.x, r.pos.y, r.pos.x, r.pos.y + r.size.y, co_start, co_end);
    nvgFillPaint(vg, g_bevel);
    nvgBeginPath(vg);
    nvgRect(vg, RECT_ARGS(r));
    nvgFill(vg);

    co_start = fromPacked(latched ? co_face_lo : co_face_hi);
    co_end = fromPacked(latched ? co_face_hi : co_face_lo);
    auto g_face = nvgLinearGradient(vg, r.pos.x, r.pos.y, r.pos.x, r.pos.y + r.size.y, co_start, co_end);
    r = r.shrink(Vec(1.f, 1.f));
    nvgBeginPath(vg);
    nvgRect(vg, RECT_ARGS(r));
    nvgFillPaint(vg, g_face);
    nvgFill(vg);

    nvgTextAlign(vg, NVG_ALIGN_TOP|NVG_ALIGN_CENTER);
    nvgFontFaceId(vg, font->handle);
    nvgFontSize(vg, text_height);
    nvgTextLetterSpacing(vg, 0.f);
    nvgFillColor(vg, fromPacked(latched ? co_text_down : co_text));
    nvgText(vg, box.size.x*.5, box.size.y*.5 - text_height*.5, text.c_str(), nullptr);

    if (hovered && hover.stroke_color) {
        BoxRect(vg, 0.f, 0.f, box.size.x, box.size.y, hover.nvg_stroke_color(), hover.width());
        //FittedBoxRect(vg, 0.f, 0.f, box.size.x, box.size.y, hover.nvg_stroke_color(), Fit::Outside, hover.width());
    }
}

}