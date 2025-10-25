#include "info_theme.hpp"
#include "services/open-file.hpp"
#include "services/json-help.hpp"

namespace pachde {

void InfoTheme::reset()
{
    theme_panel_color = info_constant::PANEL_DEFAULT;
    theme_text_color = info_constant::TEXT_DEFAULT;
    setScrews(true);
    setMainColor(colors::NoColor);
    setThemeSetting(DefaultThemeSetting);
    setUserPanelBackground(colors::NoColor);
    setUserTextColor(colors::NoColor);
    setHorizontalAlignment(HAlign::Left);
    setBrilliant(false);
    setFontSize(info_constant::DEFAULT_FONT_SIZE);
    resetFont();
}

void InfoTheme::randomize()
{
    RandomizeTheme(this, false);

    setUserPanelBackground(random::u32());
    setUserTextColor(packed_color::opaque(random::u32()));
    setHorizontalAlignment(static_cast<HAlign>(random::get<uint32_t>() % 3));
    setBrilliant(random::get<bool>());
}

float InfoTheme::getFontSize() { return font_size; }
void InfoTheme::setFontSize(float size) { font_size = clamp(size, info_constant::MIN_FONT_SIZE, info_constant::MAX_FONT_SIZE); }
HAlign InfoTheme::getHorizontalAlignment() { return horizontal_alignment; }
void InfoTheme::setHorizontalAlignment(HAlign h) { horizontal_alignment = h; }

PackedColor InfoTheme::getDisplayMainColor() {
    return visible(main_color) ? main_color : theme_panel_color;
}
PackedColor InfoTheme::getDisplayTextColor() {
    return visible(user_text_color) ? user_text_color : theme_text_color;
}

void InfoTheme::setUserPanelBackground(PackedColor color) { user_panel_color = color; }
PackedColor InfoTheme::getUserPanelBackground() { return user_panel_color; }

void InfoTheme::setUserTextColor(PackedColor color) { user_text_color = color; }
PackedColor InfoTheme::getUserTextColor() {return user_text_color; }

void InfoTheme::setBrilliant(bool brilliantness) { brilliant = brilliantness; }
bool InfoTheme::getBrilliant() { return brilliant; }
void InfoTheme::toggleBrilliant() { brilliant = !brilliant; }

json_t* InfoTheme::save(json_t* root)
{
    char hex[10];
    if (visible(user_panel_color)) {
        hexFormat(user_panel_color, sizeof(hex), hex);
        set_json(root, "text-background", hex);
    }
    if (visible(user_text_color)) {
        hexFormat(user_text_color, sizeof(hex), hex);
        set_json(root, "text-color", hex);
    }
    if (info_constant::DEFAULT_FONT_SIZE != font_size) {
        set_json(root, "text-size", font_size);
    }
    std::string align_string = { HAlignLetter(horizontal_alignment) };
    set_json(root, "text-align", align_string);
    set_json(root, "font", font_file);
    set_json(root, "font-folder", font_folder);
    set_json(root, "bright", brilliant);
    return root;
}

void InfoTheme::load(json_t* root) {
    ThemeBase::load(root);
    using namespace info_constant;
    auto color_string = get_json_string(root, "text-background");
    if (!color_string.empty()) {
        parseHexColor(user_panel_color, PANEL_DEFAULT, color_string.c_str(), nullptr);
    }
    color_string = get_json_string(root, "text-color");
    if (!color_string.empty()) {
        parseHexColor(user_text_color, TEXT_DEFAULT, color_string.c_str(), nullptr);
    }

    font_size = get_json_float(root, "text-size", DEFAULT_FONT_SIZE);
    if (font_size != DEFAULT_FONT_SIZE) {
        font_size = clamp(font_size, MIN_FONT_SIZE, MAX_FONT_SIZE);
        if (std::isnan(font_size)) {
            font_size = DEFAULT_FONT_SIZE;
        }
    }
    horizontal_alignment = parseHAlign(get_json_string(root, "text-align"));
    font_file = get_json_string(root, "font");
    font_folder= get_json_string(root, "font-folder");
    brilliant = get_json_bool(root, "bright", brilliant);
}


void InfoTheme::setTheme(Theme theme) {
    using namespace colors;
    switch (theme) {
        default:
        case Theme::Unset:
        case Theme::Light:
            theme_panel_color = G80;
            theme_text_color = G20;
            break;
        case Theme::Dark:
            theme_panel_color = G25;
            theme_text_color = G85;
            break;
        case Theme::HighContrast:
            theme_panel_color = G10;
            theme_text_color = White;
            break;
    };
}

bool InfoTheme::fontDialog()
{
    std::string path;
    bool ok = openFileDialog(font_folder, "Fonts (.ttf):ttf;Any (*):*", font_file, path);
    if (ok) {
        font_file = path;
        font_folder = system::getDirectory(path);
    }
    return ok;
}

}