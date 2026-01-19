#pragma once
#include <rack.hpp>
using namespace rack;
#include "services/colors.hpp"
using namespace pachde;
namespace widgetry {

struct SkiffOptions {
    float edge_width{.75f};
    float bezel_width{3.f};
    PackedColor edge_color{colors::Black};
    PackedColor bezel_color{colors::G40};
    PackedColor inside_color{colors::G40};
    int separation{2}; // module separation in hp to start a new skiff box
    bool shadow{true};
    bool floating_info{true};

    void init(const SkiffOptions& options) {
        edge_width = options.edge_width;
        bezel_width = options.bezel_width;
        edge_color = options.edge_color;
        bezel_color = options.bezel_color;
        shadow = options.shadow;
        floating_info = options.floating_info;
    }
};

struct SkiffBox: TransparentWidget {
    using Base = TransparentWidget;

    SkiffOptions* options{nullptr};

    std::vector<Rect> boxes;
    SkiffBox(SkiffOptions* data) : options(data) {
        assert(options);
    }

    void make_skiff_boxes();

    void draw_shadow(NVGcontext *vg, const Rect& base_box);
    void draw(const DrawArgs& args) override;
};

}