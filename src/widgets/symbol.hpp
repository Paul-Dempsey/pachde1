#pragma once
#include "widgetry.hpp"

namespace widgetry {

struct Symbol : widget::Widget
{
	std::shared_ptr<window::Svg> svg;

    Symbol();
	void wrap();
	void setSvg(std::shared_ptr<window::Svg> svg);
	void draw(const DrawArgs& args) override;
};

}