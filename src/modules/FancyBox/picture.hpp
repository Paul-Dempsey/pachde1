#pragma once
#include <rack.hpp>
using namespace ::rack;

namespace widgetry {
enum class ImageFit { Cover, Fit, Stretch, Scale };

struct PictureOptions {
    std::string path;
    bool gray{false};
    ImageFit fit{ImageFit::Cover};
    float x_offset{0.f};
    float y_offset{0.f};
    float scale{1.f};

    void init(const PictureOptions& source) {
        path = source.path;
        fit = source.fit;
        gray = source.gray;
        x_offset = source.x_offset;
        y_offset = source.y_offset;
        scale = source.scale;
    }
};

struct Picture : OpaqueWidget {
    int image_handle{0}; // nvg Image handle
    int image_width{0};
    int image_height{0};
    int image_components{0};
    PictureOptions* options{nullptr};
    unsigned char * image_data{nullptr};
    intptr_t image_cookie{0};
    std::string fail_reason;
    bool ok{true};
    std::string last_path;
    int last_gray{-1};

    Picture(PictureOptions* data) : options(data) {
        assert(options);
    }
    float width() { return image_width; }
    float height() { return image_height; }
    bool open();
    void close();
    void black_and_white();
    void updateImageCache(NVGcontext* vg);
    void clearImageCache(NVGcontext* vg);
    void step() override;
    void draw(const DrawArgs& args) override;
};

}