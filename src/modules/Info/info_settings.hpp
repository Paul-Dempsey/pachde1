#pragma once
#include "myplugin.hpp"
#include "services/theme.hpp"
#include "text_align.hpp"

namespace pachde {
using namespace packed_color;

namespace info_constant {
    using namespace colors;
    constexpr const PackedColor PANEL_DEFAULT{G80};
    constexpr const PackedColor TEXT_DEFAULT{G20};
    constexpr const float DEFAULT_FONT_SIZE{16.f};
    constexpr const float MIN_FONT_SIZE{5.f};
    constexpr const float MAX_FONT_SIZE{60.f};
}

struct InfoSettings
{
    // computed from theme
    PackedColor theme_panel_color{info_constant::PANEL_DEFAULT};
    PackedColor theme_text_color{info_constant::TEXT_DEFAULT};
    // overrides
    PackedColor user_panel_color{colors::NoColor};
    PackedColor user_text_color{colors::NoColor};

    bool brilliant{false};
    bool branding{true};

    HAlign horizontal_alignment{HAlign::Left};
    Orientation orientation{Orientation::Normal};
    float font_size{info_constant::DEFAULT_FONT_SIZE};
    float left_margin{0.f};
    float right_margin{0.f};

    std::string font_file = asset::plugin(pluginInstance, "res/fonts/HankenGrotesk-SemiBold.ttf");
    std::string font_folder{""};

    void reset();
    void randomize();

    json_t* save(json_t* root);
    void load(json_t* root);

    float getFontSize();
    void setFontSize(float size);
    std::shared_ptr<window::Font> getFont();

    HAlign getHorizontalAlignment();
    void setHorizontalAlignment(HAlign h);
    Orientation getOrientation();
    void setOrientation(Orientation orientation);
    PackedColor getDisplayPanelColor();
    PackedColor getDisplayTextColor();

    void setTheme(Theme theme);

    PackedColor getUserPanelColor();
    void setUserPanelColor(PackedColor color);

    PackedColor getUserTextColor();
    void setUserTextColor(PackedColor color);

    void setBrilliant(bool brilliance);
    bool getBrilliant();
    void setBranding(bool branded);
    bool getBranding();

    void resetFont() {
        font_file = asset::plugin(pluginInstance, "res/fonts/HankenGrotesk-SemiBold.ttf");
    }
    bool fontDialog();
};

}