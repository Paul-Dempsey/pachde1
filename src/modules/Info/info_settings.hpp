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
    constexpr const float MIN_FONT_SIZE{5.f};
    constexpr const float MAX_FONT_SIZE{60.f};
}

inline std::string get_plugin_bold_file() {
    return asset::plugin(pluginInstance, "res/fonts/HankenGrotesk-SemiBold.ttf");
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
    VAlign vertical_alignment{VAlign::Middle};
    Orientation orientation{Orientation::Normal};
    float font_size{info_constant::DEFAULT_FONT_SIZE};
    float left_margin{0.f};
    float right_margin{0.f};

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

    void resetFont() { font_file = get_plugin_bold_file(); }
    bool fontDialog();
};

struct FontSizeQuantity : Quantity
{
    InfoSettings* settings{nullptr};
    explicit FontSizeQuantity(InfoSettings* settings) : settings(settings) {}
    void setValue(float value) override { settings->setFontSize(::rack::math::clamp(value, getMinValue(), getMaxValue())); }
    float getValue() override { return settings->getFontSize(); }
    float getMinValue() override { return info_constant::MIN_FONT_SIZE; }
    float getMaxValue() override { return info_constant::MAX_FONT_SIZE; }
    float getDefaultValue() override { return info_constant::DEFAULT_FONT_SIZE; }
    int getDisplayPrecision() override { return 3; }
    std::string getLabel() override { return "Font size"; }
    std::string getUnit() override { return "px"; }
};

struct MarginQuantity : Quantity
{
    float* data{nullptr};
    std::string label;
    explicit MarginQuantity(float* value, const std::string& name) : data(value), label(name) {}
    void setValue(float value) override { *data = ::rack::math::clamp(value, getMinValue(), getMaxValue()); }
    float getValue() override { return *data; }
    float getMinValue() override { return 0.f; }
    float getMaxValue() override { return 30.f; }
    float getDefaultValue() override { return 0.f; }
    int getDisplayPrecision() override { return 3; }
    std::string getLabel() override { return label; }
    std::string getUnit() override { return "px"; }
};


}