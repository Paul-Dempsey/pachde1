#pragma once
#include <rack.hpp>
using namespace ::rack;
#include "element-style.hpp"
#include "services/svg-theme.hpp"
#include "services/text.hpp"

namespace widgetry {

struct TextInput : TextField, IThemed
{
    using Base = TextField;
    float text_height = 12.f;

    ElementStyle bg_style{"entry-bg", "hsla(42, 20%, 100%, 5%)", "hsl(42, 50%, 40%)", .25f};
    ElementStyle text_style{"entry-text", "hsl(0, 0%, 65%)"};
    ElementStyle prompt_style{"entry-prompt", "hsla(0, 0%, 55%, 75%)"};
    ElementStyle selection_style{"entry-sel", "hsl(200, 50%, 40%)"};
    std::function<void(std::string)> change_handler{nullptr};
    std::function<void(std::string)> enter_handler{nullptr};
    std::function<void(bool ctrl, bool shift)> tab_handler{nullptr};

    void set_on_change(std::function<void(std::string)> handler) { change_handler = handler; }
    void set_on_enter(std::function<void(std::string)> handler) { enter_handler = handler; }
    void set_on_tab(std::function<void(bool ctrl, bool shift)> handler) { tab_handler = handler; }

    TextInput();
    bool empty() { return text.empty(); }
    bool applyTheme(std::shared_ptr<SvgTheme> theme) override;
    void onSelectKey(const SelectKeyEvent& e) override;
    void onChange(const ChangeEvent& e) override;
    void onAction(const ActionEvent& e) override;
    int getTextPosition(math::Vec mousePos) override;
    void draw(const DrawArgs& args) override;
};

// todo: support multiline=true in TextInput
struct MultiTextInput : TextField
{
    using Base = TextField;
    std::function<void(std::string)> change_handler{nullptr};
    std::function<void(std::string)> enter_handler{nullptr};

    MultiTextInput() { multiline = true; }

    void onChange(const ChangeEvent& e) override {
        if (change_handler) change_handler(text);
        Base::onChange(e);
    }
    void onAction(const ActionEvent& e) override {
        if (enter_handler) enter_handler(text);
        Base::onAction(e);
    }
    void onSelectKey(const SelectKeyEvent& e) override
    {
        if (e.action == GLFW_PRESS || e.action == GLFW_REPEAT) {
            if (e.isKeyCommand(GLFW_KEY_ENTER, RACK_MOD_CTRL) || e.isKeyCommand(GLFW_KEY_KP_ENTER, RACK_MOD_CTRL)) {
				ActionEvent eAction;
				onAction(eAction);
                e.consume(this);
                return;
            }
        }
        Base::onSelectKey(e);
    }
    void set_on_change(std::function<void(std::string)> handler) { change_handler = handler; }
    void set_on_enter(std::function<void(std::string)> handler) { enter_handler = handler; }

};

struct TextInputMenu : TextInput
{
    using Base = TextInput;

    TextInputMenu() {
        text_height = 18.f;
    }

    void setText(const std::string& text) {
        this->text = text;
        selectAll();
    }

    void onSelectKey(const event::SelectKey &e) override {
        if (e.action == GLFW_PRESS && (e.key == GLFW_KEY_ENTER || e.key == GLFW_KEY_KP_ENTER)) {
            if (enter_handler) {
                enter_handler(text);
            }
            ui::MenuOverlay *overlay = getAncestorOfType<ui::MenuOverlay>();
            overlay->requestDelete();
            e.consume(this);
        }
        if (!e.getTarget())
            Base::onSelectKey(e);
    }

    void onChange(const ChangeEvent& e) override {
        Base::onChange(e);
        if (change_handler) {
            change_handler(text);
        }
    }

    void step() override {
        // Keep selected
        APP->event->setSelectedWidget(this);
        Base::step();
    }
};

template <typename Tin = TextInput>
Tin* createThemedTextInput(
    float x, float y, float width, float height,
    std::shared_ptr<SvgTheme> theme,
    std::string content,
    std::function<void(std::string)> on_change = nullptr,
    std::function<void(std::string)> on_enter = nullptr,
    const char* placeholder = nullptr
    )
{
    Tin* t = createWidget<Tin>(Vec(x, y));
    if (width <= 0) width = 120.f;
    if (height <= 0) height = 14.f;
    t->box.size = Vec(width, height);
    t->applyTheme(theme);
    t->setText(content);
    if (on_change) t->set_on_change(on_change);
    if (on_enter) t->set_on_enter(on_enter);
    if (placeholder) t->placeholder = placeholder;
    return t;
}

template <typename Tin = TextInput>
Tin* createTextInput(
    float x, float y, float width, float height,
    std::string content,
    std::function<void(std::string)> on_change = nullptr,
    std::function<void(std::string)> on_enter = nullptr,
    const char* placeholder = nullptr
    )
{
    Tin* t = createWidget<Tin>(Vec(x, y));
    if (width <= 0) width = 120.f;
    if (height <= 0) height = 14.f;
    t->box.size = Vec(width, height);
    t->setText(content);
    if (on_change) t->set_on_change(on_change);
    if (on_enter) t->set_on_enter(on_enter);
    if (placeholder) t->placeholder = placeholder;
    return t;
}



}