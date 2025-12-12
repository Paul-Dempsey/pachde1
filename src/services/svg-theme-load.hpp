#include "svg-theme.hpp"

namespace svg_theme {

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
    char theme_name[64]{0};
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