#include "Info.hpp"
#include "../open_file.hpp"

namespace pachde {
    
float InfoTheme::getFontSize()
{
    return font_size;
}
void InfoTheme::setFontSize(float size)
{
    font_size = clamp(size, MIN_FONT_SIZE, MAX_FONT_SIZE);
}
HAlign InfoTheme::getHorizontalAlignment()
{
    return horizontal_alignment;
}
void InfoTheme::setHorizontalAlignment(HAlign h)
{
    horizontal_alignment = h;
}
NVGcolor InfoTheme::getDisplayMainColor()
{
    return isColorTransparent(main_color) ? theme_panel_color : main_color;
}
NVGcolor InfoTheme::getDisplayTextBackground()
{
    return isColorTransparent(user_text_background) ? theme_text_background : user_text_background;
}
NVGcolor InfoTheme::getDisplayTextColor()
{
    return isColorTransparent(user_text_color) ? theme_text_color : user_text_color;
}

Theme InfoTheme::getTheme() //override
{ 
    return module_theme ? module_theme->getTheme() : ThemeBase::getTheme();
}
Theme InfoTheme::getDarkTheme() //override
{
    return module_theme ? module_theme->getDarkTheme() : ThemeBase::getDarkTheme();
}
void InfoTheme::setDarkTheme(Theme theme) //override
{
    if (module_theme) { module_theme->setDarkTheme(theme); } 
    ThemeBase::setDarkTheme(theme);
}
bool InfoTheme::getFollowRack() //override
{
    return module_theme ? module_theme->getFollowRack() : ThemeBase::getFollowRack();
}
void InfoTheme::setFollowRack(bool follow) //override
{
    if (module_theme) { module_theme->setFollowRack(follow); }
    ThemeBase::setFollowRack(follow);
}
NVGcolor InfoTheme::getMainColor() //override
{
    return module_theme ? module_theme->getMainColor() : ThemeBase::getMainColor();
}
void InfoTheme::setMainColor(NVGcolor color) //override
{
    if (module_theme) { module_theme->setMainColor(color); } 
    ThemeBase::setMainColor(color);
}
bool InfoTheme::hasScrews() //override
{
    return module_theme ? module_theme->hasScrews() : ThemeBase::hasScrews();
}
void InfoTheme::setScrews(bool screws) //override
{
    if (module_theme) { module_theme->setScrews(screws); }
    ThemeBase::setScrews(screws);
}

void InfoTheme::setUserTextBackground(NVGcolor color)
{
    user_text_background = color;
}
NVGcolor InfoTheme::getUserTextBackground()
{
    return user_text_background;
}

void InfoTheme::setUserTextColor(NVGcolor color)
{
    user_text_color = color;
}
NVGcolor InfoTheme::getUserTextColor()
{
    return user_text_color;
}

void InfoTheme::setBrilliant(bool brilliantness)
{
    brilliant = brilliantness;
}
bool InfoTheme::getBrilliant()
{
    return brilliant;
}
void InfoTheme::toggleBrilliant()
{
    brilliant = !brilliant;
}


json_t* InfoTheme::save(json_t* root)
{
    root = ThemeBase::save(root);

    if (isColorVisible(user_text_background)) {
        auto color_string = rack::color::toHexString(user_text_background);
        json_object_set_new(root, "text-background", json_stringn(color_string.c_str(), color_string.size()));
    }
    if (isColorVisible(user_text_color)) {
        auto color_string = rack::color::toHexString(user_text_color);
        json_object_set_new(root, "text-color", json_stringn(color_string.c_str(), color_string.size()));
    }
    if (DEFAULT_FONT_SIZE != font_size) {
        json_object_set_new(root, "text-size", json_real(font_size));
    }
    std::string align_string = { HAlignLetter(horizontal_alignment) };
    json_object_set_new(root, "text-align", json_stringn(align_string.c_str(), align_string.size()));
    json_object_set_new(root, "font", json_string(font_file.c_str()));
    json_object_set_new(root, "font-folder", json_string(font_folder.c_str()));
    json_object_set_new(root, "bright", json_boolean(brilliant));
    return root;
}

void InfoTheme::load(json_t* root)
{
    ThemeBase::load(root);

    auto j = json_object_get(root, "text-background");
    if (j) {
        auto color_string = json_string_value(j);
        user_text_background = rack::color::fromHexString(color_string);
    }
    j = json_object_get(root, "text-color");
    if (j) {
        auto color_string = json_string_value(j);
        user_text_color = rack::color::fromHexString(color_string);
    }
    j = json_object_get(root, "text-size");
    if (j) {
        font_size = clamp(static_cast<float>(json_real_value(j)), MIN_FONT_SIZE, MAX_FONT_SIZE);
        if (std::isnan(font_size)) {
            font_size = DEFAULT_FONT_SIZE;
        }
    }
    j = json_object_get(root, "text-align");
    if (j) {
        horizontal_alignment = parseHAlign(json_string_value(j));
    }
    j = json_object_get(root, "font");
    if (j) {
        font_file = json_string_value(j);
    }
    j = json_object_get(root, "font-folder");
    if (j) {
        font_folder = json_string_value(j);
    }
    brilliant = GetBool(root, "bright", brilliant);
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

void InfoTheme::setTheme(Theme theme) //override
{
    if (module_theme) module_theme->setTheme(theme);
    ThemeBase::setTheme(theme);
}


bool InfoTheme::fontDialog()
{
    std::string path;
    bool ok = openFileDialog(font_folder, "Fonts (.ttf):ttf;Any (*):*", font_file, path);
    if (ok) {
        //DEBUG("Selected font (%s)", path.c_str());
        font_file = path;
        font_folder = system::getDirectory(path);
    }
    return ok;
}

}