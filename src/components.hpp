#pragma once
#include "plugin.hpp"

inline float HpToPix(float hp) { return hp * 15.0; }
inline float PixToHp(float pix){ return pix / 15.0; }

struct ScrewCap : app::SvgScrew {
    ScrewCap() {
        setSvg(Svg::load(asset::plugin(pluginInstance, "res/ScrewCap.svg")));
    }
};


struct ScrewCapMed : rack::app::SvgScrew {
    ScrewCapMed() {
        setSvg(Svg::load(asset::plugin(pluginInstance, "res/ScrewCapMed.svg")));
    }
};

struct ScrewCapDark : rack::app::SvgScrew {
    ScrewCapDark() {
        setSvg(Svg::load(asset::plugin(pluginInstance, "res/ScrewCapDark.svg")));
    }
};

struct LogoWidget : rack::SvgWidget {
    LogoWidget() {
        setSvg(Svg::load(asset::plugin(pluginInstance, "res/Logo.svg")));
	}
};

struct LogoWidgetBright : rack::SvgWidget {
    LogoWidgetBright() {
        setSvg(Svg::load(asset::plugin(pluginInstance, "res/LogoBright.svg")));
	}
};

struct InfoWidgetDark : rack::SvgWidget {
    InfoWidgetDark() {
        setSvg(Svg::load(asset::plugin(pluginInstance, "res/InfoDark.svg")));
	}
};

struct InfoWidgetBright : rack::SvgWidget {
    InfoWidgetBright() {
        setSvg(Svg::load(asset::plugin(pluginInstance, "res/InfoBright.svg")));
	}
};

struct NullWidgetDark : rack::SvgWidget {
    NullWidgetDark() {
        setSvg(Svg::load(asset::plugin(pluginInstance, "res/NullDark.svg")));
	}
};

struct NullWidgetBright : rack::SvgWidget {
    NullWidgetBright() {
        setSvg(Svg::load(asset::plugin(pluginInstance, "res/NullBright.svg")));
	}
};
