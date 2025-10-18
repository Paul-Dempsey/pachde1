#include "svg-theme-2.hpp"

namespace svg_theme_2 {

std::string parseTag(const char * id)
{
    if (!id || !*id) return "";
    const char * scan = id;
    while (*scan) scan++;
    while (scan > id) {
        if ('-' == *scan--) {
            if ((scan >= id) && ('-' == *scan)) {
                return std::string(scan+1);
            }
        }
    }
    return "";
}

std::shared_ptr<Style> SvgTheme::getStyle(const std::string &name)
{
    auto found = styles.find(name);
    return (found == styles.end()) ? nullptr : found->second;
}

bool SvgTheme::getGradient(const Gradient** result, const char *name)
{
    if (!result) return false;
    auto style = getStyle(name);
    if (style && style->isApplyFill() && style->fill.isGradient()) {
        *result = style->fill.getGradient();
        return true;
    }
    return false;
}

bool SvgTheme::getFillColor(PackedColor& result, const char *name, bool with_opacity)
{
    auto style = getStyle(name);
    if (style && style->isApplyFill() && style->fill.isColor()) {
        result = with_opacity ? style->fillWithOpacity() : style->fill_color();
        return true;
    }
    return false;
}

bool SvgTheme::getStroke(PackedColor& result, const char *name, bool with_opacity, float* width)
{
    auto style = getStyle(name);
    if (style && style->isApplyStroke()) {
        if (width && style->apply_stroke_width) *width = style->stroke_width;
        result = with_opacity ? style->strokeWithOpacity() : style->stroke_color();
        return true;
    }
    return false;
}

void compact_space(char * scan) {
    bool poke_space = true;
    char * poke = scan;

    while (std::isspace(*scan)) scan++;

    while (true) {
        switch (*scan) {
        case 0:
            *poke++ = 0;
            return;

        case '/':
            if ('/' == *(scan+1)) {
                scan += 2;
                while (*scan && (*scan != '\n')) {
                    scan++;
                }
            } else {
                poke_space = true;
                *poke++ = *scan++;
            }
            break;

        case ' ':
        case '\t':
        case '\r':
            if (poke_space) {
                *poke++ = ' ';
                poke_space = false;
            }
            scan++;
            break;

        default:
            poke_space = true;
            *poke++ = *scan++;
            break;
        }
    }
}

const char * skip_space(const char * scan)
{
    while (' ' == *scan) scan++;
    return scan;
}

const char* parse_name(const char *scan, std::string& name)
{
    auto start = scan;

    while (true) {
        switch (*scan) {
        case 0: return scan;
        case '\n': return scan+1;
        case '=': {
            const char * end = scan - 1;
            scan++;
            while ((end > start) && (' ' == *end)) {
                --end;
            }
            name = std::string(start, 1 + end - start);
            return scan;
        } break;

        default:
            scan++;
            break;
        }
    }
}

template <typename T>
inline bool in_range(T value, T minimum, T maximum) { return minimum <= value && value <= maximum; }

bool is_name_char(char ch) {
    if ((ch < '-') || (ch > 'z')) return false;
    if (in_range(ch, '0', '9')) return true;
    if (in_range(ch, 'A', 'Z')) return true;
    if (in_range(ch, 'a', 'z')) return true;
    if ('_' == ch) return true;
    if ('-' == ch) return true;
    if ('.' == ch) return true;
    return false;
}

const char* parse_theme_name(const char *scan, std::string& name)
{
    while (' ' == *scan) scan++;
    if ('\n' == *scan) {
        name = "";
        return scan + 1;
    }
    auto start = scan;
    while (is_name_char(*scan)) scan++;
    if (scan == start) return scan;
    name = std::string(start, scan);
    while (true) {
        char ch = *scan;
        if (ch) {
            if ('\n' == ch) {
                return scan + 1;
            }
            scan++;
        } else {
            return scan;
        }
    }
}

// @theme=Dark
// item-a= #abcdef
// item-b= #abcdef stroke hsl(12,51%,80%) width 1.25 opacity .8
// item-c= fill [ #abcdef89 index 0 offset 0, #efefef index 1 offset 8.5] stroke #ffe width .25
/*
        fill := ["fill"] color|gradient
    gradient := '[' stop+ ']'
        stop := (color|index|offset)
      stroke := "stroke" (color|gradient)
       width := "width" float
       index := "index" integer
      offset := "offset" float
       color := "#" hex{3,4,6,8} | rgb | rgba | hsl | hsla
*/

enum class StyleKey : int {
    None = -1,
    Fill,
    Hsl,
    Hsla,
    Index,
    Offset,
    Opacity,
    Rgb,
    Rgba,
    Stroke,
    Width,
    Hex,
    Gradient
};
const char * keyword_table = "\4fill\3hsl\4hsla\5index\6offset\7opacity\3rgb\4rgba\6stroke\5width";

const char* parse_keyword(const char *scan, StyleKey& key)
{
    scan = skip_space(scan);

    if ('#' == *scan) {
        key = StyleKey::Hex;
        return scan;
    }
    if ('[' == *scan) {
        key = StyleKey::Gradient;
        return scan;
    }

    const char * kw = keyword_table;
    int k = 0;
    while (true) {
        int len = *kw++;
        if (*scan < *kw) {
            key = StyleKey::None;
            return scan;
        }
        if ((*scan == *kw) && (0 == strncmp(scan, kw, len))) {
            auto end = scan + len;
            if (!std::isalpha(*end)) {
                key = StyleKey(k);
                return end;
            }
        }
        kw += len;
        if (!*kw) {
            key = StyleKey::None;
            return scan;
        }
        k++;
        assert(k < (int)StyleKey::Hex);
    }
}

bool parse_stop(const char *scan, GradientStop& stop, const char **end)
{
    StyleKey key;
    const char * token_end;
    while (true) {
        switch (*scan) {
        case ' ': scan++; break;

        case ',':
        case ']':
            *end = scan;
            return true;

        default:
            token_end = parse_keyword(scan, key);
            switch (key) {
            case StyleKey::Rgb:
            case StyleKey::Rgba:
                if (parseRgbaColor(stop.color, colors::G0, token_end, end)) {
                    scan = *end;
                } else {
                    return false;
                }
                break;

            case StyleKey::Hsl:
            case StyleKey::Hsla:
                if (parseHslaColor(stop.color, colors::G0, token_end, end)) {
                    scan = *end;
                } else {
                    return false;
                }
                break;

            case StyleKey::Offset: {
                scan = skip_space(token_end);
                float f = parse_float(scan, end);
                if (_isnanf(f)) return false;
                scan = *end;
                stop.offset = f;
            } break;

            case StyleKey::Index: {
                scan = skip_space(token_end);
                auto number = parse_uint64(scan, end);
                if ((scan == *end) || (number > 4)) return false;
                scan = *end;
                stop.index = (int)number;
            } break;

            case StyleKey::Hex: {
                if (parseHexColor(stop.color, colors::G0, scan, end)) {
                    scan = *end;
                } else {
                    return false;
                }
            } break;

            default:
                *end = scan;
                return false;
            }
            break;
        }
    }


}

bool parse_gradient(const char *scan, Gradient& gradient, const char **end)
{
    GradientStop stop;
    while (true) {
        switch (*scan) {
        case ' ': scan++; break;

        case '[':
        case ',':
            scan++;
            if (parse_stop(scan, stop, end)) {
                gradient.stops.push_back(stop);
                scan = *end;
            } else {
                *end = scan;
                return false;
            }
            break;

        case ']':
            *end = scan+1;
            return true;

        default:
            *end = scan;
            return false;
        }
    }
}

bool parse_fill(const char *scan, std::shared_ptr<Style>& result, const char **end)
{
    StyleKey key;
    *end = parse_keyword(scan, key);
    switch (key) {
    case StyleKey::None:
    case StyleKey::Fill:
        break;

    case StyleKey::Gradient: {
        Gradient gradient;
        scan = *end;
        if (parse_gradient(scan, gradient, end)) {
            if (!result) result = std::make_shared<Style>();
            result->setFillGradient(gradient);
            return true;
        }
    } break;

    case StyleKey::Hsl:
    case StyleKey::Hsla: {
        PackedColor color;
        if (parseHslaColor(color, colors::G0, scan, end)) {
            if (!result) result = std::make_shared<Style>();
            result->setFillColor(color);
            return true;
        }
    } break;

    case StyleKey::Index:
    case StyleKey::Offset:
    case StyleKey::Opacity:
        break;

    case StyleKey::Rgb:
    case StyleKey::Rgba: {
        PackedColor color;
        if (parseRgbaColor(color, colors::G0, scan, end)) {
            if (!result) result = std::make_shared<Style>();
            result->setFillColor(color);
            return true;
        }
    } break;


    case StyleKey::Stroke:
    case StyleKey::Width: break;

    case StyleKey::Hex: {
        PackedColor color;
        if (parseHexColor(color, colors::G0, scan, end)) {
            if (!result) result = std::make_shared<Style>();
            result->setFillColor(color);
            return true;
        }
    } break;
    }

    *end = scan;
    result = nullptr;
    return false;
}

bool parse_stroke(const char *scan, std::shared_ptr<Style>& result, const char **end)
{
    StyleKey key;
    *end = parse_keyword(scan, key);
    switch (key) {
    case StyleKey::None:
    case StyleKey::Fill:
        break;

    case StyleKey::Gradient: {
        Gradient gradient;
        scan = *end;
        if (parse_gradient(scan, gradient, end)) {
            if (!result) result = std::make_shared<Style>();
            result->setStrokeGradient(gradient);
            return true;
        }
    } break;

    case StyleKey::Hsl:
    case StyleKey::Hsla: {
        PackedColor color;
        if (parseHslaColor(color, colors::G0, scan, end)) {
            if (!result) result = std::make_shared<Style>();
            result->setStrokeColor(color);
            return true;
        }
    } break;

    case StyleKey::Index:
    case StyleKey::Offset:
    case StyleKey::Opacity:
        break;

    case StyleKey::Rgb:
    case StyleKey::Rgba: {
        PackedColor color;
        if (parseRgbaColor(color, colors::G0, scan, end)) {
            if (!result) result = std::make_shared<Style>();
            result->setStrokeColor(color);
            return true;
        }
    } break;


    case StyleKey::Stroke:
    case StyleKey::Width: break;

    case StyleKey::Hex: {
        PackedColor color;
        if (parseHexColor(color, colors::G0, scan, end)) {
            if (!result) result = std::make_shared<Style>();
            result->setStrokeColor(color);
            return true;
        }
    } break;
    }

    *end = scan;
    result = nullptr;
    return false;
}

const char* parse_style(const char *scan, std::shared_ptr<Style>& result)
{
    std::shared_ptr<Style> style{nullptr};
    StyleKey key;
    const char * end;
    while (true) {
        switch (*scan) {
        case ' ': scan++; break;

        case '\n':
            result = style;
            return scan + 1;

        case 0:
            result = style;
            return scan;

        default:
            end = parse_keyword(scan, key);

            switch (key) {
            case StyleKey::Fill:
                scan = end;
                // fallthrough
            case StyleKey::Gradient:
            case StyleKey::Hsl:
            case StyleKey::Hsla:
            case StyleKey::Rgb:
            case StyleKey::Rgba:
            case StyleKey::Hex:
                if (parse_fill(scan, style, &end)) {
                    scan = end;
                } else {
                    result = nullptr;
                    return scan;
                }
                break;

            case StyleKey::Opacity: {
                scan = end;
                float f = parse_float(scan, &end);
                if (_isnanf(f)) {
                    result = nullptr;
                    return scan;
                }
                if (!style) style = std::make_shared<Style>();
                style->setOpacity(f);
                scan = end;
            } break;

            case StyleKey::Stroke:
                scan = end;
                if (parse_stroke(scan, style, &end)) {
                    scan = end;
                } else {
                    result = nullptr;
                    return scan;
                }
                break;

            case StyleKey::Width: {
                scan = end;
                float f = parse_float(scan, &end);
                if (_isnanf(f)) {
                    result = nullptr;
                    return scan;
                }
                if (!style) style = std::make_shared<Style>();
                style->setStrokeWidth(f);
                scan = end;
            } break;

            case StyleKey::Index:
            case StyleKey::Offset:
            case StyleKey::None:
            default:
                result = nullptr;
                return scan;
            }
        }
    }
    result = style;
    return scan;
}


std::shared_ptr<SvgTheme> loadFile(std::string path)
{
    auto size = system::getFileSize(path);
    if (0 == size) { return nullptr; }

    auto f = std::fopen(path.c_str(), "rb");
    if (!f) return nullptr;
    char* bytes = (char*)malloc(size+1);
    if (!bytes) {
        std::fclose(f);
        return nullptr;
    }
    DEFER({free(bytes);});

    auto red = std::fread(bytes, 1, size, f);
    std::fclose(f);
    if (red != size) {
        return nullptr;
    }
    bytes[size] = 0;
    compact_space(bytes);

    auto theme = std::make_shared<SvgTheme>();
    theme->file = path;

    const char * scan = bytes;
    const char * eob = scan + size;
    bool first = true;
    while (*scan && (scan < eob)) {
        while (std::isspace(*scan)) scan++;
        std::string name;
        scan = parse_name(scan, name);
        if (name.empty()) {
            // error
            return nullptr;
        }
        if (first && name == "@theme") {
            scan = parse_theme_name(scan, name);
            if (!name.empty()) {
                theme->name = name;
            }
        } else {
            std::shared_ptr<Style> style{nullptr};
            scan = parse_style(scan, style);
            if (style) {
                theme->styles[name] = style;
            } else {
                return nullptr;
            }
        }
        first = false;
    }

    if (theme->name.empty()) {
        theme->name = system::getStem(path);
    }
    return theme;
}

}