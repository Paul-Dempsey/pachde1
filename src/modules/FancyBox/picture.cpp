#include "picture.hpp"
#include <stb_image.h>
#include "services/text.hpp"
#include "widgets/color-widgets.hpp"

namespace widgetry {

bool Picture::open() {
    close();
    last_path = options->path;
    last_gray = options->gray;
    if (last_path.empty()) {
        ok = true;
    } else {
        image_data = stbi_load(options->path.c_str(), &image_width, &image_height, &image_components, 4);
        if (image_data) {
            if (options->gray) {
                black_and_white();
            }
            ok = true;
        } else {
            image_width = image_height = 0;
            fail_reason = stbi_failure_reason();
            ok = false;
        }
    }
    return ok;
}

void Picture::close() {
    ok = true;
    last_path = options->path;
    last_gray = options->gray;
    fail_reason = "";
    if (image_data) {
        auto mem = image_data;
        image_data = nullptr;
        stbi_image_free(mem);
    }

    auto window = APP->window;
    if (window && window->vg) {
        clearImageCache(window->vg);
    } else {
        // leak handle
        image_handle = 0;
    }
}

constexpr const float b2f_factor {1./255.f};
inline float LuminanceLinear(uint8_t r, uint8_t g, uint8_t b) {
    return (0.2126f * r * b2f_factor) + (0.7152f * g * b2f_factor) + (0.0722f * b * b2f_factor);
}

void Picture::black_and_white()
{
    if (!image_data) return;

    uint8_t* scan = image_data;
    for (int i = 0; i < image_width * image_height; i++) {
        uint8_t* poke = scan;
        auto r = *scan++;
        auto g = *scan++;
        auto b = *scan++;
        scan++;
        uint8_t l = static_cast<uint8_t>(255.f * LuminanceLinear(r,g,b));
        *poke++ = l;
        *poke++ = l;
        *poke = l;
    }
}

void Picture::clearImageCache(NVGcontext* vg) {
    if (image_handle) {
        nvgDeleteImage(vg, image_handle);
        image_handle = 0;
    }
}

void Picture::updateImageCache(NVGcontext* vg) {
    if (!image_data) {
        clearImageCache(vg);
        return;
    }
    if (!image_cookie && !image_handle) {
        image_handle = nvgCreateImageRGBA(vg, width(),  height(), 0, image_data);
        image_cookie = reinterpret_cast<intptr_t>(image_data);
    } else {
        auto cookie = reinterpret_cast<intptr_t>(image_data);
        if (cookie != image_cookie) {
            if (image_handle) {
                nvgDeleteImage(vg, image_handle);
                image_handle = nvgCreateImageRGBA(vg, width(),  height(), 0, image_data);
            }
            image_cookie = image_handle ? cookie : 0;
        }
    }
}

void Picture::step() {
    if ((last_gray != options->gray) || (last_path != options->path)) {
        open();
    }
}

void Picture::draw(const DrawArgs &args) {
    updateImageCache(args.vg);
    if (image_handle) {
        auto vg = args.vg;
        float scale;
        float x_scale = box.size.x / image_width;
        float y_scale = box.size.y / image_height;
        switch (options->fit) {
            case ImageFit::Fit:
                scale = std::min(x_scale, y_scale);
                x_scale = y_scale = scale;
                break;
            case ImageFit::Stretch:
                break;
            case ImageFit::Cover:
                scale = std::max(x_scale, y_scale);
                x_scale = y_scale = scale;
                break;
            case ImageFit::Scale:
                scale = options->scale;
                x_scale = y_scale = scale;
                break;
        }

        float x_offset = options->x_offset * image_width;
        float y_offset = options->y_offset * image_height;
        float x = x_offset + (box.size.x/x_scale - image_width)/2.f;
        float y = y_offset + (box.size.y/y_scale - image_height)/2.f;
        nvgSave(vg);
        nvgScissor(vg, 0, 0, box.size.x, box.size.y);
        nvgBeginPath(vg);
        nvgScale(vg, x_scale, y_scale);
        nvgRect(vg, x, y, image_width, image_height);
        NVGpaint paint = nvgImagePattern(vg, x, y, image_width, image_height, 0.f, image_handle, 1.f);
        nvgFillPaint(vg, paint);
        nvgFill(vg);
        nvgResetScissor(vg);
        nvgRestore(vg);
    } else if (ok) {
        drawCheckers(args, 0, 0, box.size.x, box.size.y, BLACK);
    } else {
        auto font = GetPluginFontSemiBold();
        if (!FontOk(font)) {
            drawCheckers(args, 0, 0, box.size.x, box.size.y, nvgRGB(255, 50, 50));
            return;
        }
        draw_text_box(args.vg, 0, 0, box.size.x, box.size.y,
            0.f, 0.f, 0.f, 0.f,
            fail_reason,
            font, 14.f, 0xff4040ff,
            HAlign::Center, VAlign::Middle
        );
    }
}
}