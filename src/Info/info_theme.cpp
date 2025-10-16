#include "Info.hpp"
#include "../services/open-file.hpp"
#include "../services/json-help.hpp"

namespace pachde {

void InfoTheme::reset()
{
    theme_panel_color = RampGray(G_80);
    theme_text_background = RampGray(G_90);
    theme_text_color = RampGray(G_20);
    setScrews(true);
    setMainColor(COLOR_NONE);
    setTheme(DefaultTheme);
    setDarkTheme(Theme::Dark);
    setFollowRack(true);
    setUserTextBackground(COLOR_NONE);
    setUserTextColor(COLOR_NONE);
    setHorizontalAlignment(HAlign::Left);
    setBrilliant(false);
    setFontSize(DEFAULT_FONT_SIZE);
    resetFont();
}

void InfoTheme::randomize()
{
    RandomizeTheme(this, false);
    theme_panel_color = RandomColor();
    theme_text_background = RandomColor();
    theme_text_color = RandomOpaqueColor();

    setUserTextBackground(RandomColor());
    setUserTextColor(RandomOpaqueColor());
    setHorizontalAlignment(static_cast<HAlign>(random::get<uint32_t>() % 3));
    setBrilliant(random::get<bool>());
}

float InfoTheme::getFontSize() { return font_size; }
void InfoTheme::setFontSize(float size) { font_size = clamp(size, MIN_FONT_SIZE, MAX_FONT_SIZE); }
HAlign InfoTheme::getHorizontalAlignment() { return horizontal_alignment; }
void InfoTheme::setHorizontalAlignment(HAlign h) { horizontal_alignment = h; }

NVGcolor InfoTheme::getDisplayMainColor() {
    return isColorTransparent(main_color) ? theme_panel_color : main_color;
}
NVGcolor InfoTheme::getDisplayTextBackground() {
    return isColorTransparent(user_text_background) ? theme_text_background : user_text_background;
}
NVGcolor InfoTheme::getDisplayTextColor() {
    return isColorTransparent(user_text_color) ? theme_text_color : user_text_color;
}

Theme InfoTheme::getTheme() {
    return module_theme ? module_theme->getTheme() : ThemeBase::getTheme();
}
Theme InfoTheme::getDarkTheme() {
    return module_theme ? module_theme->getDarkTheme() : ThemeBase::getDarkTheme();
}
void InfoTheme::setDarkTheme(Theme theme) {
    if (module_theme) { module_theme->setDarkTheme(theme); }
    ThemeBase::setDarkTheme(theme);
}
bool InfoTheme::getFollowRack() {
    return module_theme ? module_theme->getFollowRack() : ThemeBase::getFollowRack();
}
void InfoTheme::setFollowRack(bool follow) {
    if (module_theme) { module_theme->setFollowRack(follow); }
    ThemeBase::setFollowRack(follow);
}
NVGcolor InfoTheme::getMainColor() {
    return module_theme ? module_theme->getMainColor() : ThemeBase::getMainColor();
}
void InfoTheme::setMainColor(NVGcolor color) {
    if (module_theme) { module_theme->setMainColor(color); }
    ThemeBase::setMainColor(color);
}
bool InfoTheme::hasScrews() {
    return module_theme ? module_theme->hasScrews() : ThemeBase::hasScrews();
}
void InfoTheme::setScrews(bool screws) {
    if (module_theme) { module_theme->setScrews(screws); }
    ThemeBase::setScrews(screws);
}

void InfoTheme::setUserTextBackground(NVGcolor color) { user_text_background = color; }
NVGcolor InfoTheme::getUserTextBackground() {return user_text_background;}

void InfoTheme::setUserTextColor(NVGcolor color) { user_text_color = color; }
NVGcolor InfoTheme::getUserTextColor() {return user_text_color; }

void InfoTheme::setBrilliant(bool brilliantness) { brilliant = brilliantness; }
bool InfoTheme::getBrilliant() { return brilliant; }
void InfoTheme::toggleBrilliant() { brilliant = !brilliant; }

json_t* InfoTheme::save(json_t* root)
{
    root = ThemeBase::save(root);

    if (isColorVisible(user_text_background)) {
        auto color_string = rack::color::toHexString(user_text_background);
        set_json(root, "text-background", color_string);
    }
    if (isColorVisible(user_text_color)) {
        auto color_string = rack::color::toHexString(user_text_color);
        set_json(root, "text-color", color_string);
    }
    if (DEFAULT_FONT_SIZE != font_size) {
        set_json(root, "text-size", font_size);
    }
    std::string align_string = { HAlignLetter(horizontal_alignment) };
    set_json(root, "text-align", align_string);
    set_json(root, "font", font_file);
    set_json(root, "font-folder", font_folder);
    set_json(root, "bright", brilliant);
    return root;
}

void InfoTheme::load(json_t* root)
{
    ThemeBase::load(root);

    auto color_string = get_json_string(root, "text-background");
    if (!color_string.empty()) {
        user_text_background = rack::color::fromHexString(color_string);
    }
    color_string = get_json_string(root, "text-color");
    if (!color_string.empty()) {
        user_text_color = rack::color::fromHexString(color_string);
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


void InfoTheme::applyTheme(Theme theme) {
    switch (theme) {
        default:
        case Theme::Unset:
        case Theme::Light:
            theme_panel_color = RampGray(G_80);
            theme_text_background = RampGray(G_90);
            theme_text_color = RampGray(G_20);
            break;
        case Theme::Dark:
            theme_panel_color = RampGray(G_25);
            theme_text_background = RampGray(G_20);
            theme_text_color = RampGray(G_85);
            break;
        case Theme::HighContrast:
            theme_panel_color = RampGray(G_10);
            theme_text_background = RampGray(G_BLACK);
            theme_text_color = RampGray(G_WHITE);
            break;
    };
}

void InfoTheme::setTheme(Theme theme) {
    if (module_theme) module_theme->setTheme(theme);
    ThemeBase::setTheme(theme);
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