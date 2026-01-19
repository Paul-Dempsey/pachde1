#pragma once
#include "myplugin.hpp"
#include "services/text-align.hpp"
#include "services/theme.hpp"

namespace pachde {
using namespace packed_color;

namespace info_constant {
    using namespace colors;
    constexpr const PackedColor PANEL_DEFAULT{G80};
    constexpr const PackedColor TEXT_DEFAULT{G20};
    constexpr const float DEFAULT_FONT_SIZE{16.f};
    constexpr const float MIN_FONT_SIZE{6.f};
    constexpr const float MAX_FONT_SIZE{90.f};
}

inline std::string get_plugin_bold_file() {
    return asset::plugin(pluginInstance, "res/fonts/HankenGrotesk-SemiBold.ttf");
}

struct InfoSettings
{
    // overrides
    PackedColor panel_color{colors::NoColor};
    PackedColor text_color{colors::NoColor};

    bool brilliant{false};
    bool branding{true};

    HAlign horizontal_alignment{HAlign::Left};
    VAlign vertical_alignment{VAlign::Middle};
    Orientation orientation{Orientation::Normal};
    float font_size{info_constant::DEFAULT_FONT_SIZE};
    float left_margin{0.f};
    float right_margin{0.f};
    float top_margin{0.f};
    float bottom_margin{0.f};

    std::string font_file = get_plugin_bold_file();
    std::string font_folder{""};

    void reset();
    void randomize();

    json_t* save(json_t* root);
    void load(json_t* root);

    float getFontSize();
    void setFontSize(float size);
    std::shared_ptr<window::Font> getFont();

    void setHorizontalAlignment(HAlign h);
    void setVerticalAlignment(VAlign v);
    void setOrientation(Orientation orientation);
    HAlign getHorizontalAlignment();
    VAlign getVerticalAlignment();
    Orientation getOrientation();

    void setTheme(Theme theme);

    PackedColor getPanelColor();
    void setPanelColor(PackedColor color);

    PackedColor getTextColor();
    void setTextColor(PackedColor color);

    void setBrilliant(bool brilliance);
    bool getBrilliant();
    void setBranding(bool branded);
    bool getBranding();

    void resetFont() { font_file = get_plugin_bold_file(); }
    bool fontDialog();
};

}