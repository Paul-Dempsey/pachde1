#include "svg-theme-2-load.hpp"

namespace svg_theme_2 {

//
// ErrorContext
//
const char * errorName(ErrorCode code)
{
    switch (code) {
    case ErrorCode::NoError: return "NoError";
    case ErrorCode::SyntaxError: return "SyntaxError";
    case ErrorCode::ExpectedName: return "ExpectedName";
    case ErrorCode::ExpectedFloat: return "ExpectedFloat";
    case ErrorCode::ExpectedNumber: return "ExpectedNumber";
    case ErrorCode::UnexpectedKeyword: return "UnexpectedKeyword";
    case ErrorCode::InvalidColor: return "InvalidColor";
    case ErrorCode::OpenFileError: return "OpenFileError";
    case ErrorCode::ReadFileError: return "ReadFileError";
    case ErrorCode::GeneralError: return "GeneralError";
    default: return "Unknown error";
    }
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

std::string ErrorContext::makeErrorReport()
{
    return format_string("Style Error %d %s (%d:%d):%s", (int)error, errorName(error), line, offset, text);
}

void ErrorContext::clear()
{
    error = NoError;
    text[0] = 0;
    offset = -1;
    data = nullptr;
}

void ErrorContext::setErrorText(ErrorCode code, const char * where)
{
    assert(code != ErrorCode::NoError);
    const char * line_start = data;
    line = 1;
    for (auto scan = data; scan <= where; scan++) {
        if ('\n' == *scan) {
            line_start = scan+1;
            ++line;
        }
    }
    error = code;
    auto scan = line_start;
    while (*scan && '\n' != *scan) scan++;
    std::strncpy(text, line_start, scan - line_start);
    offset = where - line_start;
}

//
// theme parsing
//

void compact_space(char * scan) {
    bool poke_space = true;
    char * poke = scan;

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

inline bool is_letter(char ch) {
    return (ch > '@') && (ch < '{') && ((ch > '`') || (ch < '['));
}

bool is_name_char(char ch) {
    if ((ch < '-') || (ch > 'z')) return false;
    if (is_letter(ch)) return true;
    if (in_range(ch, '0', '9')) return true;
    if ('_' == ch) return true;
    if ('-' == ch) return true;
    if ('.' == ch) return true;
    return false;
}

// optimized for the keyword table which has lengths 3-7
inline bool same_n(const char * a, const char * b, int len)
{
    assert(len > 0);

    int acc = 0;
    switch (len) {
    case 8: acc += (*a++ - *b++); // fallthrough
    case 7: acc += (*a++ - *b++); // fallthrough
    case 6: acc += (*a++ - *b++); // fallthrough
    case 5: acc += (*a++ - *b++); // fallthrough
    case 4: acc += (*a++ - *b++); // fallthrough
    case 3: acc += (*a++ - *b++); // fallthrough
    case 2: acc += (*a++ - *b++); // fallthrough
    case 1: acc += (*a - *b); break;
    default: assert(false); break;
    }
    return 0 == acc;
    return true;
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
        fill := ['fill'] (color|gradient|'none')
    gradient := '[' stop[, stop]* ']'
        stop := (color|index|offset)
      stroke := 'stroke' (color|gradient|'none')
       width := 'width' float
       index := 'index' integer
      offset := 'offset' float
       color := '#'hex{3,4,6,8} | rgb() | rgba() | hsl() | hsla()
*/

enum class StyleKey : int {
    Unknown = -1,
    Fill,
    Hsl,
    Hsla,
    Index,
    None,
    Offset,
    Opacity,
    Rgb,
    Rgba,
    Stroke,
    Width,
    Hex,
    Gradient
};
const char * keyword_table = "\4fill\3hsl\4hsla\5index\4none\6offset\7opacity\3rgb\4rgba\6stroke\5width";

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
            key = StyleKey::Unknown;
            return scan;
        }
        if ((*scan == *kw) && same_n(scan, kw, len)) {
            const char * end = scan + len;
            if (!is_letter(*end)) {
                key = StyleKey(k);
                return end;
            }
        }
        kw += len;
        if (!*kw) {
            key = StyleKey::Unknown;
            return scan;
        }
        k++;
        assert(k < (int)StyleKey::Hex);
    }
}

bool parse_stop(const char *scan, GradientStop& stop, const char **end, ErrorContext* error_context)
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
            case StyleKey::None:
                stop.color = colors::NoColor;
                break;

            case StyleKey::Hex: {
                if (parseHexColor(stop.color, colors::G0, scan, end)) {
                    scan = *end;
                } else {
                    if (error_context) {
                        error_context->setErrorText(ErrorCode::InvalidColor, scan);
                    }
                    return false;
                }
            } break;

            case StyleKey::Hsl:
            case StyleKey::Hsla:
                if (parseHslaColor(stop.color, colors::G0, token_end, end)) {
                    scan = *end;
                } else {
                    if (error_context) {
                        error_context->setErrorText(ErrorCode::InvalidColor, scan);
                    }
                    return false;
                }
                break;

            case StyleKey::Index: {
                scan = skip_space(token_end);
                auto number = parse_uint64(scan, end);
                if ((scan == *end) || (number > 4)) {
                    if (error_context) {
                        error_context->setErrorText(ErrorCode::ExpectedNumber, scan);
                    }
                    return false;
                }
                scan = *end;
                stop.index = (int)number;
            } break;

            case StyleKey::Offset: {
                scan = skip_space(token_end);
                float f = parse_float(scan, end);
                if (_isnanf(f)) {
                    if (error_context) {
                        error_context->setErrorText(ErrorCode::ExpectedFloat, scan);
                    }
                    return false;
                }
                scan = *end;
                stop.offset = f;
            } break;

            case StyleKey::Rgb:
            case StyleKey::Rgba:
                if (parseRgbaColor(stop.color, colors::G0, token_end, end)) {
                    scan = *end;
                } else {
                    if (error_context) {
                        error_context->setErrorText(ErrorCode::InvalidColor, scan);
                    }
                    return false;
                }
                break;

            default:
                if (error_context) {
                    if (key == StyleKey::Unknown) {
                        error_context->setErrorText(ErrorCode::SyntaxError, scan);
                    } else {
                        error_context->setErrorText(ErrorCode::UnexpectedKeyword, scan);
                    }
                }
                *end = scan;
                return false;
            }
            break;
        }
    }
}

bool parse_gradient(const char *scan, Gradient& gradient, const char **end, ErrorContext* error_context)
{
    while (true) {
        switch (*scan) {
        case ' ': scan++; break;

        case '[':
        case ',': {
            scan++;
            GradientStop stop;
            if (parse_stop(scan, stop, end, error_context)) {
                gradient.stops.push_back(stop);
                scan = *end;
            } else {
                assert(!error_context || (ErrorCode::NoError != error_context->error));
                *end = scan;
                return false;
            }
        } break;

        case ']': {
            *end = scan+1;
            bool no_index = gradient.stops.cend() == std::find_if(gradient.stops.cbegin(), gradient.stops.cend(),
                [](const GradientStop& a) { return a.hasIndex(); });
            if (no_index) {
                int index = 0;
                for (GradientStop& stop : gradient.stops) {
                    stop.index = index++;
                }
            } else {
                bool unindexed = gradient.stops.cend() == std::find_if(gradient.stops.cbegin(), gradient.stops.cend(),
                    [](const GradientStop& a) { return a.index < 0; });
                if (unindexed) {
                    // stops are mixed
                    if (error_context) {
                        error_context->setErrorText(ErrorCode::MixedStopIndices, scan);
                    }
                    return false;
                }
            }
            return true;
        } break;

        default:
            *end = scan;
            if (error_context) {
                error_context->setErrorText(ErrorCode::SyntaxError, scan);
            }
            return false;
        }
    }
}

bool parse_fill(const char *scan, std::shared_ptr<Style>& result, const char **end, ErrorContext* error_context)
{
    StyleKey key;
    *end = parse_keyword(scan, key);
    switch (key) {
    case StyleKey::Gradient: {
        Gradient gradient;
        scan = *end;
        if (parse_gradient(scan, gradient, end, error_context)) {
            if (!result) result = std::make_shared<Style>();
            result->setFillGradient(gradient);
            return true;
        }
        assert(!error_context || (ErrorCode::NoError != error_context->error));
    } break;

    case StyleKey::None:
        if (!result) result = std::make_shared<Style>();
        result->setFillColor(colors::NoColor);
        return true;

    case StyleKey::Hex: {
        PackedColor color;
        if (parseHexColor(color, colors::G0, scan, end)) {
            if (!result) result = std::make_shared<Style>();
            result->setFillColor(color);
            return true;
        } else if (error_context) {
            error_context->setErrorText(ErrorCode::InvalidColor, scan);
        }
    } break;

    case StyleKey::Hsl:
    case StyleKey::Hsla: {
        PackedColor color;
        if (parseHslaColor(color, colors::G0, scan, end)) {
            if (!result) result = std::make_shared<Style>();
            result->setFillColor(color);
            return true;
        } else if (error_context) {
            error_context->setErrorText(ErrorCode::InvalidColor, scan);
        }
    } break;

    case StyleKey::Rgb:
    case StyleKey::Rgba: {
        PackedColor color;
        if (parseRgbaColor(color, colors::G0, scan, end)) {
            if (!result) result = std::make_shared<Style>();
            result->setFillColor(color);
            return true;
        } else if (error_context) {
            error_context->setErrorText(ErrorCode::InvalidColor, scan);
        }
    } break;

    case StyleKey::Unknown:
        if (error_context) {
            error_context->setErrorText(ErrorCode::SyntaxError, scan);
        }
        break;

    case StyleKey::Fill:
    case StyleKey::Index:
    case StyleKey::Offset:
    case StyleKey::Opacity:
    case StyleKey::Stroke:
    case StyleKey::Width:
        if (error_context) {
            error_context->setErrorText(ErrorCode::UnexpectedKeyword, scan);
        }
        break;
    }

    *end = scan;
    result = nullptr;
    assert(!error_context || (ErrorCode::NoError != error_context->error));
    return false;
}

bool parse_stroke(const char *scan, std::shared_ptr<Style>& result, const char **end, ErrorContext* error_context)
{
    StyleKey key;
    *end = parse_keyword(scan, key);
    switch (key) {
    case StyleKey::Unknown:
        if (error_context) {
            error_context->setErrorText(ErrorCode::SyntaxError, scan);
        }
        break;

    case StyleKey::Fill:
    case StyleKey::Index:
    case StyleKey::Offset:
    case StyleKey::Opacity:
    case StyleKey::Stroke:
    case StyleKey::Width:
        if (error_context) {
            error_context->setErrorText(ErrorCode::UnexpectedKeyword, scan);
        }
        break;

    case StyleKey::Gradient: {
        Gradient gradient;
        scan = *end;
        if (parse_gradient(scan, gradient, end, error_context)) {
            if (!result) result = std::make_shared<Style>();
            result->setStrokeGradient(gradient);
            return true;
        }
        assert(!error_context || (ErrorCode::NoError != error_context->error));
    } break;

    case StyleKey::Hex: {
        PackedColor color;
        if (parseHexColor(color, colors::G0, scan, end)) {
            if (!result) result = std::make_shared<Style>();
            result->setStrokeColor(color);
            return true;
        } else if (error_context) {
            error_context->setErrorText(ErrorCode::InvalidColor, scan);
        }
    } break;

    case StyleKey::Hsl:
    case StyleKey::Hsla: {
        PackedColor color;
        if (parseHslaColor(color, colors::G0, scan, end)) {
            if (!result) result = std::make_shared<Style>();
            result->setStrokeColor(color);
            return true;
        } else if (error_context) {
            error_context->setErrorText(ErrorCode::InvalidColor, scan);
        }
    } break;

    case StyleKey::None:
        if (!result) result = std::make_shared<Style>();
        result->setStrokeColor(colors::NoColor);
        return true;

    case StyleKey::Rgb:
    case StyleKey::Rgba: {
        PackedColor color;
        if (parseRgbaColor(color, colors::G0, scan, end)) {
            if (!result) result = std::make_shared<Style>();
            result->setStrokeColor(color);
            return true;
        } else if (error_context) {
            error_context->setErrorText(ErrorCode::InvalidColor, scan);
        }
    } break;
    }

    *end = scan;
    result = nullptr;
    assert(!error_context || (ErrorCode::NoError != error_context->error));
    return false;
}

const char* parse_style(const char *scan, std::shared_ptr<Style>& result, ErrorContext* error_context)
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
            case StyleKey::Hex:
            case StyleKey::Hsl:
            case StyleKey::Hsla:
            case StyleKey::None:
            case StyleKey::Rgb:
            case StyleKey::Rgba:
                if (parse_fill(scan, style, &end, error_context)) {
                    scan = end;
                } else {
                    assert(!error_context || (ErrorCode::NoError != error_context->error));
                    result = nullptr;
                    return scan;
                }
                break;

            case StyleKey::Opacity: {
                scan = end;
                float f = parse_float(scan, &end);
                if (_isnanf(f)) {
                    if (error_context) {
                        error_context->setErrorText(ErrorCode::ExpectedFloat, scan);
                    }
                    result = nullptr;
                    return scan;
                }
                if (!style) style = std::make_shared<Style>();
                style->setOpacity(f);
                result = style;
                scan = end;
            } break;

            case StyleKey::Stroke:
                scan = end;
                if (parse_stroke(scan, style, &end, error_context)) {
                    scan = end;
                } else {
                    assert(!error_context || (ErrorCode::NoError != error_context->error));
                    result = nullptr;
                    return scan;
                }
                break;

            case StyleKey::Width: {
                scan = end;
                float f = parse_float(scan, &end);
                if (_isnanf(f)) {
                    if (error_context) {
                        error_context->setErrorText(ErrorCode::ExpectedFloat, scan);
                    }
                    result = nullptr;
                    return scan;
                }
                if (!style) style = std::make_shared<Style>();
                style->setStrokeWidth(f);
                scan = end;
            } break;

            case StyleKey::Index:
            case StyleKey::Offset:
                if (error_context) {
                    error_context->setErrorText(ErrorCode::UnexpectedKeyword, scan);
                }
                result = nullptr;
                return scan;

            case StyleKey::Unknown:
            default:
                if (error_context) {
                    error_context->setErrorText(ErrorCode::SyntaxError, scan);
                }
                result = nullptr;
                return scan;
            }
        }
    }
    result = style;
    return scan;
}

std::shared_ptr<SvgTheme> loadSvgThemeFile(std::string path, ErrorContext* error_context)
{
    auto size = system::getFileSize(path);
    if (0 == size) { return nullptr; }

    auto f = std::fopen(path.c_str(), "rb");
    if (!f) {
        if (error_context) {
            error_context->error = ErrorCode::OpenFileError;
            auto file = system::getFilename(path);
            strncpy(error_context->text, file.c_str(), 80);
        }
        return nullptr;
    }
    char* bytes = (char*)malloc(size+1);
    if (!bytes) {
        std::fclose(f);
        if (error_context) {
            error_context->error = ErrorCode::GeneralError;
            error_context->text[0] = 0;
        }
        return nullptr;
    }
    DEFER({free(bytes);});

    auto red = std::fread(bytes, 1, size, f);
    std::fclose(f);
    if (red != size) {
        if (error_context) {
            error_context->error = ErrorCode::ReadFileError;
            error_context->text[0] = 0;
        }
        return nullptr;
    }
    bytes[size] = 0;
    if (error_context) {
        error_context->data = bytes;
    }

    compact_space(bytes);

    auto theme = std::make_shared<SvgTheme>();
    theme->file = path;

    const char * scan = bytes;
    const char * eob = scan + size;
    bool first = true;
    while (*scan && (scan < eob)) {
        while (std::isspace(*scan)) scan++;
        if (!*scan) break;
        std::string name;
        scan = parse_name(scan, name);
        if (name.empty()) {
            if (error_context) {
                error_context->setErrorText(ExpectedName, scan);
                error_context->sanitize();
            }
            return nullptr;
        }
        if (first && name == "@theme") {
            scan = parse_theme_name(scan, name);
            if (!name.empty()) {
                theme->name = name;
            }
        } else {
            std::shared_ptr<Style> style{nullptr};
            scan = parse_style(scan, style, error_context);
            if (style) {
                theme->styles[name] = style;
            } else {
                if (error_context) {
                    assert(error_context->error != ErrorCode::NoError); // failed without registering an error
                    error_context->sanitize();
                }
                return nullptr;
            }
        }
        first = false;
    }

    if (theme->name.empty()) {
        theme->name = system::getStem(path);
    }
    if (error_context) {
        error_context->sanitize();
    }
    return theme;
}

}