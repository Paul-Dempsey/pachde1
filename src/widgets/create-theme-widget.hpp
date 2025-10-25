#pragma once
#include "components.hpp"
#include "../services/theme.hpp"

namespace pachde
{
// TODO: remove Center variants

template <class TParamWidget>
TParamWidget *createThemeParam(Theme theme, math::Vec pos, engine::Module *module, int paramId)
{
    TParamWidget *o = new TParamWidget();
    o->setTheme(theme);
    o->box.pos = pos;
    o->app::ParamWidget::module = module;
    o->app::ParamWidget::paramId = paramId;
    o->initParamQuantity();
    return o;
}

template <class TParamWidget>
TParamWidget *createThemeParamCentered(Theme theme, math::Vec pos, engine::Module *module, int paramId)
{
    TParamWidget *o = createThemeParam<TParamWidget>(theme, pos, module, paramId);
    o->box.pos = o->box.pos.minus(o->box.size.div(2));
    return o;
}

template <class TPortWidget>
TPortWidget *createThemeInput(Theme theme, math::Vec pos, engine::Module *module, int inputId)
{
    TPortWidget *o = new TPortWidget();
    o->setTheme(theme);
    o->box.pos = pos;
    o->app::PortWidget::module = module;
    o->app::PortWidget::type = engine::Port::INPUT;
    o->app::PortWidget::portId = inputId;
    return o;
}

template <class TPortWidget>
TPortWidget *createThemeInputCentered(Theme theme, math::Vec pos, engine::Module *module, int inputId)
{
    TPortWidget *o = createThemeInput<TPortWidget>(theme, pos, module, inputId);
    o->box.pos = o->box.pos.minus(o->box.size.div(2));
    return o;
}

template <class TColorPortWidget>
TColorPortWidget *createColorInput(Theme theme, PackedColor color, math::Vec pos, engine::Module *module, int id)
{
    TColorPortWidget *o = new TColorPortWidget();
    o->setTheme(theme);
    o->setMainColor(color);
    o->box.pos = pos;
    o->app::PortWidget::module = module;
    o->app::PortWidget::type = engine::Port::INPUT;
    o->app::PortWidget::portId = id;
    return o;
}

template <class TColorPortWidget>
TColorPortWidget *createColorInputCentered(Theme theme, PackedColor color, math::Vec pos, engine::Module *module, int id)
{
    TColorPortWidget *o = createColorInput<TColorPortWidget>(theme, color, pos, module, id);
    o->box.pos = pos.minus(o->box.size.div(2));
    return o;
}

template <class TPortWidget>
TPortWidget *createThemeOutput(Theme theme, math::Vec pos, engine::Module *module, int outputId)
{
    TPortWidget *o = new TPortWidget();
    o->setTheme(theme);
    o->box.pos = pos;
    o->app::PortWidget::module = module;
    o->app::PortWidget::type = engine::Port::OUTPUT;
    o->app::PortWidget::portId = outputId;
    return o;
}

template <class TPortWidget>
TPortWidget *createThemeOutputCentered(Theme theme, math::Vec pos, engine::Module *module, int outputId)
{
    TPortWidget *o = createThemeOutput<TPortWidget>(theme, pos, module, outputId);
    o->box.pos = o->box.pos.minus(o->box.size.div(2));
    return o;
}

template <class TColorPortWidget>
TColorPortWidget *createColorOutput(Theme theme, PackedColor color, math::Vec pos, engine::Module *module, int outputId)
{
    TColorPortWidget *o = new TColorPortWidget();
    o->setTheme(theme);
    o->setMainColor(color);
    o->box.pos = pos;
    o->app::PortWidget::module = module;
    o->app::PortWidget::type = engine::Port::OUTPUT;
    o->app::PortWidget::portId = outputId;
    return o;
}

template <class TColorPortWidget>
TColorPortWidget *createColorOutputCentered(Theme theme, PackedColor color, math::Vec pos, engine::Module *module, int outputId)
{
    TColorPortWidget *o = createColorOutput<TColorPortWidget>(theme, color, pos, module, outputId);
    o->box.pos = o->box.pos.minus(o->box.size.div(2));
    return o;
}

template <class TWidget>
TWidget *createThemeWidget(Theme theme, math::Vec pos)
{
    TWidget *o = new TWidget();
    o->setTheme(theme);
    o->box.pos = pos;
    return o;
}

template <class TWidget>
TWidget *createThemeWidgetCentered(Theme theme, math::Vec pos)
{
    TWidget *o = createThemeWidget<TWidget>(theme, pos);
    o->box.pos = o->box.pos.minus(o->box.size.div(2));
    return o;
}

}
