#pragma once
#include <rack.hpp>
using namespace ::rack;
#include <unordered_map>
#include "packed-color.hpp"
using namespace ::packed_color;

namespace svg_theme_2 {

const char *scanTag(const char * id);

const float NoOffset = std::nanf("");

struct GradientStop {
    int index{-1};
    PackedColor color{colors::NoColor};
    float offset{NoOffset};

    void reset() { index = -1; color = colors::NoColor; offset = NoOffset; }
    bool hasIndex() const { return index >= 0; }
    bool hasOffset() const { return !std::isnan(offset); }
};

struct Gradient {
    std::vector<GradientStop> stops;
    size_t stopCount() { return stops.size(); }
    void clear() { stops.clear(); }
};

enum class PaintKind { Unset, Color, Gradient, None };

class Paint {
    PaintKind kind{PaintKind::Unset};
    union {
        PackedColor color;
        Gradient gradient;
    };
public:
    Paint() {}
    ~Paint();
    Paint& operator=(const Paint& source);
    Paint(const Paint& source);
    Paint(PackedColor color) { setColor(color); }
    Paint(const Gradient& gradient) { setGradient(gradient); }

    PaintKind Kind() { return kind; }
    void setColor(PackedColor new_color);
    void setGradient(const Gradient& g);
    void setNone();
    bool isColor()    { return kind == PaintKind::Color; }
    bool isGradient() { return kind == PaintKind::Gradient; }
    bool isNone()     { return kind == PaintKind::None; }
    PackedColor getColor() { return isColor() ? color : colors::NoColor; }
    const Gradient* getGradient() { return isGradient() ? &gradient : nullptr; }
    bool isApplicable() { return kind != PaintKind::Unset; }
};

struct Style
{
    Paint fill;
    Paint stroke;
	float opacity{1.f};
	float stroke_width{1.f};
    bool apply_stroke_width{false};
    bool apply_opacity{false};

    void setFill(const Paint& paint);
    void setFillColor(PackedColor color);
    void setFillGradient(const Gradient& gradient);
    void setStroke(const Paint& paint);
    void setStrokeColor(PackedColor color);
    void setStrokeGradient(const Gradient& gradient);
    void setOpacity(float alpha);
    void setStrokeWidth(float width);

    bool isApplyFill();
    bool isApplyStroke();
    bool isApplyOpacity();
    bool isApplyStrokeWidth();
    PackedColor fill_color();
    PackedColor stroke_color();
    PackedColor fillWithOpacity();
    PackedColor strokeWithOpacity();

    bool applyPaint(NSVGpaint& target, Paint& source);
    bool applyFill(NSVGshape* shape);
    bool applyStroke(NSVGshape* shape);
    bool applyOpacity(NSVGshape *shape);
    bool applyStrokeWidth(NSVGshape *shape);
};

struct SvgTheme
{
    std::string name;
    std::string file;
    std::unordered_map<std::string, std::shared_ptr<Style>> styles;

    std::shared_ptr<Style> getStyle(const std::string &name);
    bool getGradient(const Gradient** result, const char *name);
    bool getFillColor(PackedColor& result, const char *name, bool with_opacity);
    bool getStroke(PackedColor& result, const char *name, bool with_opacity, float* width);
};

bool applyImageTheme(NSVGimage* svg_handle, std::shared_ptr<SvgTheme> theme);

inline bool applySvgTheme(std::shared_ptr<::rack::window::Svg> svg, std::shared_ptr<SvgTheme> theme) {
    return (theme && svg) ? applyImageTheme(svg->handle, theme) : false;
}

// // Implement IThemeHolder to enable the appendThemeMenu helper
// // This is usually most conveniently implemented by your module widget.
// struct IThemeHolder
// {
//     virtual const std::string& getThemeName() = 0;
//     virtual void setThemeName(const std::string& theme_name, void *context) = 0;
// };

struct ThemeCache
{
    std::vector<std::shared_ptr<SvgTheme>> themes;

    // updating add (for hot-reload)
    void addTheme(std::shared_ptr<SvgTheme> theme);

    std::shared_ptr<SvgTheme> getTheme(const std::string& name);

    // sort themes in ascending alphabetical order
    void sort();

    void clear() { themes.clear(); }

    // Appends a theme menu that offers a "Theme" option submenu with a
    // list of the themes present in the cache.
    // Call from your module widget's appendContextMenu override.
    // Your IThemeHolder::applyTheme(std::string theme) override should update
    // the themes of visible widgets, and remember the name.
    //
//    void appendThemeMenu(Menu* menu, IThemeHolder* holder, bool disable = false, void* context = nullptr);
};

struct ILoadSvg
{
    virtual std::shared_ptr<::rack::window::Svg> loadSvg(const std::string& filename) = 0;
};

// Uncached Svg loader
struct SvgNoCache  : ILoadSvg
{
    std::shared_ptr<::rack::window::Svg> loadSvg(const std::string& filename) override;
};

// Global Rack SVG cache
struct RackSvgCache : ILoadSvg
{
    // straight wrapper for ::rack::window::Svg::load
    std::shared_ptr<::rack::window::Svg> loadSvg(const std::string& filename) override {
        return ::rack::window::Svg::load(filename);
    }
    // load an Svg and apply a theme
    std::shared_ptr<::rack::window::Svg> loadThemedSvg(const std::string& filename, std::shared_ptr<SvgTheme> theme);
    // "hot-reload" an Svg
    std::shared_ptr<::rack::window::Svg> reloadSvg(const std::string& filename);
    // "hot-reload" a themed Svg
    std::shared_ptr<::rack::window::Svg> reloadThemedSvg(const std::string& filename, std::shared_ptr<SvgTheme> theme);
};

// A local SVG cache can be used for a scope smaller than Rack-wide.
// The cache can be per-plugin or per-module.
struct SvgCache: ILoadSvg
{
    std::map<std::string, std::shared_ptr<::rack::window::Svg>> svgs;
    std::shared_ptr<SvgTheme> applied_theme{nullptr};

    // Load a shared Svg
    std::shared_ptr<::rack::window::Svg> loadSvg(const std::string& filename) override;

    // Apply a theme to the svgs in the cache.
    // Svg widgets will require an explicit refresh (marked dirty) because
    // they are normally cached with a framebuffer.
    // Widgets using multiple frames such as a Rack switch or latched button may
    // require logic to reset the frame before the theme change becomes visible.
    void changeTheme(std::shared_ptr<SvgTheme> theme);

    // "hot-reload" one Svg
    bool reload(const std::string& filename);

    // "hot-reload" all Svgs (and re-apply theme)
    bool reloadAll();

    // (debug-only) print the files in the cache to the Rack log
    void showCache();
};


// Implement IThemed for widgets that are drawn instead of Svg-based,
// or need custom update logic on theme changes.
struct IThemed
{
    virtual bool applyTheme(std::shared_ptr<SvgTheme> theme) = 0;
};

// Walks the Widget tree from `widget`, finding widgets
// implementing IThemed, and applies the theme.
// If modifications to widgets are detected, the DirtyEvent is sent down the
// tree so that affected widgets will be redrawn.
//
bool applyChildrenTheme(Widget * widget, std::shared_ptr<SvgTheme> theme, bool top = true);

// send Dirty event to widget
void sendDirty(Widget* widget);

}