#pragma once
#include <rack.hpp>
#include "services/packed-color.hpp"

namespace pachde {

enum OverlayPosition { OnPanel, OnTop };

struct GuideLine {
    Vec origin{0.f};
    float angle{0.f};
    float width{7.5};
    packed_color::PackedColor color{0x50e04bcf};
    float repeat{0.f};
    std::string name;
    //json_t * toJson();
    //void fromJson(json_t * root);
};

struct GuideData {
    OverlayPosition position{OverlayPosition::OnPanel};
    packed_color::PackedColor co_overlay{packed_color::packHsla(0.f, 0.f, 1.f, .35f)};
    std::vector<std::shared_ptr<GuideLine>> guides;
    //json_t * toJson();
    //void fromJson(json_t * root);
};


}