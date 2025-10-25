#include "svg-theme-2.hpp"

namespace svg_theme_2 {

enum ErrorCode {
    NoError,
    SyntaxError,
    ExpectedName,
    ExpectedFloat,
    ExpectedNumber,
    UnexpectedKeyword,
    InvalidColor,
    MixedStopIndices,
    OpenFileError,
    ReadFileError,
    GeneralError
};

const char * errorName(ErrorCode code);

struct ErrorContext {
    ErrorCode error{NoError};
    char text[82]{0};
    int offset{-1};
    int line{0};
    const char *data{nullptr};

    void sanitize() { data = nullptr; }
    void clear();
    void setErrorText(ErrorCode code, const char * where);
    std::string makeErrorReport();
};

std::shared_ptr<SvgTheme> loadSvgThemeFile(std::string path, ErrorContext* optional_error_context);

}