#include "Info.hpp"
#include "services/text.hpp"
#include "widgets/draw-logo.hpp"
using namespace widgetry;

namespace pachde {

InfoPanel::InfoPanel(InfoModuleWidget* info, InfoSettings* settings, ThemeBase* theme, Vec size) :
    info_module(info->my_module),
    ui(info),
    settings(settings),
    theme_holder(theme),
    preview(nullptr == info->my_module)
{
    box.size = size;
}

void InfoPanel::fetchColors()
{
    if (!info_module) return;

    panel = settings->getDisplayPanelColor();
    text_color = settings->getDisplayTextColor();

    NVGcolor color;
    if (info_module->getLeftCopperTarget() != CopperTarget::None) {
        if (info_module->leftExpanderColor(color)) {
            switch (info_module->getLeftCopperTarget()) {
            case CopperTarget::Panel:
                panel = toPacked(color);
                theme_holder->setMainColor(panel);
                settings->setUserPanelColor(panel);
                break;
            case CopperTarget::Text:
                text_color = toPacked(color);
                settings->setUserTextColor(text_color);
                break;
            default: break;
            }
        }
    }
    if (info_module->getRightCopperTarget() != CopperTarget::None) {
        if (info_module->rightExpanderColor(color)) {
            switch (info_module->getRightCopperTarget()) {
            case CopperTarget::Panel:
                panel = toPacked(color);
                theme_holder->setMainColor(panel);
                settings->setUserPanelColor(panel);
                break;
            case CopperTarget::Text:
                text_color = toPacked(color);
                settings->setUserTextColor(text_color);
                break;
            default: break;
            }
        }
    }
}

void InfoPanel::step()
{
    Widget::step();
    fetchColors();
}

#if DEV_BUILD
void draw_margins(const rack::widget::Widget::DrawArgs &args, const Rect& box, InfoSettings * settings) {
    if (!settings) return;
    auto vg = args.vg;
    Orientation orientation = settings->getOrientation();
    nvgBeginPath(vg);
    switch (orientation) {
        default:
        case Orientation::Normal:
            nvgMoveTo(vg, settings->left_margin, 0.f);
            nvgLineTo(vg, settings->left_margin, box.size.y);
            nvgMoveTo(vg, box.size.x - settings->right_margin, 0.f);
            nvgLineTo(vg, box.size.x - settings->right_margin, box.size.y);
            break;
        case Orientation::Inverted:
            nvgMoveTo(vg, box.size.x - settings->left_margin, 0.f);
            nvgLineTo(vg, box.size.x - settings->left_margin, box.size.y);
            nvgMoveTo(vg, settings->right_margin, 0.f);
            nvgLineTo(vg, settings->right_margin, box.size.y);
            break;
        case Orientation::Up:
            nvgMoveTo(vg, 0.f, settings->right_margin);
            nvgLineTo(vg, box.size.x, settings->right_margin);
            nvgMoveTo(vg, 0.f, box.size.y - settings->left_margin);
            nvgLineTo(vg, box.size.x, box.size.y - settings->left_margin);
            break;
        case Orientation::Down:
            nvgMoveTo(vg, 0.f, settings->left_margin);
            nvgLineTo(vg, box.size.x, settings->left_margin);
            nvgMoveTo(vg, 0.f, box.size.y - settings->right_margin);
            nvgLineTo(vg, box.size.x, box.size.y - settings->right_margin);
            break;
    }
    nvgStrokeColor(vg, nvgHSL(320.f/360.f, .5, .5));
    nvgStrokeWidth(vg, 1.0);
    nvgStroke(vg);
}
#endif

void InfoPanel::showText(const DrawArgs &args, std::shared_ptr<rack::window::Font> font, const std::string& text)
{
#if DEV_BUILD
    //draw_margins(args, box, settings);
#endif
    auto font_size = preview ? 60.f : settings->getFontSize();
    auto halign = preview ? HAlign::Center : settings->getHorizontalAlignment();
    auto valign = preview ? VAlign::Middle : settings->getVerticalAlignment();
    Orientation orientation = preview ? Orientation::Up : settings->getOrientation();
    float left_margin   = preview ? 7.5f : settings->left_margin;
    float right_margin  = preview ? 7.5f : settings->right_margin;
    float top_margin    = preview ? 7.5f : settings->top_margin;
    float bottom_margin = preview ? 7.5f : settings->bottom_margin;

    draw_oriented_text_box(
        args.vg, box,
        left_margin, right_margin, top_margin, bottom_margin,
        text, font, font_size, text_color,
        halign, valign, orientation);
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
    std::string text = preview ? "#d Info" : info_module->text;
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
    Base::drawLayer(args, layer);
    if (!ui->editing() && (layer == 1) && settings->getBrilliant()) {
        drawText(args);
    }
}

void InfoPanel::draw(const DrawArgs &args)
{
    nvgBeginPath(args.vg);
    nvgRect(args.vg, 0.f, 0.f, box.size.x, box.size.y);
    nvgFillColor(args.vg, fromPacked(panel));
    nvgFill(args.vg);

    if (!settings->getBrilliant() && !ui->editing()) {
        drawText(args);
    }

    if (preview) {
        DrawLogo(args.vg, box.size.x*.5-30.0f, 35.f, Overlay(COLOR_BRAND), 4.25f);
    }
    Base::draw(args);
}

}