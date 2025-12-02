#include "guide-data.hpp"
#include "services/json-help.hpp"
using namespace packed_color;
namespace pachde {

json_t *GuideLine::toJson() {
    char hex[10];
    json_t* root = json_object();
    set_json(root, "name", name);
    packed_color::hexFormat(color, 10, hex);
    set_json(root, "color", hex);
    set_json(root, "x", origin.x);
    set_json(root, "y", origin.x);
    set_json(root, "width", width);
    set_json(root, "angle", angle);
    set_json(root, "repeat", repeat);
    return root;
}

void GuideLine::fromJson(json_t *root) {
    name = get_json_string(root, "name", name);
    color = parseColor(get_json_string(root, "color").c_str(), color);
    origin.x = get_json_float(root, "x", origin.x);
    origin.y = get_json_float(root, "y", origin.y);
    width    = get_json_float(root, "width", width);
    angle    = get_json_float(root, "angle", angle);
    repeat   = get_json_float(root, "repeat", repeat);
}

json_t *GuideData::toJson() {
    char hex[10];
    json_t* root = json_object();
    set_json(root, "position", (position == OverlayPosition::OnPanel) ? "panel" : "widget");
    hexFormat(co_overlay, 10, hex);
    set_json(root, "color", hex);
    auto jar = json_array();
    if (jar) {
        for (auto guide : guides) {
            auto j = guide->toJson();
            if (j) json_array_append_new(jar, j);
        }
        json_object_set_new(root, "guides", jar);
    }
    return root;
}

void GuideData::fromJson(json_t *root) {
    auto spec = get_json_string(root, "position");
    if (!spec.empty()) {
        if (0 == spec.compare("panel")) {
            position = OverlayPosition::OnPanel;
        } else if (0 == spec.compare("widget")) {
            position = OverlayPosition::OnTop;
        } else {
            position = OverlayPosition::OnPanel;
        }
    }
    spec = get_json_string(root, "color");
    if (!spec.empty()) {
        co_overlay = parseColor(spec.c_str(), co_overlay);
    }
    auto jar = json_object_get(root, "guides");
    if (jar) {
        json_t* jp;
        size_t index;
        json_array_foreach(jar, index, jp) {
            auto guide = std::make_shared<GuideLine>();
            guide->fromJson(jp);
            guides.push_back(guide);
        }
    }
}

}