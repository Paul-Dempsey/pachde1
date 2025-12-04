#pragma once
#include "Info.hpp"
#include "widgets/text-input.hpp"
#include "widgets/close-button.hpp"

namespace pachde {
struct InfoModuleWidget;

struct InfoEdit : OpaqueWidget
{
    using Base = OpaqueWidget;

    InfoModuleWidget* ui{nullptr};
    CloseButton* close_button{nullptr};
    MultiTextInput* text_input{nullptr};

    InfoEdit(InfoModuleWidget* host);
    bool editing();
    void begin_editing();
    void close();
    void size_to_parent();
    void onButton(const ButtonEvent& e) override;
    void onHoverKey(const HoverKeyEvent& e) override;
    void step() override;
    void draw(const DrawArgs& args) override;
};

}