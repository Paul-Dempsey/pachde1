#include "pic_widget.hpp"
#include "../text.hpp"
#include "imagine_layout.hpp"

namespace pachde {

PicWidget::PicWidget(Imagine *module) : module(module)
{
    mousepos = Vec(-1.0f,-1.0f);
}

void PicWidget::onButton(const event::Button& e)
{
    if (!(e.action == GLFW_PRESS && e.button == GLFW_MOUSE_BUTTON_LEFT && (e.mods & RACK_MOD_MASK) == 0)) {
        return;
    }
    e.consume(this);
    mousepos = e.pos;
    updateClient();
}

void PicWidget::onDragMove(const event::DragMove& e)
{
    auto z = APP->scene->rackScroll->zoomWidget->zoom;
    mousepos.x += e.mouseDelta.x / z;
    mousepos.y += e.mouseDelta.y / z;
    updateClient();
}

void PicWidget::updateClient()
{
    if (!module) return;
    float pwidth, pheight, scale, width, height, left, top, px, py;
    auto pic = module->getImage();
    bool havePic = pic && pic->ok();

    pwidth = havePic ? pic->width() : PANEL_IMAGE_WIDTH;
    pheight = havePic ? pic->height() : PANEL_IMAGE_HEIGHT;
    scale = std::min(box.size.x / pwidth, box.size.y / pheight);
    width = pwidth * scale;
    height = pheight * scale;
    left = (box.size.x - width)/2.0f;
    top = (box.size.y - height)/2.0f;
    px = (mousepos.x - left)/scale;
    py = (mousepos.y - top)/scale;
    if (px >= 0.0f && px < pwidth && py >= 0.0f && py < pheight) {
        module->traversal->set_position(Vec(px, py));
    }
}

void PicWidget::clearImageCache()
{
    if (vg_cookie && image_handle) {
        auto vg = reinterpret_cast<NVGcontext*>(vg_cookie);
        nvgDeleteImage(vg, image_handle);
    }
    image_handle = 0;
    vg_cookie = 0;
}

void PicWidget::updateImageCache(NVGcontext* vg, Pic* pic)
{
    if (!pic) {
        clearImageCache();
        return;
    }
    auto icookie = reinterpret_cast<intptr_t>(pic->data());
    auto vcookie = reinterpret_cast<intptr_t>(vg);
    if (!image_cookie && !image_handle && !vg_cookie) {
        image_handle = nvgCreateImageRGBA(vg, pic->width(),  pic->height(), 0, pic->data());
        image_cookie = icookie;
        vg_cookie = vcookie;
    } else {
        if (icookie != image_cookie || vcookie != vg_cookie) {
            if (image_handle) {
                nvgDeleteImage(vg, image_handle);
                image_handle = 0;
                image_handle = nvgCreateImageRGBA(vg, pic->width(),  pic->height(), 0, pic->data());
            }
            if (image_handle) {
                image_cookie = icookie;
                vg_cookie = vcookie;
            } else {
                image_cookie = 0;
                vg_cookie = 0;
            }
        }
    }
}

void PicWidget::drawPic(const DrawArgs &args)
{
    auto vg = args.vg;
    auto pic = module ? module->getImage() : nullptr;
    if (pic && !pic->ok()) {
        pic = nullptr;
    }
    // experiment
    // if (pic && !pic->ok()) {
    //     if (!spectrum)
    //     {
    //         spectrum = CreateHSLSpectrum(.6f);
    //     }
    //     pic = spectrum;
    // }
    if (pic) {
        auto width = pic->width();
        auto height = pic->height();
        updateImageCache(vg, pic);

        if (image_handle) {
            float scale = std::min(box.size.x / width, box.size.y / height);
            float x = (box.size.x/scale - width)/2.f;
            float y = (box.size.y/scale - height)/2.f;

            nvgSave(vg);
            nvgBeginPath(vg);
            nvgScale(vg, scale, scale);
            NVGpaint imgPaint = nvgImagePattern(vg, x, y, width, height, 0.f, image_handle, 1.f);
            nvgRect(vg, x, y, width, height);
            nvgFillPaint(vg, imgPaint);
            nvgFill(vg);
            nvgClosePath(vg);
            nvgRestore(vg);
            return;
        }
    }
    // no pic or bad pic
    clearImageCache();

    auto background = COLOR_BRAND;
    FillRect(vg, 0, 0, box.size.x, box.size.y, background);
    auto font = GetPluginFontSemiBold();
    if (FontOk(font))
    {
        auto color = RampGray(G_85);
        auto text = !pic || (pic && pic->reason().empty()) ? "[no image]" : pic->reason().c_str();

        if (module && module->isXYPad()) {
            text = "[ xy pad ]";
            if (module->isBipolar())
            {
                Line(vg, 0., box.size.y/2., box.size.x, box.size.y/2., color, .5f);
                Line(vg, box.size.x/2., 0, box.size.x/2., box.size.y, color, .5f);
            }
            FillRect(vg, box.size.x/2 - 2, box.size.y - box.size.y/8.f - 12.f, 4.f, 20.f, background);
            SetTextStyle(vg, font, color, 16.f);
            CenterText(vg, box.size.x/2.f, box.size.y - box.size.y/8.f, text, nullptr);
        } else {
            SetTextStyle(vg, font, color, 16.f);
            CenterText(vg, box.size.x/2.f, box.size.y/2.f, text, nullptr);
        }
    }
}

void PicWidget::drawSample(const DrawArgs &args) {
    if (!module) return;
    if (module->isBypassed()) return;
    auto pic = module->getImage();
    if (!pic->ok()) {
        pic = nullptr;
    }
    // experiment
    // if (!pic && spectrum) {
    //     pic = spectrum;
    // }
    bool is_xypad = !pic && module->isXYPad();
    if (!pic && !is_xypad) return;
    auto vg = args.vg;

    auto pos = module->traversal->get_position();

    float width = is_xypad ? PANEL_IMAGE_WIDTH : pic->width();
    float height = is_xypad ? PANEL_IMAGE_HEIGHT : pic->height();
    float scale = std::min(box.size.x / width, box.size.y / height);
    float x = (box.size.x/scale - width)/2.f;
    float y = (box.size.y/scale - height)/2.f;
    auto color = is_xypad
        ? nvgHSL(pos.x / width, .6f, pos.y / height)
        : module->image.pixel(pos.x, pos.y);
    float cx = x*scale + pos.x*scale;
    float cy = y*scale + pos.y*scale;

    // halo
    if (rack::settings::rackBrightness < 0.98f && rack::settings::haloBrightness > 0.f) {
        auto haloColor = COLOR_BRAND_HI; //nvgRGB(255,255,255);
        nvgBeginPath(vg);
        nvgRect(vg, cx - 15.f, cy - 15.f, 30.f, 30.f);
        NVGcolor icol = nvgTransRGBAf(haloColor, rack::settings::haloBrightness);
        NVGcolor ocol = nvgTransRGBAf(haloColor, 0.f);
        NVGpaint paint = nvgRadialGradient(vg, cx, cy, 6.f, 15.f, icol, ocol);
        nvgFillPaint(vg, paint);
        nvgFill(vg);
    }

    // sample
    nvgBeginPath(vg);
    nvgCircle(vg, cx, cy, 6.f);
    nvgFillColor(vg, color);
    nvgFill(vg);

    // outline
    auto lum = LuminanceLinear(color);
    auto outline = lum < 0.5f ? nvgRGBf(1.f,1.f,1.f) : nvgRGBf(0.f,0.f,0.f);
    nvgBeginPath(vg);
    nvgCircle(vg, cx, cy, 6.25f);
    nvgStrokeColor(vg, outline);
    nvgStrokeWidth(vg, 1.f);
    nvgStroke(vg);
}

void PicWidget::drawLayer(const DrawArgs &args, int layer)
{
    OpaqueWidget::drawLayer(args, layer);
    if (1 != layer || !module) return;
    if (module->bright_image) {
        drawPic(args);
    }
    drawSample(args);
}

void PicWidget::draw(const DrawArgs &args)
{
    if (!module || !module->bright_image) {
        drawPic(args);
    }
    OpaqueWidget::draw(args);
}

}