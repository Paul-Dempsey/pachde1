#include "Info.hpp"
#include "../services/text.hpp"
#include "../widgets/draw-logo.hpp"
using namespace widgetry;

namespace pachde {

InfoPanel::InfoPanel(InfoModule* module, InfoTheme* info, Vec size)
{
    this->module = module;
    preview = !module;
    info_theme = info;
    box.size = size;
}

void InfoPanel::fetchColors()
{
    panel = info_theme->getDisplayMainColor();
    background = info_theme->getDisplayTextBackground();
    text_color = info_theme->getDisplayTextColor();
    if (module && module->getCopperTarget() != CopperTarget::None) {
        auto left = module->leftExpanderColor();
        auto right = module->rightExpanderColor();
        switch (module->getCopperTarget()) {
            case CopperTarget::Interior: {
                if (isColorVisible(left)) {
                    background = left;
                    info_theme->setUserTextBackground(background);
                }
                if (isColorVisible(right)) {
                    text_color = right;
                    info_theme->setUserTextColor(text_color);
                }
            } break;

            case CopperTarget::Panel: {
                bool set_panel = false;
                if (isColorVisible(left)) {
                    set_panel = true;
                    panel = left;
                }
                if (isColorVisible(right)) {
                    set_panel = true;
                    panel = right;
                }
                if (set_panel) {
                    info_theme->setMainColor(panel);
                }
            } break;

            case CopperTarget::None:
                break;
        }
    }
}

void InfoPanel::step()
{
    fetchColors();
    Widget::step();
}

void InfoPanel::showText(const DrawArgs &args, std::shared_ptr<rack::window::Font> font, std::string text)
{
    nvgScissor(args.vg, RECT_ARGS(args.clipBox));
    auto font_size = info_theme->getFontSize();
    SetTextStyle(args.vg, font, text_color, font_size);
    nvgTextAlign(args.vg, nvgAlignFromHAlign(info_theme->getHorizontalAlignment()));
    nvgTextBox(args.vg, box.pos.x + 10.f, box.pos.y + ONE_HP + font_size, box.size.x - 15.f, text.c_str(), nullptr);
    nvgResetScissor(args.vg);
}

void InfoPanel::drawError(const DrawArgs &args)
{
    auto r = box.size.x /3.;
    auto color = nvgRGB(250,0,0);
    nvgBeginPath(args.vg);
    nvgCircle(args.vg, box.size.x/2., box.size.y/2., r);
    nvgMoveTo(args.vg, box.size.x/2.-r, box.size.y/2 + r);
    nvgLineTo(args.vg, box.size.x/2 + r, box.size.y/2 - r);
    nvgStrokeColor(args.vg, color);
    nvgStrokeWidth(args.vg, 6.);
    nvgStroke(args.vg);
}

void InfoPanel::drawText(const DrawArgs &args)
{
    nvgBeginPath(args.vg);
    nvgRect(args.vg, 5.0, ONE_HP, box.size.x - 10.f, box.size.y - RACK_GRID_WIDTH * 2.f);
    nvgFillColor(args.vg, background);
    nvgFill(args.vg);

    std::string text = module ? module->text : "<sample text>";
    if (!text.empty()) {
        auto font = APP->window->loadFont(info_theme->font_file);
        if (FontOk(font)) {
            showText(args, font, text);
        } else {
            info_theme->resetFont();
            font = APP->window->loadFont(info_theme->font_file);
            if (FontOk(font)) {
                showText(args, font, text);
            } else {
                drawError(args);
            }
        }
    }

}

void InfoPanel::drawLayer(const DrawArgs &args, int layer)
{
    if (layer == 1 && info_theme->getBrilliant()) {
        drawText(args);
    }
}

void InfoPanel::draw(const DrawArgs &args)
{
    nvgBeginPath(args.vg);
    nvgRect(args.vg, 0.f, 0.f, box.size.x, box.size.y);
    nvgFillColor(args.vg, panel);
    nvgFill(args.vg);

    if (!info_theme->getBrilliant()) {
        drawText(args);
    }

    if (preview) {
        DrawLogo(args.vg, box.size.x / 2.f - 30.0f, box.size.y / 2.f - 40.f, Overlay(COLOR_BRAND), 4.f);
    }
    Widget::draw(args);
}

}