#include "info_settings.hpp"
#include "Info.hpp"
#include "services/open-file.hpp"
#include "services/json-help.hpp"

namespace pachde {

void InfoSettings::reset()
{
    theme_panel_color = info_constant::PANEL_DEFAULT;
    theme_text_color = info_constant::TEXT_DEFAULT;
    user_panel_color = colors::NoColor;
    user_text_color = colors::NoColor;
    horizontal_alignment = HAlign::Left;
    vertical_alignment = VAlign::Middle;
    left_margin = right_margin = top_margin = bottom_margin = 0.f;
    brilliant = false;
    branding = true;
    font_size = info_constant::DEFAULT_FONT_SIZE;
    resetFont();
}

void InfoSettings::randomize()
{
    user_panel_color = random::u32();
    user_text_color = packed_color::opaque(random::u32());
    horizontal_alignment = static_cast<HAlign>(random::get<uint32_t>() % 3);
    vertical_alignment = static_cast<VAlign>(random::get<uint32_t>() % 3);
    brilliant = random::get<bool>();
}

float InfoSettings::getFontSize() { return font_size; }
void InfoSettings::setFontSize(float size) { font_size = clamp(size, info_constant::MIN_FONT_SIZE, info_constant::MAX_FONT_SIZE); }
std::shared_ptr<window::Font> InfoSettings::getFont() { return APP->window->loadFont(font_file); }
HAlign InfoSettings::getHorizontalAlignment() { return horizontal_alignment; }
void InfoSettings::setHorizontalAlignment(HAlign h) { horizontal_alignment = h; }
VAlign InfoSettings::getVerticalAlignment() { return vertical_alignment; }
void InfoSettings::setVerticalAlignment(VAlign v) { vertical_alignment = v; }
Orientation InfoSettings::getOrientation() { return orientation; }
void InfoSettings::setOrientation(Orientation orient) { orientation = orient; }
void InfoSettings::setUserPanelColor(PackedColor color) { user_panel_color = color; }
PackedColor InfoSettings::getUserPanelColor() { return user_panel_color; }
void InfoSettings::setUserTextColor(PackedColor color) { user_text_color = color; }
PackedColor InfoSettings::getUserTextColor() {return user_text_color; }
void InfoSettings::setBrilliant(bool brilliance) { brilliant = brilliance; }
bool InfoSettings::getBrilliant() { return brilliant; }
void InfoSettings::setBranding(bool branded) { branding = branded; }
bool InfoSettings::getBranding() { return branding; }
PackedColor InfoSettings::getDisplayPanelColor() { return packed_color::isVisible(user_panel_color) ? user_panel_color : theme_panel_color; }
PackedColor InfoSettings::getDisplayTextColor() { return packed_color::isVisible(user_text_color) ? user_text_color : theme_text_color; }

json_t* InfoSettings::save(json_t* root)
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
    set_json(root, "left-margin", left_margin);
    set_json(root, "right-margin", right_margin);
    set_json(root, "top-margin", top_margin);
    set_json(root, "bottom-margin", bottom_margin);
    std::string align_string{HAlignLetter(horizontal_alignment)};
    set_json(root, "text-align", align_string);
    align_string = {VAlignLetter(vertical_alignment)};
    set_json(root, "text-v-align", align_string);
    set_json(root, "text-orient", OrientationJValue(orientation));
    set_json(root, "font", make_portable_path(font_file));
    set_json(root, "font-folder", make_portable_path(font_folder));
    set_json(root, "bright", brilliant);
    return root;
}

void InfoSettings::load(json_t* root) {
    using namespace info_constant;
    std::string str;
    str = get_json_string(root, "text-background");
    if (!str.empty()) {
        parseHexColor(user_panel_color, PANEL_DEFAULT, str.c_str(), nullptr);
    }
    str = get_json_string(root, "text-color");
    if (!str.empty()) {
        parseHexColor(user_text_color, TEXT_DEFAULT, str.c_str(), nullptr);
    }

    font_size = get_json_float(root, "text-size", DEFAULT_FONT_SIZE);
    if (font_size != DEFAULT_FONT_SIZE) {
        font_size = clamp(font_size, MIN_FONT_SIZE, MAX_FONT_SIZE);
        if (std::isnan(font_size)) {
            font_size = DEFAULT_FONT_SIZE;
        }
    }

    left_margin = clamp(get_json_float(root, "left-margin", left_margin), 0.f, 30.f);
    if (std::isnan(left_margin)) {
        left_margin = 0.f;
    }
    right_margin = clamp(get_json_float(root, "right-margin", right_margin), 0.f, 30.f);
    if (std::isnan(right_margin)) {
        right_margin = 0.f;
    }
    top_margin = clamp(get_json_float(root, "top-margin", top_margin), 0.f, 30.f);
    if (std::isnan(top_margin)) {
        top_margin = 0.f;
    }
    bottom_margin = clamp(get_json_float(root, "bottom-margin", bottom_margin), 0.f, 30.f);
    if (std::isnan(bottom_margin)) {
        bottom_margin = 0.f;
    }

    orientation = parseOrientation(get_json_string(root, "text-orient"));
    horizontal_alignment = parseHAlign(get_json_string(root, "text-align"));

    str = get_json_string(root, "text-v-align");
    vertical_alignment = str.empty() ? VAlign::Middle : parseVAlign(str);

    font_file = get_json_string(root, "font");
    if (!font_file.empty()) font_file = path_from_portable_path(font_file);
    font_folder = get_json_string(root, "font-folder");
    if (!font_folder.empty()) font_folder = path_from_portable_path(font_folder);
    brilliant = get_json_bool(root, "bright", brilliant);
}

void InfoSettings::setTheme(Theme theme) {
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

bool InfoSettings::fontDialog()
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