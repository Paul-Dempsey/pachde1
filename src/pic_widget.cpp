#include "pic_widget.hpp"
#include "text.hpp"

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
    auto pic = module->getImage();
    if (!pic || !pic->ok()) return;
    float pwidth = pic->width();
    float pheight = pic->height();
    float scale = std::min(box.size.x / pwidth, box.size.y / pheight);
    float width = pwidth * scale;
    float height = pheight * scale;
    float left = (box.size.x - width)/2.0f;
    float top = (box.size.y - height)/2.0f;
    float px = (mousepos.x - left)/scale;
    float py = (mousepos.y - top)/scale;
    if (px >= 0.0f && px < pwidth && py >= 0.0f && py < pheight) {
        module->traversal->set_position(Vec(px, py));
    }
}

void PicWidget::drawPic(const DrawArgs &args)
{
    auto vg = args.vg;
    auto pic = module ? module->getImage() : nullptr;
    if (pic && pic->ok()) {
        auto width = pic->width();
        auto height = pic->height();
        auto icookie = reinterpret_cast<intptr_t>(pic->data());
        auto vcookie = reinterpret_cast<intptr_t>(args.vg);
        if (!image_cookie && !image_handle && !vg_cookie) {
            image_handle = nvgCreateImageRGBA(vg, width, height, 0, pic->data());
            image_cookie = icookie;
            vg_cookie = vcookie;
        } else {
            if (icookie != image_cookie || vcookie != vg_cookie) {
                if (image_handle) {
                    nvgDeleteImage(vg, image_handle);
                    image_handle = 0;
                    image_handle = nvgCreateImageRGBA(vg, width, height, 0, pic->data());
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
        if (image_handle) {
            float scale = std::min(box.size.x / width, box.size.y / height);
            float x = (box.size.x/scale - width)/2.0f;
            float y = (box.size.y/scale - height)/2.0f;

            nvgSave(vg);
            nvgBeginPath(vg);
            nvgScale(vg, scale, scale);
            NVGpaint imgPaint = nvgImagePattern(vg, x, y, width, height, 0, image_handle, 1.0f);
            nvgRect(vg, x, y, width, height);
            nvgFillPaint(vg, imgPaint);
            nvgFill(vg);
            nvgClosePath(vg);
            nvgRestore(vg);
            return;
        }
        FillRect(vg, 0, 0, box.size.x, box.size.y, COLOR_BRAND);
        auto font = GetPluginFontSemiBold();
        if (FontOk(font))
        {
            auto text = !pic || (pic && pic->reason().empty()) ? "[no image]" : pic->reason().c_str();
            SetTextStyle(vg, font, GRAY85, 16);
            CenterText(vg, box.size.x/2, box.size.y/2, text, NULL);
        }
    }
}

void PicWidget::drawSample(const DrawArgs &args) {
    if (!module) return;
    if (module->isBypassed()) return;
    auto pic = module->getImage();
    if (!pic || !pic->ok()) return;

    auto vg = args.vg;

    auto width = pic->width();
    auto height = pic->height();
    float scale = std::min(box.size.x / width, box.size.y / height);
    float x = (box.size.x/scale - width)/2.0f;
    float y = (box.size.y/scale - height)/2.0f;
    auto pos = module->traversal->get_position();
    auto color = module->image.pixel(pos.x, pos.y);
    float cx = x*scale + pos.x*scale;
    float cy = y*scale + pos.y*scale;

    // halo
    if (rack::settings::rackBrightness < 0.98f && rack::settings::haloBrightness > 0.0f) {
        auto haloColor = COLOR_BRAND_HI; //nvgRGB(255,255,255);
        nvgBeginPath(vg);
        nvgRect(vg, cx - 13.0f, cy - 13.0f, 26.0f, 26.0f);
        NVGcolor icol = nvgTransRGBAf(haloColor, rack::settings::haloBrightness);
        NVGcolor ocol = nvgTransRGBAf(haloColor, 0);
        NVGpaint paint = nvgRadialGradient(vg, cx, cy, 5, 13, icol, ocol);
        nvgFillPaint(vg, paint);
        nvgFill(vg);
    }

    // sample
    nvgBeginPath(vg);
    nvgCircle(vg, cx, cy, 3.0);
    nvgFillColor(vg, color);
    nvgFill(vg);

    // outline
    auto lum = LuminanceLinear(color);
    auto outline = lum < 0.5 ? nvgRGB(255,255,255) : nvgRGB(0,0,0);
    nvgBeginPath(vg);
    nvgCircle(vg, cx, cy, 3.0);
    nvgStrokeColor(vg, outline);
    nvgStrokeWidth(vg, 0.25f);
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