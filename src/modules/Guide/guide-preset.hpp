#pragma once
#include <rack.hpp>
using namespace ::rack;
#include "widgets/action-button.hpp"
using namespace widgetry;
namespace guides {

struct TopMarginData {
    static std::string up()   { return asset::plugin(pluginInstance, "res/guides/top-margin.svg"); }
    static std::string down() { return asset::plugin(pluginInstance, "res/guides/top-margin-dn.svg"); }
    static std::string guide() { return asset::plugin(pluginInstance, "res/guides/top-margin.json"); }
    static const char * tooltip() { return "20px top margin"; }
    static const char * key() { return "k:guide-top"; }
};

struct BottomMarginData {
    static std::string up()   { return asset::plugin(pluginInstance, "res/guides/bottom-margin.svg"); }
    static std::string down() { return asset::plugin(pluginInstance, "res/guides/bottom-margin-dn.svg"); }
    static std::string guide() { return asset::plugin(pluginInstance, "res/guides/bottom-margin.json"); }
    static const char * tooltip() { return "1hp bottom margin"; }
    static const char * key() { return "k:guide-bottom"; }
};

struct LeftData {
    static std::string up()   { return asset::plugin(pluginInstance, "res/guides/left.svg"); }
    static std::string down() { return asset::plugin(pluginInstance, "res/guides/left-dn.svg"); }
    static std::string guide() { return asset::plugin(pluginInstance, "res/guides/left.json"); }
    static const char * tooltip() { return "1/2hp left margin"; }
    static const char * key() { return "k:guide-left"; }
};

struct RightData {
    static std::string up()   { return asset::plugin(pluginInstance, "res/guides/right.svg"); }
    static std::string down() { return asset::plugin(pluginInstance, "res/guides/right-dn.svg"); }
    static std::string guide() { return asset::plugin(pluginInstance, "res/guides/right.json"); }
    static const char * tooltip() { return "1/2hp right margin"; }
    static const char * key() { return "k:guide-right"; }
};

struct HorizontalData {
    static std::string up()   { return asset::plugin(pluginInstance, "res/guides/horizontal.svg"); }
    static std::string down() { return asset::plugin(pluginInstance, "res/guides/horizontal-dn.svg"); }
    static std::string guide() { return asset::plugin(pluginInstance, "res/guides/horizontal.json"); }
    static const char * tooltip() { return "1/2hp horizontal rules"; }
    static const char * key() { return "k:guide-horz"; }
};

struct VerticalData {
    static std::string up()   { return asset::plugin(pluginInstance, "res/guides/vertical.svg"); }
    static std::string down() { return asset::plugin(pluginInstance, "res/guides/vertical-dn.svg"); }
    static std::string guide() { return asset::plugin(pluginInstance, "res/guides/vertical.json"); }
    static const char * tooltip() { return "1/2hp vertical rules"; }
    static const char * key() { return "k:guide-vert"; }
};

struct GridData {
    static std::string up()   { return asset::plugin(pluginInstance, "res/guides/grid.svg"); }
    static std::string down() { return asset::plugin(pluginInstance, "res/guides/grid-dn.svg"); }
    static std::string guide() { return asset::plugin(pluginInstance, "res/guides/grid.json"); }
    static const char * tooltip() { return "1/2hp ruled grid"; }
    static const char * key() { return "k:guide-grid"; }
};

using TopMarginButton = TActionButton<TopMarginData>;
using BottomMarginButton = TActionButton<BottomMarginData>;
using LeftButton = TActionButton<LeftData>;
using RightButton = TActionButton<RightData>;
using HorizontalButton = TActionButton<HorizontalData>;
using VerticalButton = TActionButton<VerticalData>;
using GridButton = TActionButton<GridData>;

}