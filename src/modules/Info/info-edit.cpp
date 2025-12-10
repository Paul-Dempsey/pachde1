#include "info-edit.hpp"

namespace pachde {

InfoEdit::InfoEdit(InfoModuleWidget *host) : ui(host) {
    assert(ui && ui->module);
    size_to_parent();
    setVisible(false);
}

bool InfoEdit::editing() { return isVisible(); }

void InfoEdit::begin_editing() {
    if (!close_button) {
        close_button = createWidget<CloseButton>(Vec(box.size.x - 15.5, .5));
        close_button->set_handler([=](){ close(); });
        addChild(close_button);
    }
    if (!text_input) {
        text_input = new MultiTextInput();
        text_input->box.pos = Vec(.5, 16.f);
        text_input->box.size = Vec(box.size.x - 1, box.size.y-16.f);
        text_input->multiline = true;
        text_input->placeholder = "Type or paste your Info here.";
        text_input->setText(ui->my_module->text);
        text_input->change_handler = [=](std::string content){
            ui->my_module->text = content;
        };
        addChild(text_input);
        APP->event->setSelectedWidget(text_input);
    }
    setVisible(true);
}

void InfoEdit::close() {
    setVisible(false);
}

void InfoEdit::size_to_parent() {
    box.pos = Vec(3.5f, 16.f);
    box.size = Vec(ui->box.size.x - 7.f, ui->box.size.y - 30.f);
    if (close_button) {
        close_button->box.pos = Vec(box.size.x - 15.5, .5);
    }
    if (text_input) {
        text_input->box.pos = Vec(.5, 16.f);
        text_input->box.size = Vec(box.size.x - 1, box.size.y-16.f);
    }
}

void InfoEdit::onHoverKey(const HoverKeyEvent &e) {
    if (editing()
        && (e.action == GLFW_PRESS)
        && ((e.mods & RACK_MOD_MASK) == 0)
        && (e.key == GLFW_KEY_ESCAPE)
    ) {
        close();
    }
    Base::onHoverKey(e);
}

void InfoEdit::step() {
    size_to_parent();
    Base::step();
}

void InfoEdit::draw(const DrawArgs &args)
{
    if (editing()) {
        FillRect(args.vg, box.size.x - 16, 0, 15, 15, RampGray(G_50));
    }
    Base::draw(args);
}
}