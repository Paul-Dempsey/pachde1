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
        NVGcolor color;
        if (module->rightExpanderColor(color) || module->leftExpanderColor(color)) {
            switch (module->getCopperTarget()) {
            case CopperTarget::Text: settings->setUserTextColor(toPacked(color)); break;
            case CopperTarget::Panel:
                panel = toPacked(color);
                theme_holder->setMainColor(panel);
                settings->setUserPanelColor(panel);
                break;
            default: break;
            }
        }
    }
}

void InfoPanel::step()
{
    fetchColors();
    Widget::step();
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

void dt(NVGcontext *vg, float x, float y, float w, float h, float lm, float rm,
    std::string text,
    std::shared_ptr<rack::window::Font> font,
    float font_size,
    NVGalign align,
    const NVGcolor& text_color
) {
    NVGtextRow text_rows[10];

    SetTextStyle(vg, font, text_color, font_size);
    nvgTextAlign(vg, align);
    float width = (align & NVG_ALIGN_CENTER) ? w : w - (lm + rm);
    int nrows = nvgTextBreakLines(vg, text.c_str(), nullptr, width, text_rows, 10);
    float text_height = nrows * font_size;
    float text_width = 0.f;
    for (int i = 0; i < 10; ++i) {
        text_width = std::max(text_width, std::min(width, text_rows[i].width));
    }
    float tx{0};
    switch (align & (NVG_ALIGN_LEFT|NVG_ALIGN_CENTER|NVG_ALIGN_RIGHT)) {
        case NVG_ALIGN_LEFT: tx = x + lm; break;
        case NVG_ALIGN_CENTER: break;
        case NVG_ALIGN_RIGHT: tx = x + w - rm - text_width; break;
    }
    float ty = y + h*.5 - text_height*.5;
    nvgTextBox(vg, tx, ty, text_width, text.c_str(), nullptr);
}

void InfoPanel::showText(const DrawArgs &args, std::shared_ptr<rack::window::Font> font, const std::string& text)
{
#if DEV_BUILD
    draw_margins(args, box, settings);
#endif
    auto vg = args.vg;
    auto font_size = module ? settings->getFontSize() : 60;
    auto align = module ? settings->getHorizontalAlignment() : HAlign::Center;
    Orientation orientation = module ? settings->getOrientation() : Orientation::Up;
    float width = box.size.x;
    float height = box.size.y;
    if ((orientation == Orientation::Down) || (orientation == Orientation::Up)) {
        std::swap(width, height);
    }
    float left_margin = module ? settings->left_margin : 7.5f;
    float right_margin = module ? settings->right_margin : 7.5f;
    NVGalign nvg_align = NVGalign(NVG_ALIGN_TOP|nvgAlignFromHAlign(align));
    auto co_text = fromPacked(text_color);

    switch (orientation) {
    case Orientation::Normal:
        dt(vg, 0, 0, width, height, left_margin, right_margin, text, font, font_size, nvg_align, co_text);
        break;

    case Orientation::Down:
        nvgSave(vg);
        nvgRotate(vg, M_PI/2.f); //down
        nvgTranslate(vg, 0, -height);
        dt(vg, 0, 0, width, height, left_margin, right_margin, text, font, font_size, nvg_align, co_text);
        nvgRestore(vg);
        break;

    case Orientation::Up:
        nvgSave(vg);
        nvgRotate(vg, -M_PI/2.f); //up
        nvgTranslate(vg, -width, 0);
        dt(vg, 0, 0, width, height, left_margin, right_margin, text, font, font_size, nvg_align, co_text);
        nvgRestore(vg);
        break;

    case Orientation::Inverted:
        nvgSave(vg);
        nvgRotate(vg, M_PI);
        nvgTranslate(vg, -width, -height);
        dt(vg, 0, 0, width, height, left_margin, right_margin, text, font, font_size, nvg_align, co_text);
        nvgRestore(vg);
        break;
    }
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
    std::string text = module ? module->text : "#d Info";
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
        DrawLogo(args.vg, box.size.x*.5-30.0f, 35.f, Overlay(COLOR_BRAND), 4.25f);
    }
    Widget::draw(args);
}

}