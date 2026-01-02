#include "Fancy.hpp"
#include "dialogs/fancy-dialogs.hpp"
#include "services/rack-help.hpp"
#include "widgets/hamburger.hpp"
#include "widgets/switch.hpp"

namespace pachde {

struct MiniFancySvg {
    static std::string background() { return asset::plugin(pluginInstance, "res/FancyBox-mini.svg"); }
};

FancyMini::FancyMini(Fancy *module) : my_module(module) {
    setModule(module);
    if (my_module) {
        my_module->other_fancy = !is_single_fancy(my_module);
    }
    theme_holder = my_module ? my_module : new ThemeBase();
    theme_holder->setNotify(this);
    auto theme = theme_holder->getTheme();
    auto svg_theme = getThemeCache().getTheme(ThemeName(theme));
    auto panel = createSvgThemePanel<MiniFancySvg>(&my_svgs, nullptr);
    auto layout = panel->svg;
    setPanel(panel);

    if (my_module && my_module->other_fancy) {
        ::svg_query::hideElements(layout, "k:");
        return;
    }

    ::svg_query::BoundsIndex bounds;
    svg_query::addBounds(layout, "k:", bounds, true);

    auto light = createLightCentered<SmallLight<BlueLight>>(bounds["k:fancy-light"].getCenter(), my_module, Fancy::L_FANCY);
    HOT_POSITION("k:fancy-light", HotPosKind::Center, light);
    addChild(light);

    auto fancy_switch = createParam<widgetry::Switch>(0, my_module, Fancy::P_FANCY);
    fancy_switch->box = bounds["k:on-off"];
    HOT_POSITION("k:on-off", HotPosKind::Box, fancy_switch);
    fancy_switch->applyTheme(svg_theme);
    addChild(fancy_switch);

    add_check(bounds,"k:pic-check", Fancy::P_FANCY_IMAGE_ON, svg_theme);
    add_check(bounds,"k:fill-check", Fancy::P_FANCY_FILL_ON, svg_theme);
    add_check(bounds, "k:linear-check", Fancy::P_FANCY_LINEAR_ON, svg_theme);
    add_check(bounds, "k:radial-check", Fancy::P_FANCY_RADIAL_ON, svg_theme);
    add_check(bounds, "k:box-check", Fancy::P_FANCY_BOX_ON, svg_theme);

    auto ham = createWidgetCentered<Hamburger>(bounds["k:pic-options"].getCenter());
    HOT_POSITION("k:pic-options", HotPosKind::Center, ham);
    ham->set_handler([=](){ pic_options(); });
    addChild(ham);

    ham = createWidgetCentered<Hamburger>(bounds["k:fill-options"].getCenter());
    HOT_POSITION("k:fill-options", HotPosKind::Center, ham);
    ham->set_handler([=](){ fill_options(); });
    addChild(ham);

    ham = createWidgetCentered<Hamburger>(bounds["k:linear-options"].getCenter());
    HOT_POSITION("k:linear-options", HotPosKind::Center, ham);
    ham->set_handler([=](){ linear_options(); });
    addChild(ham);

    ham = createWidgetCentered<Hamburger>(bounds["k:radial-options"].getCenter());
    HOT_POSITION("k:radial-options", HotPosKind::Center, ham);
    ham->set_handler([=](){ radial_options(); });
    addChild(ham);

    ham = createWidgetCentered<Hamburger>(bounds["k:box-options"].getCenter());
    HOT_POSITION("k:box-options", HotPosKind::Center, ham);
    ham->set_handler([=](){ box_options(); });
    addChild(ham);
}

void FancyMini::add_check( ::svg_query::BoundsIndex &bounds, const char *key, int param, std::shared_ptr<svg_theme::SvgTheme> svg_theme) {
    auto check = Center(createThemeParamButton<CheckButton>(
        &my_svgs,
        bounds[key].getCenter(),
        my_module,
        param,
        svg_theme
    ));
    HOT_POSITION(key, HotPosKind::Center, check);
    addChild(check);
}

FancyMini::~FancyMini() {
    if (my_module && my_module->my_cloak) {
        if (!my_module->orphan_cloak) {
            fancy_background(false);
        }
    }
}

void FancyMini::fancy_background(bool fancy) {
    if (!my_module || my_module->other_fancy) return;
    my_module->make_fancy(fancy);
}

void FancyMini::pic_options() {
    show_picture_dialog(this, my_module, theme_holder->getTheme());
}

void FancyMini::fill_options() {
    show_fill_dialog(this, my_module, theme_holder->getTheme());
}

void FancyMini::linear_options() {
    show_linear_gradient_dialog(this, my_module, theme_holder->getTheme());
}

void FancyMini::radial_options() {
    show_radial_gradient_dialog(this, my_module, theme_holder->getTheme());
}

void FancyMini::box_options() {
    show_box_gradient_dialog(this, my_module, theme_holder->getTheme());
}

void FancyMini::onChangeTheme(ChangedItem item)
{
    if (ChangedItem::Theme == item) {
        auto svg_theme = getThemeCache().getTheme(ThemeName(theme_holder->getTheme()));
        my_svgs.changeTheme(svg_theme);
        applyChildrenTheme(this, svg_theme);
        sendDirty(this);
    }
}

void FancyMini::onHoverKey(const HoverKeyEvent &e)
{
    if (!my_module || my_module->other_fancy) {
        Base::onHoverKey(e);
        return;
    }
#ifdef HOT_SVG
    auto mods = e.mods & RACK_MOD_MASK;
    switch (e.key) {
    case GLFW_KEY_F5: {
        if (e.action == GLFW_RELEASE && (0 == mods)) {
            e.consume(this);
            reloadThemeCache();
            my_svgs.reloadAll();
            onChangeTheme(ChangedItem::Theme);
            if (!my_module->other_fancy) {
                auto panel = dynamic_cast<SvgThemePanel<MiniFancySvg>*>(getPanel());
                auto bounds = makeBounds(panel->svg, "k:", true);
                positionWidgets(pos_widgets, bounds);
            }
            sendDirty(this);
        }
    } break;
    }
#endif
    Base::onHoverKey(e);
}

void FancyMini::step() {
    Base::step();
    theme_holder->pollRackThemeChanged();
}

void FancyMini::draw(const DrawArgs &args) {
    Base::draw(args);
    if (my_module && my_module->other_fancy) {
        draw_disabled_panel(this, GetPreferredTheme(theme_holder), args, 20.f, 20.f);
    }
}

void FancyMini::appendContextMenu(Menu *menu) {
    menu->addChild(createMenuLabel<HamburgerTitle>("#d Mini FancyBox"));
    menu->addChild(createCheckMenuItem("Keep effects when removed", "",
        [=](){ return my_module->orphan_cloak; },
        [=](){ my_module->orphan_cloak = !my_module->orphan_cloak; }
    ));
    menu->addChild(createMenuLabel<FancyLabel>("theme"));
    AddThemeMenu(menu, this, theme_holder, false, false, false);
}
}