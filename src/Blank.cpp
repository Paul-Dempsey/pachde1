#include <rack.hpp>
#include "plugin.hpp"
#include "components.hpp"
#include "resizable.hpp"
#include "colors.hpp"

enum HpSizes {
    Default = 5,
    Least = 1,
};


struct BlankModule : Module {
    bool dirty = false;
    Theme theme = Theme::Unset;
    int hp_wide = HpSizes::Default;

	void onReset() override {
        dirty = true;
	}

    json_t* dataToJson() override {
        json_t* rootJ = json_object();

        std::string value = ToString(theme);
        json_object_set_new(rootJ, "theme", json_stringn(value.c_str(), value.size()));

        json_object_set_new(rootJ, "width", json_integer(hp_wide));

        return rootJ;
    }

    void dataFromJson(json_t* rootJ) override {
        theme = ThemeFromJson(rootJ);

		json_t* widthJ = json_object_get(rootJ, "width");
		if (widthJ) {
			hp_wide = json_integer_value(widthJ);
        }

        dirty = true;
    }
};

struct BlankPanel : Widget {
    Theme theme;

	BlankPanel(Theme t, Vec size) {
        theme = ConcreteTheme(t);
        box.size = size;
	}

    // void step() override {
    // 	Widget::step();
    // }

	void draw(const DrawArgs& args) override {
        NVGcolor outer = GRAY80, inner = GRAY90;
        switch (theme) {
            case Theme::Dark:
            case Theme::HighContrast:
                outer = GRAY20;
                inner = GRAY25;
                break;
            case Theme::Light:
            case Theme::Unset:
                outer = GRAY80;
                inner = GRAY90;
                break;
        };

		nvgBeginPath(args.vg);
		nvgRect(args.vg, 0.0, 0.0, box.size.x, box.size.y);
		nvgFillColor(args.vg, outer);
		nvgFill(args.vg);

		Widget::draw(args);
	}
};

struct BlankModuleResizeHandle : OpaqueWidget {
	bool right = false;
	Vec dragPos;
	Rect originalBox;
	BlankModule* module;

	BlankModuleResizeHandle() {
		box.size = Vec(RACK_GRID_WIDTH * 1, RACK_GRID_HEIGHT);
	}

	void onDragStart(const DragStartEvent& e) override {
		if (e.button != GLFW_MOUSE_BUTTON_LEFT)
			return;

		dragPos = APP->scene->rack->getMousePos();
		ModuleWidget* mw = getAncestorOfType<ModuleWidget>();
		assert(mw);
		originalBox = mw->box;
	}

	void onDragMove(const DragMoveEvent& e) override {
		ModuleWidget* mw = getAncestorOfType<ModuleWidget>();
		assert(mw);

		Vec newDragPos = APP->scene->rack->getMousePos();
		float deltaX = newDragPos.x - dragPos.x;

		Rect newBox = originalBox;
		Rect oldBox = mw->box;
		const float minWidth = HpSizes::Least * RACK_GRID_WIDTH;
		if (right) {
            newBox.size.x = NearestHp(newBox.size.x + deltaX, minWidth);
		}
		else {
            newBox.size.x = NearestHp(newBox.size.x - deltaX, minWidth);
			newBox.pos.x = originalBox.pos.x + originalBox.size.x - newBox.size.x;
		}

		// Set box and test whether it's valid
		mw->box = newBox;
		if (!APP->scene->rack->requestModulePos(mw, newBox.pos)) {
			mw->box = oldBox;
		}
		module->hp_wide = std::round(mw->box.size.x / RACK_GRID_WIDTH);
	}

	// void draw(const DrawArgs& args) override {
	// 	for (float x = 5.0; x <= 10.0; x += 5.0) {
	// 		nvgBeginPath(args.vg);
	// 		const float margin = 5.0;
	// 		nvgMoveTo(args.vg, x + 0.5, margin + 0.5);
	// 		nvgLineTo(args.vg, x + 0.5, box.size.y - margin + 0.5);
	// 		nvgStrokeWidth(args.vg, 1.0);
	// 		nvgStrokeColor(args.vg, nvgRGBAf(0.5, 0.5, 0.5, 0.5));
	// 		nvgStroke(args.vg);
	// 	}
	// }
};

struct BlankModuleWidget : ModuleWidget {
	Widget* rightHandle = NULL;
	Widget* topRightScrew = NULL;
    Widget* title = NULL;
    Widget* logo = NULL;
	Widget* bottomRightScrew = NULL;
    BlankPanel * panel = NULL;

    BlankModuleWidget(BlankModule* module) {
        setModule(module);
        onChangeTheme();
	}
    void clear() {
        panel = NULL;
        setPanel(NULL);
	    rightHandle = NULL;
	    topRightScrew = NULL;
        title = NULL;
        logo = NULL;
	    bottomRightScrew = NULL;
        clearChildren();
    }
    void addResizeHandles(BlankModule* module) {
        BlankModuleResizeHandle* leftHandle = new BlankModuleResizeHandle;
        leftHandle->module = module;
        addChild(leftHandle);

        BlankModuleResizeHandle* rightHandle = new BlankModuleResizeHandle;
        rightHandle->right = true;
        this->rightHandle = rightHandle;
        rightHandle->module = module;
        addChild(rightHandle);
    }

    void onChangeTheme() {
        auto module = dynamic_cast<BlankModule*>(this->module);
        auto theme = module ? ConcreteTheme(module->theme) : Theme::Light;
        // set default size for browser
        box.size = Vec(RACK_GRID_WIDTH * HpSizes::Default, RACK_GRID_HEIGHT);
        clear();
        panel = new BlankPanel(theme, box.size);
        setPanel(panel);
        switch (theme) {
            case Theme::Dark:
            case Theme::HighContrast:
                if (module) addResizeHandles(module);

                addChild(createWidget<ScrewCapDark>(Vec(0, 0)));

                title = createWidgetCentered<NullWidgetDark>(Vec(box.size.x / 2, 7.5f));
                addChild(title);

                topRightScrew = createWidget<ScrewCapDark>(Vec(box.size.x - RACK_GRID_WIDTH, 0));
                addChild(topRightScrew);

                addChild(createWidget<ScrewCapDark>(Vec(0, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

                logo = createWidgetCentered<LogoWidgetBright>(Vec(box.size.x / 2, RACK_GRID_HEIGHT - RACK_GRID_WIDTH + 7.5f));
                addChild(logo);

                bottomRightScrew = createWidget<ScrewCapDark>(Vec(box.size.x - RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH));
                addChild(bottomRightScrew);
                break;

            case Theme::Light:
            default:
                if (module) addResizeHandles(module);
                addChild(createWidget<ScrewCapMed>(Vec(0, 0)));

                title = createWidgetCentered<NullWidgetBright>(Vec(box.size.x / 2, 7.5f));
                addChild(title);

                topRightScrew = createWidget<ScrewCapMed>(Vec(box.size.x - RACK_GRID_WIDTH, 0));
                addChild(topRightScrew);

                addChild(createWidget<ScrewCapMed>(Vec(0, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

                logo = createWidgetCentered<LogoWidget>(Vec(box.size.x / 2, RACK_GRID_HEIGHT - RACK_GRID_WIDTH + 7.5f));
                addChild(logo);

                bottomRightScrew = createWidget<ScrewCapMed>(Vec(box.size.x - RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH));
                addChild(bottomRightScrew);
                break;
        }
        // Set box width from loaded Module before adding to the RackWidget, so modules aren't unnecessarily shoved around.
        if (module) {
            box.size.x = module->hp_wide * RACK_GRID_WIDTH;
        }
    }

	void step() override {
        BlankModule* module = dynamic_cast<BlankModule*>(this->module);
		if (module) {
			box.size.x = module->hp_wide * RACK_GRID_WIDTH;
		}
        panel->box.size = box.size;
        if (3 * RACK_GRID_WIDTH > box.size.x) {
            title->box.pos.y = RACK_GRID_WIDTH;
            logo->box.pos.y = RACK_GRID_HEIGHT - 2 * RACK_GRID_WIDTH;
        } else {
            title->box.pos.y = 0;
            logo->box.pos.y = RACK_GRID_HEIGHT - RACK_GRID_WIDTH;
        }
		topRightScrew->box.pos.x = box.size.x - RACK_GRID_WIDTH;
        title->box.pos.x = box.size.x / 2 - title->box.size.x / 2;
		bottomRightScrew->box.pos.x = box.size.x - RACK_GRID_WIDTH;
        logo->box.pos.x = box.size.x / 2- logo->box.size.x / 2;
		if (rightHandle) {
            rightHandle->box.pos.x = box.size.x - rightHandle->box.size.x;
        }
		ModuleWidget::step();
	}

    void appendContextMenu(Menu* menu) override {
        if (!this->module) return;
        BlankModule* module = dynamic_cast<BlankModule*>(this->module);
        menu->addChild(new MenuSeparator);
        menu->addChild(createSubmenuItem("Theme", "", 
            [=](Menu* menu) {
                menu->addChild(createCheckMenuItem("Light", "",
                    [=]() { return module->theme == Theme::Light; },
                    [=]() {
                        module->theme = Theme::Light;
                        module->dirty = true;
                        onChangeTheme();
                    }
                ));
                menu->addChild(createCheckMenuItem("Dark", "",
                    [=]() { return module->theme == Theme::Dark; },
                    [=]() { 
                        module->theme = Theme::Dark;
                        module->dirty = true;
                        onChangeTheme();
                    }
                ));
            }));
    }
};

Model* modelBlank = createModel<BlankModule, BlankModuleWidget>("pachde-blank");
