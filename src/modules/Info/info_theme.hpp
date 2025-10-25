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

struct InfoTheme : ThemeBase
{
    // computed from theme
    PackedColor theme_panel_color{info_constant::PANEL_DEFAULT};
    PackedColor theme_text_color{info_constant::TEXT_DEFAULT};
    // overrides
    PackedColor user_panel_color{colors::NoColor};
    PackedColor user_text_color{colors::NoColor};
    HAlign horizontal_alignment{HAlign::Left};

    PackedColor main_color{colors::NoColor};

    bool brilliant{false};

    float font_size{info_constant::DEFAULT_FONT_SIZE};
    std::string font_file = asset::plugin(pluginInstance, "res/fonts/HankenGrotesk-SemiBold.ttf");
    std::string font_folder{""};

    void reset();
    void randomize();

    json_t* save(json_t* root) override;
    void load(json_t* root) override;

    float getFontSize();
    void setFontSize(float size);
    HAlign getHorizontalAlignment();
    void setHorizontalAlignment(HAlign h);
    PackedColor getDisplayMainColor();
    PackedColor getDisplayTextColor();

    void setTheme(Theme theme);

    PackedColor getUserPanelBackground();
    void setUserPanelBackground(PackedColor color);

    PackedColor getUserTextColor();
    void setUserTextColor(PackedColor color);

    void setBrilliant(bool brilliant);
    bool getBrilliant();
    void toggleBrilliant();

    void resetFont() {
        font_file = asset::plugin(pluginInstance, "res/fonts/HankenGrotesk-SemiBold.ttf");
    }
    bool fontDialog();
};

}