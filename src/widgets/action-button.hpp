#pragma once
#include "widgetry.hpp"
#include "tip-widget.hpp"
using namespace pachde;
namespace widgetry {

template<typename TSvg>
struct TActionButton : ::rack::app::SvgButton
{
    using Base = ::rack::app::SvgButton;

    bool key_ctrl{false};
    bool key_shift{false};
    bool sticky{false};
    bool latched{false};
    std::function<void(bool, bool)> handler{nullptr};
    TipHolder * tip_holder{nullptr};

    TActionButton() {
        this->shadow->hide();
    }

    virtual ~TActionButton() {
        if (tip_holder) {
            delete tip_holder;
            tip_holder = nullptr;
        }
    }

    void describe(std::string text) {
        if (!tip_holder) {
            tip_holder = new TipHolder();
        }
        tip_holder->setText(text);
    }

    void set_handler(std::function<void(bool,bool)> callback) { handler = callback; }

    void set_sticky(bool is_sticky) { sticky = is_sticky; }

    void onHover(const HoverEvent& e) override {
        e.consume(this);
    }

    void destroyTip() {
        if (tip_holder) { tip_holder->destroyTip(); }
    }
    void createTip() {
        if (tip_holder) { tip_holder->createTip(); }
    }

    void onEnter(const EnterEvent& e) override {
        Base::onEnter(e);
        createTip();
    }

    void onLeave(const LeaveEvent& e) override {
        destroyTip();
        Base::onLeave(e);
    }

    void onDragStart(const DragStartEvent& e) override {
        destroyTip();
        Base::onDragStart(e);
        if (!sticky) {
            latched = true;
        }
    }

    void onDragLeave(const DragLeaveEvent& e) override {
        destroyTip();
        Base::onDragLeave(e);
        if (!sticky) {
            latched = false;
        }
    }

    void onDragEnd(const DragEndEvent& e) override {
        destroyTip();
        if (!sticky) Base::onDragEnd(e);
    }

    void onButton(const ButtonEvent &e) override {
        if (e.button != GLFW_MOUSE_BUTTON_LEFT) {
            Base::onButton(e);
            return;
        }

        if (sticky) {
            if (e.action == GLFW_PRESS) {
                ActionEvent eAction;
                onAction(eAction);
            }
        } else {
            if (e.action == GLFW_RELEASE) {
                ActionEvent eAction;
                onAction(eAction);
            }
        }
    }

    void onAction(const ActionEvent& e) override {
        destroyTip();
        e.consume(this) ;
        if (handler) {
            handler(key_ctrl, key_shift);
        }
        if (sticky) {
            latched = !latched;
        } else {
            Base::onAction(e);
        }
    }

    void onHoverKey(const HoverKeyEvent& e) override {
        Base::onHoverKey(e);
        key_ctrl = (e.mods & RACK_MOD_MASK) & RACK_MOD_CTRL;
        key_shift = (e.mods & RACK_MOD_MASK) & GLFW_MOD_SHIFT;
    }

    void onDirty(const DirtyEvent& e) override {
        fb->setDirty();
    }

    void sync_frame() {
        if (sticky) {
            sw->setSvg(frames[latched ? 1 : 0]);
            fb->setDirty();
        }
    }

    void step() override {
        if (sticky) sync_frame();
    }

    void appendContextMenu(ui::Menu* menu) {}

    void createContextMenu() {
        ui::Menu* menu = createMenu();
    	appendContextMenu(menu);
    }

    void loadSvg(ILoadSvg* loader) {
        if (frames.empty()) {
            addFrame(loader->loadSvg(TSvg::up()));
            addFrame(loader->loadSvg(TSvg::down()));
        }
    }

    void applyTheme(std::shared_ptr<SvgTheme> theme) {
        applySvgTheme(frames[0], theme);
        applySvgTheme(frames[1], theme);
    }

    void updateSvg(std::shared_ptr<SvgTheme> theme) {
        frames[0]->loadFile(TSvg::up());
        frames[1]->loadFile(TSvg::down());
        applyTheme(theme);
        fb->dirty = true;
    }
};

struct MediumButtonSvg {
    static std::string up()   { return asset::plugin(pluginInstance, "res/widget/med-but-up.svg"); }
    static std::string down() { return asset::plugin(pluginInstance, "res/widget/med-but-dn.svg"); }
};
using MediumActionButton = TActionButton<MediumButtonSvg>;

struct SmallButtonSvg {
    static std::string up()   { return asset::plugin(pluginInstance, "res/widget/sm-but-up.svg"); }
    static std::string down() { return asset::plugin(pluginInstance, "res/widget/sm-but-dn.svg"); }
};
using SmallActionButton = TActionButton<SmallButtonSvg>;

struct PlayButtonSvg {
    static std::string up()   { return asset::plugin(pluginInstance, "res/widget/play-btn.svg"); }
    static std::string down() { return asset::plugin(pluginInstance, "res/widget/pause-btn.svg"); }
};
using PlayActionButton = TActionButton<PlayButtonSvg>;

struct Pallette1Svg {
    static std::string up()   { return asset::plugin(pluginInstance, "res/widget/palette-1-blue.svg"); }
    static std::string down() { return asset::plugin(pluginInstance, "res/widget/palette-down.svg"); }
};
using Palette1ActionButton = TActionButton<Pallette1Svg>;

struct Pallette2Svg {
    static std::string up()   { return asset::plugin(pluginInstance, "res/widget/palette-2-orange.svg"); }
    static std::string down() { return asset::plugin(pluginInstance, "res/widget/palette-down.svg"); }
};
using Palette2ActionButton = TActionButton<Pallette2Svg>;

struct Pallette3Svg {
    static std::string up()   { return asset::plugin(pluginInstance, "res/widget/palette-3-green.svg"); }
    static std::string down() { return asset::plugin(pluginInstance, "res/widget/palette-down.svg"); }
};
using Palette3ActionButton = TActionButton<Pallette3Svg>;

struct Pallette4Svg {
    static std::string up()   { return asset::plugin(pluginInstance, "res/widget/palette-4-yellow.svg"); }
    static std::string down() { return asset::plugin(pluginInstance, "res/widget/palette-down.svg"); }
};
using Palette4ActionButton = TActionButton<Pallette4Svg>;

struct Pallette5Svg {
    static std::string up()   { return asset::plugin(pluginInstance, "res/widget/palette-5-magenta.svg"); }
    static std::string down() { return asset::plugin(pluginInstance, "res/widget/palette-down.svg"); }
};
using Palette5ActionButton = TActionButton<Pallette5Svg>;

struct TinyButtonSvg {
    static std::string up()   { return asset::plugin(pluginInstance, "res/widget/tiny-btn-up.svg"); }
    static std::string down() { return asset::plugin(pluginInstance, "res/widget/tiny-btn-down.svg"); }
};
using TinyActionButton = TActionButton<TinyButtonSvg>;

template <typename TActionButton>
TActionButton* createThemeSvgButton(ILoadSvg* loader, Vec pos) {
    TActionButton* o = new TActionButton();
    o->box.pos = pos;
    o->loadSvg(loader);
    return o;
}

template <typename TActionButton>
TActionButton* createThemeSvgButton(ILoadSvg* loader, std::shared_ptr<SvgTheme> theme, Vec pos) {
    TActionButton* o = new TActionButton();
    o->box.pos = pos;
    o->loadSvg(loader);
    o->applyTheme(theme);
    return o;
}


}