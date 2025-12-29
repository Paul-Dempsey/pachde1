#pragma once
#include <rack.hpp>
using namespace ::rack;
#include "services/colors.hpp"
#include "services/text.hpp"
#include "element-style.hpp"
#include "tip-widget.hpp"

namespace widgetry {

namespace text_button_constants {
    constexpr const char* punch_key = "tbtn-punch";
    constexpr const char* bevel_key = "tbtn-bevel";
    constexpr const char* face_key = "tbtn-face";
    constexpr const char* text_key = "tbtn-text";
    constexpr const char* text_down_key = "tbtn-text-dn";
    constexpr const char* hover_key = "tbtn-hover";
}

enum TextButtonKey { tbtn_Punch, tbtn_Bevel, tbtn_Face, tbtn_Text, tbtn_TextDown, tbtn_Hover, tbtn_Count };

struct TextButton : OpaqueWidget, IThemed {
    using Base = OpaqueWidget;

    bool hovered{false};
    bool key_ctrl{false};
    bool key_shift{false};
    bool sticky{false};
    bool latched{false};
    bool bold{false};
    float text_height{14.f};
    const char * style_key[tbtn_Count];
    std::function<void(bool, bool)> handler{nullptr};
    TipHolder * tip_holder{nullptr};
    std::string text;

    ElementStyle hover{text_button_constants::hover_key, colors::NoColor, colors::G10, 1.35f};
    ElementStyle punch{text_button_constants::punch_key, colors::NoColor, colors::G20, .75f};
    PackedColor co_bevel_hi{colors::G75};
    PackedColor co_bevel_lo{colors::G40};
    PackedColor co_face_hi{colors::G65};
    PackedColor co_face_lo{colors::G45};
    PackedColor co_text{colors::G0};
    PackedColor co_text_down{colors::G40};

    TextButton();
    virtual ~TextButton();
    void set_style_keys(const char * punch_key, const char * bevel_key, const char * face_key, const char * text_key, const char * text_down_key, const char * hover_key);
    void set_text(const std::string& label) { text = label; }
    void set_handler(std::function<void(bool,bool)> callback) { handler = callback; }
    void set_sticky(bool is_sticky) { sticky = is_sticky; }
    void describe(std::string text);
    void set_bold(bool is_bold) { bold = is_bold; }

    // IThemed
    bool applyTheme(std::shared_ptr<SvgTheme> theme) override;

    void onHover(const HoverEvent& e) override { e.consume(this); }
    void destroyTip() { if (tip_holder) { tip_holder->destroyTip(); } }
    void createTip() { if (tip_holder) { tip_holder->createTip(); } }

    void onEnter(const EnterEvent& e) override ;
    void onLeave(const LeaveEvent& e) override;
    void onDragStart(const DragStartEvent& e) override;
    void onDragLeave(const DragLeaveEvent& e) override;
    void onDragEnd(const DragEndEvent& e) override;
    void onHoverKey(const HoverKeyEvent& e) override;
    void onButton(const ButtonEvent& e) override;
    void onAction(const ActionEvent& e) override;

    void appendContextMenu(ui::Menu* menu) {}
    void createContextMenu() {
        ui::Menu* menu = createMenu();
    	appendContextMenu(menu);
    }

    void draw(const DrawArgs& args) override;
};

}