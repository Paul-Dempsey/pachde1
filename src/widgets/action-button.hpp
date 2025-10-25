#pragma once
#include "widgetry.hpp"
#include "tip-widget.hpp"
using namespace pachde;
namespace widgetry {

template<typename TSvg>
struct TActionButton : ::rack::app::SvgButton, IBasicTheme
{
    using Base = ::rack::app::SvgButton;

    bool key_ctrl{false};
    bool key_shift{false};
    bool sticky{false};
    bool latched{false};
    std::function<void(bool, bool)> handler{nullptr};
    TipHolder * tip_holder{nullptr};

    TActionButton()
    {
        this->shadow->hide();
    }

    virtual ~TActionButton()
    {
        if (tip_holder) {
            delete tip_holder;
            tip_holder = nullptr;
        }
    }

    void describe(std::string text)
    {
        if (!tip_holder) {
            tip_holder = new TipHolder();
        }
        tip_holder->setText(text);
    }

    void setHandler(std::function<void(bool,bool)> callback)
    {
        handler = callback;
    }

    void set_sticky(bool is_sticky)
    {
        sticky = is_sticky;
    }

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

    void onDragStart(const DragStartEvent& e) override
    {
        destroyTip();
        if (!sticky) Base::onDragStart(e);
    }

    void onDragLeave(const DragLeaveEvent& e) override {
        destroyTip();
        if (!sticky) Base::onDragLeave(e);
    }

    void onDragEnd(const DragEndEvent& e) override
    {
        if (!sticky) Base::onDragEnd(e);
        destroyTip();
    }

    void onHoverKey(const HoverKeyEvent& e) override
    {
        Base::onHoverKey(e);
        key_ctrl = (e.mods & RACK_MOD_MASK) & RACK_MOD_CTRL;
        key_shift = (e.mods & RACK_MOD_MASK) & GLFW_MOD_SHIFT;
    }

    void onAction(const ActionEvent& e) override
    {
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

    void onDirty(const DirtyEvent& e) override {
        fb->setDirty();
    }

    void sync_frame()
    {
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