#include "svg-theme.hpp"

namespace svg_theme {

const char * scanTag(const char * id)
{
    if (!*id) return nullptr;
    const char * scan = id;
    const char * last_ddash{nullptr};
    while (*scan) {
        if (('-' == *scan) && ('-' == *(scan+1))) last_ddash = scan;
        scan++;
    }
    if (last_ddash) {
        last_ddash += 2;
        if (*last_ddash) return last_ddash;
    }
    return id;
}

bool applyChildrenTheme(Widget *widget, std::shared_ptr<SvgTheme> theme, bool top)
{
    bool modified = false;

    for (Widget* child : widget->children) {
        if (applyChildrenTheme(child, theme, false)) {
            modified = true;
        }
    }

    auto themed = dynamic_cast<IThemed*>(widget);
    if (themed && themed->applyTheme(theme)) {
        modified = true;
    }

    if (top && modified) {
        sendDirty(widget);
    }

    return modified;
}

void sendDirty(Widget *widget)
{
    if (!widget) return;
	EventContext cDirty;
	Widget::DirtyEvent eDirty;
	eDirty.context = &cDirty;
	widget->onDirty(eDirty);
}

bool alpha_order(const std::string& a, const std::string& b)
{
    if (a.empty()) return false;
    if (b.empty()) return true;
    auto ita = a.cbegin();
    auto itb = b.cbegin();
    for (; (ita != a.cend()) && (itb != b.cend()); ++ita, ++itb) {
        if (*ita == *itb) continue;
        auto c1 = std::tolower(*ita);
        auto c2 = std::tolower(*itb);
        if (c1 == c2) continue;
        if (c1 < c2) return true;
        return false;
    }
    if ((ita == a.cend()) && (itb != b.cend())) {
        return true;
    }
    return false;
}

//
// Paint
//
Paint::~Paint() {
    if (isGradient()) gradient.clear();
}

Paint::Paint(const Paint& source)
{
    switch (source.kind) {
    case PaintKind::Unset: break;
    case PaintKind::Color: setColor(source.color); break;
    case PaintKind::Gradient: setGradient(source.gradient); break;
    case PaintKind::None: setNone(); break;
    }
}

void Paint::Init(const Paint &source)
{
    if (isGradient()) gradient.clear();
    kind = source.kind;
    switch (source.kind) {
    case PaintKind::Unset: break;
    case PaintKind::Color: setColor(source.color); break;
    case PaintKind::Gradient: setGradient(source.gradient); break;
    case PaintKind::None: setNone(); break;
    }
}

void Paint::setColor(PackedColor new_color) {
    if (isGradient()) gradient.clear();
    kind = PaintKind::Color;
    color = new_color;
}

void Paint::setGradient(const Gradient& source) {
    kind = PaintKind::Gradient;
    gradient.clear();
    for (const GradientStop& stop : source.stops) {
        gradient.stops.push_back(stop);
    }
}

void Paint::setNone() {
    if (isGradient()) gradient.clear();
    kind = PaintKind::None;
}

//
// Style
//

void Style::setOpacity(float alpha) {
    opacity = alpha;
    apply_opacity = true;
}
void Style::setStrokeWidth(float width) {
    stroke_width = width;
    apply_stroke_width = true;
}
void Style::setFill(const Paint& paint) {
    fill.Init(paint);
}
void Style::setFillColor(PackedColor color) {
    fill.setColor(color);
}
void Style::setFillGradient(const Gradient& gradient) {
    fill.setGradient(gradient);
}
void Style::setStroke(const Paint& paint) {
    stroke.Init(paint);
}
void Style::setStrokeColor(PackedColor color) {
    stroke.setColor(color);
}
void Style::setStrokeGradient(const Gradient& gradient) {
    stroke.setGradient(gradient);
}
bool Style::isApplyFill() {
    return fill.isApplicable();
}
bool Style::isApplyStroke() {
    return stroke.isApplicable();
}
bool Style::isApplyOpacity() {
    return apply_opacity;
}
bool Style::isApplyStrokeWidth() {
    return apply_stroke_width;
}
PackedColor Style::fill_color() {
    return isApplyFill() ? fill.getColor() : colors::NoColor;
}
PackedColor Style::stroke_color() {
    return isApplyStroke() ? stroke.getColor() : colors::NoColor;
}
PackedColor Style::fillWithOpacity() {
    if (!isApplyFill()) return colors::NoColor;
    return (apply_opacity) ? transparent(fill.getColor(), opacity) : fill.getColor();
}
PackedColor Style::strokeWithOpacity() {
    if (!isApplyStroke()) return colors::NoColor;
    return (apply_opacity) ? transparent(stroke.getColor(), opacity) : stroke.getColor();
}

bool Style::applyPaint(NSVGpaint &target, Paint &source)
{
    assert(source.isApplicable());
    switch (source.Kind()) {
        case PaintKind::None:
            if (target.type != NSVG_PAINT_NONE) {
                if ((target.type == NSVG_PAINT_RADIAL_GRADIENT)
                    || (target.type == NSVG_PAINT_LINEAR_GRADIENT)) {
                    // make gradient transparent
                    for (int i = 0; i < target.gradient->nstops; ++i) {
                        target.gradient->stops[i].color = colors::NoColor;
                    }
                } else {
                    target.type = NSVG_PAINT_NONE;
                }
                return true;
            }
            break;

        case PaintKind::Color: {
                auto source_color = source.getColor();
                if ((target.type != NSVG_PAINT_COLOR) || (target.color != source_color)) {
                    if ((target.type == NSVG_PAINT_RADIAL_GRADIENT)
                        || (target.type == NSVG_PAINT_LINEAR_GRADIENT)) {
                        // make gradient flat
                        for (int i = 0; i < target.gradient->nstops; ++i) {
                            target.gradient->stops[i].color = source_color;
                        }
                        return true;
                    }
                    target.type = NSVG_PAINT_COLOR;
                    target.color = source_color;
                    return true;
                }
            }
            break;

        case PaintKind::Gradient: {
                const Gradient* gradient = source.getGradient();
                if (!gradient) return false; // unexpected - defensive

                if (!((target.type == NSVG_PAINT_RADIAL_GRADIENT)
                    || (target.type == NSVG_PAINT_LINEAR_GRADIENT))) {
                    //if (isLogging()) logWarning(ErrorCode::GradientNotPresent,
                    //    format_string("'%s': Skipping SVG element without a gradient", tag.c_str()));
                    return false;
                }

                bool changed = false;
                for (const GradientStop& stop : gradient->stops) {
                    if (stop.index < target.gradient->nstops) {
                        NSVGgradientStop& target_stop = target.gradient->stops[stop.index];
                        if (stop.hasOffset() && target_stop.offset != stop.offset) {
                            target_stop.offset = stop.offset;
                            changed = true;
                        }
                        if (target_stop.color != stop.color) {
                            target_stop.color = stop.color;
                            changed = true;
                        }
                    }
                }
                return changed;
            }

        default:
            break;
    }
    return false;
}

bool Style::applyFill(NSVGshape *shape)
{
    if (!fill.isApplicable()) return false;
    return applyPaint(shape->fill, fill);
}

bool Style::applyStroke(NSVGshape *shape)
{
    if (!stroke.isApplicable()) return false;
    return applyPaint(shape->stroke, stroke);
}

bool Style::applyOpacity(NSVGshape *shape)
{
    if (isApplyOpacity() && (shape->opacity != opacity)) {
        shape->opacity = opacity;
        return true;
    }
    return false;
}

bool Style::applyStrokeWidth(NSVGshape *shape)
{
    if (isApplyStrokeWidth() && (shape->strokeWidth != stroke_width)) {
        shape->strokeWidth = stroke_width;
        return true;
    }
    return false;
}

//
// SvgTheme
//

std::shared_ptr<Style> SvgTheme::getStyle(const std::string &name)
{
    auto found = styles.find(name);
    return (found == styles.end()) ? nullptr : found->second;
}

bool SvgTheme::getGradient(const Gradient** result, const char *name)
{
    if (!result) return false;
    auto style = getStyle(name);
    if (style && style->isApplyFill() && style->fill.isGradient()) {
        *result = style->fill.getGradient();
        return true;
    }
    return false;
}

bool SvgTheme::getFillColor(PackedColor& result, const char *name, bool with_opacity)
{
    auto style = getStyle(name);
    if (style && style->isApplyFill() && style->fill.isColor()) {
        result = with_opacity ? style->fillWithOpacity() : style->fill_color();
        return true;
    }
    return false;
}

bool SvgTheme::getStroke(PackedColor& result, const char *name, bool with_opacity, float* width)
{
    auto style = getStyle(name);
    if (style && style->isApplyStroke()) {
        if (width && style->apply_stroke_width) *width = style->stroke_width;
        result = with_opacity ? style->strokeWithOpacity() : style->stroke_color();
        return true;
    }
    return false;
}

//
// Theme application
//

bool applyImageTheme(NSVGimage* svg_handle, std::shared_ptr<SvgTheme> theme)
{
    if (!theme || !svg_handle || !svg_handle->shapes) return false;
    bool modified = false;

    for (NSVGshape* shape = svg_handle->shapes; nullptr != shape; shape = shape->next) {
        const char * tag = scanTag(shape->id);
        if (!tag || !*tag) continue;
        auto style = theme->getStyle(tag);
        if (!style) continue;
        if (style->applyFill(shape)) modified = true;
        if (style->applyStroke(shape)) modified = true;
        if (style->applyOpacity(shape)) modified = true;
        if (style->applyStrokeWidth(shape)) modified = true;
    }
    return modified;
}

//
// SvgNoChache
//

std::shared_ptr<::rack::window::Svg> SvgNoCache::loadSvg(const std::string &filename)
{
    auto svg = std::make_shared<::rack::window::Svg>();
    try {
        svg->loadFile(filename);
    } catch (Exception& e) {
        WARN("%s", e.what());
        svg = nullptr;
    }
    return svg;
}

//
// RackSvgCache
//

std::shared_ptr<::rack::window::Svg> RackSvgCache::loadThemedSvg(const std::string& filename, std::shared_ptr<SvgTheme> theme) {
    auto svg = loadSvg(filename);
    if (svg) applySvgTheme(svg, theme);
    return svg;
}


std::shared_ptr<::rack::window::Svg> RackSvgCache::reloadSvg(const std::string& filename) {
    auto svg = loadSvg(filename);
    if (svg) {
        try {
            svg->loadFile(filename);
        } catch (Exception& e) {
            WARN("%s", e.what());
            return nullptr;
        }
    }
    return svg;
}

std::shared_ptr<::rack::window::Svg> RackSvgCache::reloadThemedSvg(const std::string& filename, std::shared_ptr<SvgTheme> theme) {
    auto svg = loadSvg(filename);
    if (svg) {
        try {
            svg->loadFile(filename);
        } catch (Exception& e) {
            WARN("%s", e.what());
            return nullptr;
        }
        applySvgTheme(svg, theme);
    }
    return svg;
}

//
// SvgCache
//
std::shared_ptr<::rack::window::Svg> SvgCache::loadSvg(const std::string& filename)
{
	const auto& pair = svgs.find(filename);
	if (pair != svgs.end()) {
		return pair->second;
    }

    // Load svg
	try {
		auto svg = std::make_shared<::rack::window::Svg>();
		svg->loadFile(filename);
        svgs.insert(std::make_pair(filename, svg));
        return svg;
	}
	catch (rack::Exception& e) {
		WARN("%s", e.what());
		return nullptr;
	}
}

void SvgCache::changeTheme(std::shared_ptr<SvgTheme> theme)
{
    applied_theme = theme;
    if (!theme) return;
    for (auto entry : svgs) {
        if (entry.second) {
            applySvgTheme(entry.second, theme);
        }
    }
}

bool SvgCache::reload(const std::string &filename)
{
	const auto& pair = svgs.find(filename);
	if (pair == svgs.end()) {
        WARN("SvgCache::reload: File not found: %s", filename.c_str());
        return false;
    } else {
        try {
            pair->second->loadFile(filename);
            if (applied_theme) {
                applySvgTheme(pair->second, applied_theme);
            }
        } catch (Exception& e) {
            WARN("%s", e.what());
            return false;
        }
    }
    return true;
}

bool SvgCache::reloadAll()
{
    bool ok = true;
    for (auto entry : svgs) {
        try {
            entry.second->loadFile(entry.first);
            if (applied_theme) {
                applySvgTheme(entry.second, applied_theme);
            }
        } catch (Exception& e) {
            WARN("%s", e.what());
            ok = false;
        }
    }
    return ok;
}

void SvgCache::showCache()
{
    unsigned int n = 0;
	for (auto entry : svgs) {
		DEBUG("%u %s %p", ++n, entry.first.c_str(), (entry.second).get());
	}
}

void ThemeCache::addTheme(std::shared_ptr<SvgTheme> theme)
{
    if (!theme) return;
    auto it = std::find_if(themes.begin(), themes.end(), [theme](const std::shared_ptr<SvgTheme> item){ return item->name == theme->name; });
 	if (it == themes.end()) {
        themes.push_back(theme);
    } else if (theme != *it) {
        *it = theme;
    }
}

std::shared_ptr<SvgTheme> ThemeCache::getTheme(const std::string& name)
{
    if (name.empty()) return nullptr;
    auto it = std::find_if(themes.cbegin(), themes.cend(), [name](const std::shared_ptr<SvgTheme> item){ return item->name == name; });
 	return (it == themes.cend()) ? nullptr : *it;
}

void ThemeCache::sort()
{
    std::sort(themes.begin(), themes.end(), [](std::shared_ptr<SvgTheme> a, std::shared_ptr<SvgTheme> b){ return alpha_order(a->name, b->name); });
}

// void ThemeCache::appendThemeMenu(Menu *menu, IThemeHolder *holder, bool disable, void *context)
// {
//     for (auto theme : themes) {
//         std::string& name = theme->name;
//         menu->addChild(createCheckMenuItem(
//             name, "",
//             [name, holder, context]() { return 0 == name.compare(holder->getThemeName()); },
//             [name, holder, context]() { holder->setThemeName(name, context); },
//             disable
//         ));
//     }
// }

}