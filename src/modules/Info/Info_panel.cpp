#include "Info.hpp"
#include "services/text.hpp"
#include "widgets/draw-logo.hpp"
using namespace widgetry;

namespace pachde {

InfoPanel::InfoPanel(InfoModule* module, InfoSettings* info, ThemeBase* theme, Vec size)
{
    this->module = module;
    preview = !module;
    settings = info;
    theme_holder = theme;
    box.size = size;
}

void InfoPanel::fetchColors()
{
    panel = settings->getDisplayPanelColor();
    text_color = settings->getDisplayTextColor();
    if (module && module->getCopperTarget() != CopperTarget::None) {
        auto left = toPacked(module->leftExpanderColor());
        auto right = toPacked(module->rightExpanderColor());
        switch (module->getCopperTarget()) {
            case CopperTarget::Text: {
                if (left) {
                    text_color = left;
                }
                if (right) {
                    text_color = right;
                }
                settings->setUserTextColor(text_color);
            } break;

            case CopperTarget::Panel: {
                //bool set_panel = false;
                if (left) {
                    //set_panel = true;
                    panel = left;
                }
                if (right) {
                    //set_panel = true;
                    panel = right;
                }
                //if (set_panel) {
                    theme_holder->setMainColor(panel);
                    settings->setUserPanelColor(panel);
                //}
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
    auto vg = args.vg;
    if (!FontOk(font)) return;

    nvgSave(vg);
    auto font_size = settings->getFontSize();
    SetTextStyle(vg, font, fromPacked(text_color), font_size);
    nvgTextAlign(vg, NVG_ALIGN_TOP|nvgAlignFromHAlign(settings->getHorizontalAlignment()));

    auto orientation = settings->getOrientation();
    float width = box.size.x - 15.f;
    float height = box.size.y - 25.f;
    if ((orientation == Orientation::Down) || (orientation == Orientation::Up)) {
        std::swap(width, height);
    }

    int vroom = std::floor(height / font_size);
    NVGtextRow* rows = (NVGtextRow*)malloc(vroom*sizeof(NVGtextRow));
    if (!rows) {
        nvgRestore(vg);
        return;
    }

    int nrows = nvgTextBreakLines(vg, text.c_str(), nullptr, width, rows, vroom);
    free(rows);
    float text_height = nrows * font_size;

    float x,y;
    switch (orientation) {
    case Orientation::Normal:
        x = 7.5f;
        y = 190.f - text_height*.5;
        break;
    case Orientation::Down:
        nvgRotate(vg, M_PI/2.f);
        x = 12.5;
        y = -height*.5 - text_height*.5;
        break;
    case Orientation::Up:
        nvgRotate(vg, M_PI/-2.f);
        x = (-box.size.y) + (box.size.y - width)*.5;
        y = (box.size.x - text_height)*.5;
        break;

    case Orientation::Inverted:
        nvgRotate(vg, M_PI);
        nvgTranslate(vg, -box.size.x, 0);
        x = ((box.size.x - width)*.5f);
        y = -(190.f + text_height*.5);
        break;
    }
    // Debug bounds box
    // nvgBeginPath(vg);
    // nvgRect(vg, x, y, width, text_height);
    // nvgStrokeColor(vg, nvgHSL(216.f, .5, .5));
    // nvgStrokeWidth(vg, 2.0);
    // nvgStroke(vg);
    nvgTextBox(vg, x, y, width, text.c_str(), nullptr);
    nvgRestore(vg);
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
    std::string text = module ? module->text : "<sample text>";
    if (!text.empty()) {
        auto font = settings->getFont();
        if (!FontOk(font)) {
            settings->resetFont();
            font = settings->getFont();
        }
        if (FontOk(font)) {
            showText(args, font, text);
        } else {
            drawError(args);
        }
    }
}

void InfoPanel::drawLayer(const DrawArgs &args, int layer)
{
    if (layer == 1 && settings->getBrilliant()) {
        drawText(args);
    }
}

void InfoPanel::draw(const DrawArgs &args)
{
    nvgBeginPath(args.vg);
    nvgRect(args.vg, 0.f, 0.f, box.size.x, box.size.y);
    nvgFillColor(args.vg, fromPacked(panel));
    nvgFill(args.vg);

    if (!settings->getBrilliant()) {
        drawText(args);
    }

    if (preview) {
        DrawLogo(args.vg, box.size.x / 2.f - 30.0f, box.size.y / 2.f - 40.f, Overlay(COLOR_BRAND), 4.f);
    }
    Widget::draw(args);
}

}