#include <rack.hpp>
#include <osdialog.h>
#include "plugin.hpp"
#include "components.hpp"
#include "text.hpp"
#include "dsp.hpp"
#include "themehelpers.hpp"
#include "pic.hpp"
using namespace rack;
using namespace pachde;

const float ONE_HP = 15.0f;
const float PANEL_WIDTH = 300.0f; // 20hp
const float PANEL_CENTER = PANEL_WIDTH / 2.0f;
const float PANEL_MARGIN = 5.0f;

const float IMAGE_UNIT = 18.0f; // 18px image unit for 16x9 landscape
const float PANEL_IMAGE_WIDTH = 16.0f * IMAGE_UNIT;
const float PANEL_IMAGE_HEIGHT = 9.0f * IMAGE_UNIT;
const float PANEL_IMAGE_TOP = 17.0f;
const float SECTION_WIDTH = PANEL_WIDTH - PANEL_MARGIN * 2.0f;
const float ORIGIN_X = PANEL_CENTER;
const float ORIGIN_Y = PANEL_IMAGE_TOP + PANEL_IMAGE_HEIGHT;

enum VRange {
    UNIPOLAR,
    BIPOLAR
};

struct Imagine : ThemeModule
{
    enum ParamIds {
        SLEW_PARAM,
        VOCT_RANGE_PARAM,
        RUN_PARAM,
        NUM_PARAMS
    };
    enum OutputIds {
        X_OUT,
        Y_OUT,
        VOCT_OUT,
        NUM_OUTPUTS
    };

    Pic image;

    bool running = false;
    int scan_x;
    int scan_y;

    SlewLimiter x_slew, y_slew, voct_slew;
    ControlRateTrigger control_rate;
    ControlRateTrigger pix_rate;
    VRange voct_range = VRange::BIPOLAR;

    Imagine() : pix_rate(15.0f) {
        scan_x = 0;
        scan_y = 0;
        config(NUM_PARAMS, 0, NUM_OUTPUTS, 0);
		configParam(SLEW_PARAM, 0.0f, 1.0f, 0.8f, "Slew", "%", 0.0f, 100.0f);
        configSwitch(VOCT_RANGE_PARAM, 0.0f, 1.0f, 0.0f, "V/Oct Polarity", { "Unipolar", "Bipolar"});
        configSwitch(RUN_PARAM, 0.0f, 1.0f, 0.0f, "Play", { "Paused", "Playing" });
        
        configOutput(X_OUT, "x");
        configOutput(Y_OUT, "y");
        configOutput(VOCT_OUT, "V/Oct");
    }

    Pic* getImage() { return &image; }

    void play() { running = true;}

    void pause() { running = false; }

    bool setPlaying(bool play) {
        bool previous = running;
        running = play;
        return previous;
    }
    bool isPlaying() { return running; }

    bool loadImageDialog() {
        scan_x = 0;
        scan_y = 0;
		osdialog_filters* filters = osdialog_filters_parse("Images (.png .jpg .gif):png,jpg,jpeg,gif;Any (*):*");
		DEFER({osdialog_filters_free(filters);});

        std::string dir = asset::user("");
		char* pathC = osdialog_file(OSDIALOG_OPEN, dir.c_str(), NULL, filters);
		if (!pathC) {
            pause();
            image.close();
			return false;
		}
		std::string path = pathC;
		std::free(pathC);
        DEBUG("Selected image (%s)", path.c_str());
        auto run = setPlaying(false);
        if (image.open(path)) {
            setPlaying(run);
            return true;
        } else {
            DEBUG("Image load failed: %s", image.reason().c_str());
            //image.close();
            return false;
        }
    }

    void onSampleRateChange() override {
        control_rate.onSampleRateChanged();
        pix_rate.onSampleRateChanged();
        updateParams();
    }

    json_t *dataToJson() override {
        json_t *rootJ = ThemeModule::dataToJson();

        auto name = image.name();
        if (!name.empty()) {
            json_object_set_new(rootJ, "image", json_stringn(name.c_str(), name.size()));
        }
        return rootJ;
    }

    void dataFromJson(json_t *rootJ) override {
        json_t *textJ = json_object_get(rootJ, "image");
        if (textJ) {
            std::string path = json_string_value(textJ);
            if (image.open(path)) {

            } else {
                DEBUG("Image load failed: %s", image.reason().c_str());
            }
        }
        ThemeModule::dataFromJson(rootJ);
    }

    void updateParams() {
        running = getParam(RUN_PARAM).getValue() > 0.5f;

        float sampleRate = APP->engine->getSampleRate();
        float slew = params[SLEW_PARAM].getValue();
        x_slew.configure(sampleRate, slew, .01f);
        y_slew.configure(sampleRate, slew, .01f);
        voct_slew.configure(sampleRate, slew, .01f);

        voct_range = (params[VOCT_RANGE_PARAM].getValue() < 0.5)
            ? VRange::BIPOLAR
            : VRange::UNIPOLAR;
    }

	void process(const ProcessArgs& args) override {
        if (control_rate.process()) {
            updateParams();
        }

        int width, height;
        if (image.ok()) {
            width = image.width();
            height = image.height();
        } else {
            width = PANEL_IMAGE_WIDTH;
            height = PANEL_IMAGE_HEIGHT;
        }

        auto x = scan_x;
        auto y = scan_y;
        if (outputs[X_OUT].isConnected()) {
            auto v = (float)x / width * 10.0f;
            outputs[X_OUT].setVoltage(x_slew.next(v));
        }

        if (outputs[Y_OUT].isConnected()) {
            auto v = (float)y / height * 10.0f;
            outputs[Y_OUT].setVoltage(y_slew.next(v));
        }

        if (outputs[VOCT_OUT].isConnected()) {
            auto pix = image.pixel(x, y);
            auto lum = LuminanceLinear(pix) * 10.0f;
            if (voct_range == VRange::BIPOLAR) {
                lum -= 5.0;
            } 
            outputs[VOCT_OUT].setVoltage(voct_slew.next(lum));
        }

        // TODO: triggers and gates
        // TODO: progress at scaled param rate, rather than sample rate and interpolate
        // TODO: selectable trajectory
        // scanlines trajectory

        if (running && pix_rate.process()) {
            ++scan_x;
            if (scan_x >= width) {
                scan_x = 0;
                ++scan_y;
                if (scan_y > height) {
                    scan_y = 0;
                }
            }
        }
    }
};


// struct XYPad : OpaqueWidget {
//     ISetPos * receiver;
//     float margin;
//     Vec position;

//     XYPad(ISetPos * host, float inset) : receiver(host), margin(inset) {
//         assert(margin >= 0);
//     }

//     void onButton(const event::Button& e) override {
//         if (!(e.action == GLFW_PRESS && e.button == GLFW_MOUSE_BUTTON_LEFT && (e.mods & RACK_MOD_MASK) == 0)) {
//             return;
//         }
//         e.consume(this);
//         position = e.pos;
//         updateClient();
//     }

//     void onDragMove(const event::DragMove& e) override {
//         auto z = APP->scene->rackScroll->zoomWidget->zoom;
//         position.x += e.mouseDelta.x / z;
//         position.y += e.mouseDelta.y / z;
//         updateClient();
//     }

//     void updateClient()
//     {
//         if (!receiver) return;
//         // compute bipolar -5.0 - 5.0
//         float unit = 2.0f / insideWidth();
//         float x = clamp(position.x, margin, margin + insideWidth());
//         x = 5.0f * ((x - margin) * unit - 1.0f);

//         unit = 2.0f / insideHeight();
//         float y  = clamp(position.y, margin, margin + insideHeight());
//         y = 5.0f * ((y - margin) * unit - 1.0f);
//         receiver->setPosition(new Vec(x, -y));
//     }

//     float insideWidth() { return box.getWidth() - 2.0f * margin; }
//     float insideHeight() { return box.getHeight() - 2.0f * margin; }

//     void draw(const DrawArgs& args) override {
//         auto w = insideWidth();
//         auto h = insideHeight();
//         FillRect(args.vg, margin, margin, w, h, COLOR_BRAND);
//         Line(args.vg, margin + w/2.0f, margin, margin + w/2.0f, margin + h, Overlay(GRAY90), 0.5f);
//         Line(args.vg, margin, margin + h/2.0f, margin + w, margin + h/2.0f, Overlay(GRAY90), 0.5f);
//     }

//     void drawLayer(const DrawArgs& args, int layer) override {
//         if (1 == layer) {
//             auto vg = args.vg;
//             auto x = clamp(position.x, margin, margin + insideWidth());
//             auto y = clamp(position.y, margin, margin + insideHeight());
//             auto color = nvgTransRGBAf(SCHEME_YELLOW, 0.85);
//             if (rack::settings::rackBrightness < 0.98f && rack::settings::haloBrightness > 0.0f) {
//                 nvgBeginPath(vg);
//             	nvgRect(vg, x - 13.0f, y - 13.0f, 26.0f, 26.0f);
//                 NVGcolor icol = nvgTransRGBAf(color, rack::settings::haloBrightness / 2);
// 	            NVGcolor ocol = nvgTransRGBAf(color, 0);
//             	NVGpaint paint = nvgRadialGradient(vg, x, y, 5, 13, icol, ocol);
//                 nvgFillPaint(vg, paint);
//                 nvgFill(vg);
//             }
//             nvgBeginPath(vg);
//             nvgCircle(vg, x, y, 2.0);
//             nvgFillColor(vg, color);
//             nvgFill(vg);
//         }
//     }
// };

struct ImaginePanel : Widget
{
    Theme theme;

    ImaginePanel(Theme t, Vec size)
    {
        theme = ConcreteTheme(t);
        box.size = size;
    }

    void draw(const DrawArgs &args) override
    {
        NVGcolor panel = PanelBackground(theme), textColor = ThemeTextColor(theme);
        auto vg = args.vg;

        // nvgScissor(args.vg, RECT_ARGS(args.clipBox)); // when is scissor managment necessary?

        // panel background
        FillRect(vg, 0.0, 0.0, box.size.x, box.size.y, panel);

        // image placeholder
        //BoxRect(vg, PANEL_CENTER - PANEL_IMAGE_WIDTH / 2.0f, PANEL_IMAGE_TOP, PANEL_IMAGE_WIDTH, PANEL_IMAGE_HEIGHT, COLOR_BRAND);

        //separator
        //Line(vg, PANEL_MARGIN, 196.0f, SECTION_WIDTH, 196.0f, GRAY65);

        // outputs 1
        RoundRect(vg, 5.0f, 299.0f, 290.0f, 65.0f, OutputBackground(theme), 6.0f);

        // title
        auto font = GetPluginFont();
        if (FontOk(font))
        {
            SetTextStyle(vg, font, textColor);
            CenterText(vg, PANEL_CENTER, ONE_HP / 2.0f, "imagine", NULL);
            SetTextStyle(vg, font, GRAY85, 16);
            CenterText(vg, 24.5f, 336.0f, "x", NULL);
            CenterText(vg, 52.5f, 336.0f, "y", NULL);
            CenterText(vg, 82.5f, 336.0f, "slew", NULL);
        }
        Widget::draw(args);
        // nvgResetScissor(args.vg);
    }
};

struct PicWidget : OpaqueWidget {
    Imagine *module = nullptr;
    Vec mousepos;

    PicWidget(Imagine *module) : module(module) {
        mousepos = Vec(-1.0f,-1.0f);
    }

    void onButton(const event::Button& e) override {
        if (!(e.action == GLFW_PRESS && e.button == GLFW_MOUSE_BUTTON_LEFT && (e.mods & RACK_MOD_MASK) == 0)) {
            return;
        }
        e.consume(this);
        mousepos = e.pos;
        updateClient();
    }

    void onDragMove(const event::DragMove& e) override {
        auto z = APP->scene->rackScroll->zoomWidget->zoom;
        mousepos.x += e.mouseDelta.x / z;
        mousepos.y += e.mouseDelta.y / z;
        updateClient();
    }

    void updateClient()
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
            module->scan_x = px;
            module->scan_y = py;
        }
    }

    void draw(const DrawArgs &args) override {
        if (module) {
            auto vg = args.vg;
            auto pic = module->getImage();
            if (pic && pic->ok()) {
                auto width = pic->width();
                auto height = pic->height();
                int handle = nvgCreateImageRGBA(vg, width, height, 0, pic->data());
                if (handle) {
                    float scale = std::min(box.size.x / width, box.size.y / height);
                    float x = (box.size.x/scale - width)/2.0f;
                    float y = (box.size.y/scale - height)/2.0f;

                    nvgSave(vg);
                    nvgBeginPath(vg);
                    nvgScale(vg, scale, scale);
                    NVGpaint imgPaint = nvgImagePattern(vg, x, y, width, height, 0, handle, 1.0f);
                    nvgRect(vg, x, y, width, height);
                    nvgFillPaint(vg, imgPaint);
                    nvgFill(vg);
                    nvgClosePath(vg);
                    nvgRestore(vg);

                    //if (module->isPlaying()) {
                        auto sx = module->scan_x;
                        auto sy = module->scan_y;
                        auto color = module->image.pixel(sx, sy);
                        nvgBeginPath(vg);
                        nvgCircle(vg, x*scale + sx*scale, y*scale + sy*scale, 3.0);
                        nvgFillColor(vg, color);
                        nvgFill(vg);

                        auto lum = LuminanceLinear(color);
                        auto outline = lum < 0.5 ? nvgRGB(255,255,255) : nvgRGB(0,0,0);
                        nvgBeginPath(vg);
                        nvgCircle(vg, x*scale + sx*scale, y*scale + sy*scale, 3.0);
                        nvgStrokeColor(vg, outline);
                        nvgStrokeWidth(vg, 0.25f);
                        nvgStroke(vg);
                    //}

                } else {
                    goto BadImage;
                }
            } else {
BadImage:
                FillRect(vg, 0, 0, box.size.x, box.size.y, COLOR_BRAND);
                auto font = GetPluginFont();
                if (FontOk(font))
                {
                    auto text = pic && pic->reason().empty() ? "[no image]" : pic->reason().c_str();
                    SetTextStyle(vg, font, GRAY85, 16);
                    CenterText(vg, box.size.x/2, box.size.y/2, text, NULL);
                }
            }
        }
        OpaqueWidget::draw(args);
    }
};

struct ImagineUi : ModuleWidget, IChangeTheme
{
    ImaginePanel *panel = nullptr;

    ImagineUi(Imagine *module)
    {
        setModule(module);
        setTheme(ModuleTheme(module));
    }

    void setTheme(Theme theme) override
    {
        auto module = dynamic_cast<Imagine *>(this->module);
        box.size = Vec(300, RACK_GRID_HEIGHT);
        if (children.empty()) {
            panel = new ImaginePanel(theme, box.size);
            setPanel(panel);
            CreateScrews(this, theme, ScrewCap::Brightness::Less);
            addChild(createThemeWidgetCentered<LogoWidget>(theme, Vec(box.size.x / 2.0f, RACK_GRID_HEIGHT - RACK_GRID_WIDTH + 7.5f)));
            addOutput(createThemeOutput<BluePort>(theme, Vec(15.0f, 305.0f), module, Imagine::X_OUT));
            addOutput(createThemeOutput<BluePort>(theme, Vec(40.0f, 305.0f), module, Imagine::Y_OUT));

            addParam(createThemeParamCentered<SmallKnob>(theme, Vec(82.5f, 315.0f), module, Imagine::SLEW_PARAM));
            addOutput(createThemeOutput<BluePort>(theme, Vec(100.0f, 305.0f), module, Imagine::VOCT_OUT));

            addParam(createParam<CKSS>(Vec(125.0f, 305.0f), module, Imagine::VOCT_RANGE_PARAM));

            auto run = createThemeParamCentered<PushButton>(theme, Vec (150.0f, 188.0f), module, Imagine::RUN_PARAM);
            run->momentary = false;
            if (module) {
                run->onClick([module]() {
                    module->setPlaying(!module->isPlaying());
                });
            }
            addParam(run);

            auto image = new PicWidget(module);
            image->box.pos = Vec(6, 17);
            image->box.size = Vec(288, 162);
            addChild(image);

            auto picButton = new PicButton(theme);
            picButton->center(Vec (285.0f, 188.0f));
            if (module) {
                picButton->onClick([this, module]() {
                    module->loadImageDialog();
                });
            }
            addChild(picButton);

        } else {
            panel->theme = theme;
            SetThemeChildren(this, theme);
        }
    }

    void appendContextMenu(Menu *menu) override
    {
        if (!this->module)
            return;
        ThemeModule *themeModule = dynamic_cast<ThemeModule *>(this->module);
        themeModule->addThemeMenu(menu, dynamic_cast<IChangeTheme *>(this));
    }
};

Model *modelImagine = createModel<Imagine, ImagineUi>("pachde-imagine");
