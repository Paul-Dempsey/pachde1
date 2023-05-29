#include <rack.hpp>
#include <osdialog.h>
#include "plugin.hpp"
#include "components.hpp"
#include "text.hpp"
#include "dsp.hpp"
#include "themehelpers.hpp"

using namespace rack;


const float ONE_HP = 15.0f;
const float PANEL_WIDTH = 300.0f; // 20hp
const float PANEL_CENTER = PANEL_WIDTH / 2.0f;
const float PANEL_MARGIN = 5.0f;

const float IMAGE_UNIT = 18.0f; // 18px image unit for 16x9 landscape
const float PANEL_IMAGE_WIDTH = 16.0f * IMAGE_UNIT;
const float PANEL_IMAGE_HEIGHT = 9.0f * IMAGE_UNIT;
const float PANEL_IMAGE_SQUARE = PANEL_IMAGE_HEIGHT;
const float PANEL_IMAGE_TOP = 17.0f;
const float SECTION_WIDTH = PANEL_WIDTH - PANEL_MARGIN * 2.0f;
const float ORIGIN_X = PANEL_CENTER;
const float ORIGIN_Y = PANEL_IMAGE_TOP + PANEL_IMAGE_HEIGHT;

struct ISetPos {
    virtual void setPosition(Vec * pos) = 0;
};

struct Imagine : ThemeModule, ISetPos
{
    enum ParamIds {
        SLEW_PARAM,
        PICKPIC_PARAM,
        NUM_PARAMS
    };
    enum OutputIds {
        X_OUT,
        Y_OUT,
        NUM_OUTPUTS
    };

    std::string image_path;
    bool xy_pad = true; // default to square xy pad

    float image_width = PANEL_IMAGE_SQUARE;
    float image_height = PANEL_IMAGE_SQUARE;

    std::atomic<Vec*> position;
    SlewLimiter x_slew, y_slew;
    ControlRateTrigger control_rate;

    Imagine() {
        setPosition(new Vec(0,0));
        config(NUM_PARAMS, 0, NUM_OUTPUTS, 0);
		configParam(SLEW_PARAM, 0.0f, 1.0f, 0.8f, "Slew", "%", 0.0f, 100.0f);
        configParam(PICKPIC_PARAM, 0.0f, 1.0f, 0.0f, "Choose picture", "!");
        configOutput(X_OUT, "x");
        configOutput(Y_OUT, "y");
    }

    void setPosition(Vec *pos) override {
        position = pos;
    }

    bool loadImageDialog() {
		osdialog_filters* filters = osdialog_filters_parse("Images (.png .jpg .gif):.png,.jpg,.jpeg,.gif;Any (*):*"); 
		DEFER({osdialog_filters_free(filters);});

        std::string dir = asset::user("");
		char* pathC = osdialog_file(OSDIALOG_OPEN, dir.c_str(), NULL, filters);
		if (!pathC) {
			return false;
		}
		std::string path = pathC;
		std::free(pathC);
        image_path = path;
        DEBUG("Selected image (%s)", image_path.c_str());
        xy_pad = false;
        // todo: Load image
        return true;
    }


    void onSampleRateChange() override {
        control_rate.onSampleRateChanged();
        updateParams();
    }

    json_t *dataToJson() override {
        json_t *rootJ = ThemeModule::dataToJson();
        json_object_set_new(rootJ, "image", json_stringn(image_path.c_str(), image_path.size()));
        return rootJ;
    }

    void dataFromJson(json_t *rootJ) override {
        json_t *textJ = json_object_get(rootJ, "image");
        if (textJ) {
            image_path = json_string_value(textJ);
        }
        ThemeModule::dataFromJson(rootJ);
    }

    void updateParams() {
        float sampleRate = APP->engine->getSampleRate();
        float slew = params[SLEW_PARAM].getValue();
        x_slew.configure(sampleRate, slew, .01f);
        y_slew.configure(sampleRate, slew, .01f);
    }

	void process(const ProcessArgs& args) override {
        if (control_rate.process()) {
            updateParams();
        }

        if (position) {
            Vec *pos = position;
            outputs[X_OUT].setVoltage(x_slew.next(pos->x));
            outputs[Y_OUT].setVoltage(y_slew.next(pos->y));
        }
    }
};

struct XYPad : OpaqueWidget {
    ISetPos * receiver;
    float margin;
    Vec position;

    XYPad(ISetPos * host, float inset) : receiver(host), margin(inset) {
        assert(margin >= 0);
    }

    void onButton(const event::Button& e) override {
        if (!(e.action == GLFW_PRESS && e.button == GLFW_MOUSE_BUTTON_LEFT && (e.mods & RACK_MOD_MASK) == 0)) {
            return;
        }
        e.consume(this);
        position = e.pos;
        updateClient();
    }

    void onDragMove(const event::DragMove& e) override {
        auto z = APP->scene->rackScroll->zoomWidget->zoom;
        position.x += e.mouseDelta.x / z;
        position.y += e.mouseDelta.y / z;
        updateClient();
    }

    void updateClient()
    {
        if (!receiver) return;
        // compute bipolar -5.0 - 5.0
        float unit = 2.0f / insideWidth();
        float x = clamp(position.x, margin, margin + insideWidth());
        x = 5.0f * ((x - margin) * unit - 1.0f);

        unit = 2.0f / insideHeight();
        float y  = clamp(position.y, margin, margin + insideHeight());
        y = 5.0f * ((y - margin) * unit - 1.0f);
        receiver->setPosition(new Vec(x, -y));
    }

    float insideWidth() { return box.getWidth() - 2.0f * margin; }
    float insideHeight() { return box.getHeight() - 2.0f * margin; }

    void draw(const DrawArgs& args) override {
        auto w = insideWidth();
        auto h = insideHeight();
        FillRect(args.vg, margin, margin, w, h, COLOR_BRAND);
        Line(args.vg, margin + w/2.0f, margin, margin + w/2.0f, margin + h, Overlay(GRAY90), 0.5f);
        Line(args.vg, margin, margin + h/2.0f, margin + w, margin + h/2.0f, Overlay(GRAY90), 0.5f);
    }

    void drawLayer(const DrawArgs& args, int layer) override {
        if (1 == layer) {
            auto vg = args.vg;
            auto x = clamp(position.x, margin, margin + insideWidth());
            auto y = clamp(position.y, margin, margin + insideHeight());
            auto color = nvgTransRGBAf(SCHEME_YELLOW, 0.85);
            if (rack::settings::rackBrightness < 0.98f && rack::settings::haloBrightness > 0.0f) {
                nvgBeginPath(vg);
            	nvgRect(vg, x - 13.0f, y - 13.0f, 26.0f, 26.0f);
                NVGcolor icol = nvgTransRGBAf(color, rack::settings::haloBrightness / 2);
	            NVGcolor ocol = nvgTransRGBAf(color, 0);
            	NVGpaint paint = nvgRadialGradient(vg, x, y, 5, 13, icol, ocol);
                nvgFillPaint(vg, paint);
                nvgFill(vg);
            }
            nvgBeginPath(vg);
            nvgCircle(vg, x, y, 2.0);
            nvgFillColor(vg, color);
            nvgFill(vg);
        }
    }
};

struct ImaginePanel : Widget
{
    Theme theme;

    ImaginePanel(Theme t, Vec size)
    {
        theme = ConcreteTheme(t);
        box.size = size;
    }

    // void OutputCover(NVGcontext * vg, float x, float y, float width, float height)
    // {
    //     FillRect(vg, x, y, width, height, GRAY95);
    //     //BoxRect(vg, x, y, width, height, GRAY65);
    //     Line(vg, x, y + height, x + width, y + height, GRAY85);

    //     for (int i = 0; i < 3; ++i) {
    //         auto x1 = x + 4 + i*1.5f;
    //         Line(vg, x1, y + 4.0f, x1, y + height - 4.0f, GRAY75, 0.8f);
    //     }
    // }

    void draw(const DrawArgs &args) override
    {
        NVGcolor panel = PanelBackground(theme), textColor = ThemeTextColor(theme);
        auto vg = args.vg;

        // nvgScissor(args.vg, RECT_ARGS(args.clipBox)); // when is scissor managment necessary?

        // panel background
        FillRect(vg, 0.0, 0.0, box.size.x, box.size.y, panel);

        // image placeholder
        BoxRect(vg, PANEL_CENTER - PANEL_IMAGE_WIDTH / 2.0f, PANEL_IMAGE_TOP, PANEL_IMAGE_WIDTH, PANEL_IMAGE_HEIGHT, COLOR_BRAND);

        // x/y pad and square image placeholder frame
        // auto x = PANEL_CENTER - PANEL_IMAGE_SQUARE/2;
        // Line(vg, x, PANEL_IMAGE_TOP, x, PANEL_IMAGE_TOP + PANEL_IMAGE_HEIGHT, Overlay(GRAY95));
        // x += PANEL_IMAGE_SQUARE;
        // Line(vg, x, PANEL_IMAGE_TOP, x, PANEL_IMAGE_TOP + PANEL_IMAGE_HEIGHT, Overlay(GRAY95));

        //separator
        Line(vg, PANEL_MARGIN, 196.0f, SECTION_WIDTH, 196.0f, GRAY65);

        // outputs 1
        RoundRect(vg, 5.0f, 299.0f, 290.0f, 65.0f, OutputBackground(theme), 6.0f);

        //OutputCover(vg, PANEL_MARGIN, 250.0f, SECTION_WIDTH, 54.0f);
        //OutputCover(vg, PANEL_MARGIN, 308.0f, SECTION_WIDTH, 54.0f);

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

struct ImagineUi : ModuleWidget, IChangeTheme
{
    ImaginePanel *panel = NULL;
    RoundBlackKnob* knob = NULL;

    ImagineUi(Imagine *module)
    {
        setModule(module);
        setTheme(ModuleTheme(module));
    }

    void step() override {
        ModuleWidget::step();
        auto module = dynamic_cast<Imagine *>(this->module);
        if (module) {
            auto value = module->getParam(Imagine::PICKPIC_PARAM).getValue();
            if (value > 0.0f) {
                module->params[Imagine::PICKPIC_PARAM].value = 0.0f;
                if (module->loadImageDialog()) {
                    removeXYPad();
                }
            }
        }        
    }
    void removeXYPad() {
        for (Widget * child: children) {
            auto xy = dynamic_cast<XYPad*>(child);
            if (xy) {
                removeChild(child);
                delete child;
                break;
            }
        }
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
            addParam(createParamCentered<LEDLightButton<WhiteLight>>(Vec(278.0f, 190.0f), module, Imagine::PICKPIC_PARAM));

            {
                const float margin = 2.0f;
                auto xy = new XYPad(static_cast<ISetPos*>(module), margin);
                xy->box.pos.x = PANEL_CENTER - PANEL_IMAGE_SQUARE/2 - margin;
                xy->box.pos.y = PANEL_IMAGE_TOP - margin;
                xy->box.size.x = PANEL_IMAGE_SQUARE + 2 * margin;
                xy->box.size.y = xy->box.size.x;
                xy->position.x = margin + PANEL_IMAGE_SQUARE/2;
                xy->position.y = margin + PANEL_IMAGE_SQUARE/2;
                addChild(xy);
            }

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
