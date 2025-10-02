#pragma once
#include "widgetry.hpp"
#include "tip-widget.hpp"
using namespace pachde;
namespace widgetry {

template<typename TSvg>
struct TButton : ::rack::app::SvgButton, IBasicTheme
{
    using Base = ::rack::app::SvgButton;

    bool key_ctrl{false};
    bool key_shift{false};
    bool sticky{false};
    bool latched{false};
    std::function<void(bool, bool)> handler{nullptr};
    TipHolder * tip_holder{nullptr};

    TButton()
    {
        this->shadow->hide();
    }

    virtual ~TButton()
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
            sync_frame();
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

    void applyTheme(Theme theme) {
        if (frames.empty()) {
            switch (theme) {
            case Theme::Dark:
                addFrame(Svg::load(asset::plugin(pluginInstance, TSvg::up_dark())));
                addFrame(Svg::load(asset::plugin(pluginInstance, TSvg::down_dark())));
                break;
            case Theme::HighContrast:
                addFrame(Svg::load(asset::plugin(pluginInstance, TSvg::up_hc())));
                addFrame(Svg::load(asset::plugin(pluginInstance, TSvg::down_hc())));
                break;
            case Theme::Unset:
            case Theme::Light:
            default:
                addFrame(Svg::load(asset::plugin(pluginInstance, TSvg::up())));
                addFrame(Svg::load(asset::plugin(pluginInstance, TSvg::down())));
                break;
            }
        } else {
            switch (theme) {
            case Theme::Dark:
                frames[0]->loadFile(asset::plugin(pluginInstance, TSvg::up_dark()));
                frames[1]->loadFile(asset::plugin(pluginInstance, TSvg::down_dark()));
                break;
            case Theme::HighContrast:
                frames[0]->loadFile(asset::plugin(pluginInstance, TSvg::up_hc()));
                frames[1]->loadFile(asset::plugin(pluginInstance, TSvg::down_hc()));
                break;
            case Theme::Unset:
            case Theme::Light:
            default:
                frames[0]->loadFile(asset::plugin(pluginInstance, TSvg::up()));
                frames[1]->loadFile(asset::plugin(pluginInstance, TSvg::down()));
                break;
            }
        }
        fb->setDirty();
    }

    void setTheme(Theme theme) override {
        IBasicTheme::setTheme(theme);
        applyTheme(theme);
    }

    void appendContextMenu(ui::Menu* menu) {}

    void createContextMenu() {
        ui::Menu* menu = createMenu();
    	appendContextMenu(menu);
    }

};

struct MediumButtonSvg {
    static std::string up() { return "res/widget/med-but-up.svg"; }
    static std::string up_dark() { return "res/widget/med-but-up.svg"; }
    static std::string up_hc() { return "res/widget/med-but-up.svg"; }
    static std::string down() { return "res/widget/med-but-dn.svg"; }
    static std::string down_dark() { return "res/widget/med-but-dn.svg"; }
    static std::string down_hc() { return "res/widget/med-but-dn.svg"; }
};
using MediumButton = TButton<MediumButtonSvg>;

struct SmallButtonSvg {
    static std::string up() { return "res/widget/sm-but-up.svg"; }
    static std::string up_dark() { return "res/widget/sm-but-up-dark.svg"; }
    static std::string up_hc() { return "res/widget/sm-but-up-hc.svg"; }
    static std::string down() { return "res/widget/sm-but-dn.svg"; }
    static std::string down_dark() { return "res/widget/sm-but-dn-dark.svg"; }
    static std::string down_hc() { return "res/widget/sm-but-dn-hc.svg"; }
};
using SmallButton = TButton<SmallButtonSvg>;

}