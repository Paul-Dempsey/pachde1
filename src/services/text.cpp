#include "../myplugin.hpp"
#include "text.hpp"

namespace pachde {

std::shared_ptr<window::Font> GetPluginFontSemiBold(const char * path)
{
    return APP->window->loadFont(asset::plugin(pluginInstance, path ? path : "res/fonts/HankenGrotesk-SemiBold.ttf"));
}
std::shared_ptr<window::Font> GetPluginFontRegular(const char * path)
{
    return APP->window->loadFont(asset::plugin(pluginInstance, path ? path : "res/fonts/HankenGrotesk-Regular.ttf"));
}

std::string format_string(const char *fmt, ...)
{
    const int len = 512;
    std::string s(len, '\0');
    va_list args;
    va_start(args, fmt);
    auto r = std::vsnprintf(&(*s.begin()), len, fmt, args);
    va_end(args);
    if (r < 0) return "??";
    s.resize(std::min(r, len));
    return s;
}

size_t format_buffer(char * buffer, size_t length, const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    auto r = std::vsnprintf(buffer, length, fmt, args);
    va_end(args);
    return r;
}

std::string rgba_string(PackedColor color) {
    uint8_t r = color & 0xff; color >>= 8;
    uint8_t g = color & 0xff; color >>= 8;
    uint8_t b = color & 0xff; color >>= 8;
    uint8_t a = color & 0xff;
    if (a < 255) {
        return format_string("rgba(%d, %d, %d, %d)", r, g, b, a);
    } else {
        return format_string("rgb(%d, %d, %d)", r, g, b);
    }
}

std::string hsla_string(float hue, float saturation, float lightness, float alpha)
{
    char buffer[25];
     std::string result{"hsl"};
     int ihue = static_cast<int>(hue * 360.f);
     if (alpha < 1.0f) {
        result.append("a(");
        auto len = format_buffer(buffer, sizeof(buffer), "%d, ", ihue);
        result.append(buffer, len);
        if (saturation > .01f) {
            len = format_buffer(buffer, sizeof(buffer), "%.1f, ", saturation);
            result.append(buffer, len);
        } else {
            result.append("0");
        }
        if (lightness > .01f) {
            len = format_buffer(buffer, sizeof(buffer), "%.1f, ", lightness);
            result.append(buffer, len);
        } else {
            result.append("0,");
        }
        if (alpha > .01f) {
            len = format_buffer(buffer, sizeof(buffer), "%.1f)", alpha);
            result.append(buffer, len);
        } else {
            result.append("0)");
        }
     } else {
        result.push_back('(');
        auto len = format_buffer(buffer, sizeof(buffer), "%d, ", ihue);
        result.append(buffer, len);
        if (saturation > .01f) {
            len = format_buffer(buffer, sizeof(buffer), "%.1f, ", saturation);
            result.append(buffer, len);
        } else {
            result.append("0,");
        }
        if (lightness > .01f) {
            len = format_buffer(buffer, sizeof(buffer), "%.1f)", lightness);
            result.append(buffer, len);
        } else {
            result.append("0)");
        }
     }
     return result;
}

void SetTextStyle(NVGcontext *vg, std::shared_ptr<window::Font> font, NVGcolor color, float height)
{
    assert(FontOk(font));
    nvgFillColor(vg, color);
    nvgFontFaceId(vg, font->handle);
    nvgTextLetterSpacing(vg, 0.f);
    nvgFontSize(vg, height);
}

void CenterText(NVGcontext *vg, float x, float y, const char * text, const char * end)
{
    nvgTextAlign(vg, NVG_ALIGN_CENTER);
    nvgText(vg, x, y, text, end);
}

void RightAlignText(NVGcontext *vg, float x, float y, const char * text, const char * end, BaselineCorrection correction)
{

    float bounds[4] = { 0, 0, 0, 0 };
    nvgTextBounds(vg, 0, 0, text, end, bounds);

    if (correction == BaselineCorrection::Baseline) {
        float tm_ascent;
        nvgTextMetrics(vg, &tm_ascent, nullptr, nullptr);
        nvgText(vg, x - bounds[2], y + tm_ascent, text, end);
    } else {
        nvgText(vg, x - bounds[2], y, text, end);
    }

}

void draw_text_box (
    NVGcontext *vg,
    float x, float y, float w, float h,
    float left_margin, float right_margin,
    std::string text,
    std::shared_ptr<rack::window::Font> font,
    float font_size,
    PackedColor text_color,
    HAlign halign,
    VAlign valign,
    float first_baseline
) {
    // { // DEBUG
    //     auto co_debug = nvgHSLAf(30.f/360.f, .8f, .8f, .35f);
    //     BoxRect(vg, x, y, w, h, co_debug);
    //     if (left_margin > 0.f) { Line(vg, x + left_margin, y, x + left_margin, y + h, co_debug); }
    //     if (right_margin > 0.f) { Line(vg, x + w - right_margin, y, x + w - right_margin, y + h, co_debug); }
    // }
    NVGtextRow text_rows[50];
    //nvgSave(vg);
    SetTextStyle(vg, font, fromPacked(text_color), font_size);

    float width = (HAlign::Center == halign) ? w : w - (left_margin + right_margin);
    int nrows = nvgTextBreakLines(vg, text.c_str(), nullptr, width, text_rows, 50);
    float tm_ascent;
    float tm_height;
	nvgTextMetrics(vg, &tm_ascent, nullptr, &tm_height);
    float total_height = nrows * tm_height;
    float ty{0};
    switch (valign) {
        case VAlign::Top: ty = y; break;
        case VAlign::Middle: ty = y + h*.5 - total_height*.5; break;
        case VAlign::Bottom: ty = h - total_height; break;
        case VAlign::Baseline: ty = (std::isfinite(first_baseline)) ? first_baseline : tm_ascent; break;
    }
    nvgTextAlign(vg, NVG_ALIGN_LEFT | nvgAlignFromVAlign(valign));
    float tx{0};
    switch (halign) {
    case HAlign::Left: {
        tx = x + left_margin;
        NVGtextRow* row = text_rows;
        for (int n = 0; n< nrows; n++, row++) {
            nvgText(vg, tx, ty, row->start, row->end);
            ty += tm_height;
        }
    } break;
    case HAlign::Center: {
        NVGtextRow* row = text_rows;
        for (int n = 0; n< nrows; n++, row++) {
            nvgText(vg, x + w*.5f - row->width *.5f, ty, row->start, row->end);
            ty += tm_height;
        }
    } break;
    case HAlign::Right: {
        tx = x + w - right_margin;
        NVGtextRow* row = text_rows;
        for (int n = 0; n< nrows; n++, row++) {
            nvgText(vg, tx - row->width, ty, row->start, row->end);
            ty += tm_height;
        }
    } break;
    }
    //nvgRestore(vg);
}

void draw_oriented_text_box(
    NVGcontext *vg,
    Rect box,
    float left_margin,
    float right_margin,
    const std::string& text,
    std::shared_ptr<rack::window::Font> font,
    float font_size,
    PackedColor text_color,
    HAlign halign,
    VAlign valign,
    Orientation orientation,
    float first_baseline
) {
    float width = box.size.x;
    float height = box.size.y;
    if ((orientation == Orientation::Down) || (orientation == Orientation::Up)) {
        std::swap(width, height);
    }

    switch (orientation) {
    case Orientation::Normal:
        draw_text_box(vg, box.pos.x, box.pos.y, width, height,
            left_margin, right_margin,
            text, font, font_size, text_color, halign, valign, first_baseline);
        break;

    case Orientation::Down:
        nvgSave(vg);
        nvgRotate(vg, NVG_PI/2.f); //down
        nvgTranslate(vg, 0.f, -height);
        draw_text_box(vg, box.pos.x, box.pos.y, width, height,
            left_margin, right_margin,
            text, font, font_size, text_color, halign, valign, first_baseline);
        nvgRestore(vg);
        break;

    case Orientation::Up:
        nvgSave(vg);
        nvgRotate(vg, -NVG_PI/2.f); //up
        nvgTranslate(vg, -width, 0.f);
        draw_text_box(vg, box.pos.x, box.pos.y, width, height,
            left_margin, right_margin,
            text, font, font_size, text_color, halign, valign, first_baseline);
        nvgRestore(vg);
        break;

    case Orientation::Inverted:
        nvgSave(vg);
        nvgRotate(vg, NVG_PI);
        nvgTranslate(vg, -width, -height);
        draw_text_box(vg, box.pos.x, box.pos.y, width, height,
            left_margin, right_margin,
            text, font, font_size, text_color, halign, valign, first_baseline);
        nvgRestore(vg);
        break;
    }
}

}