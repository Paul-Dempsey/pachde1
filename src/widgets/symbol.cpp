#include "symbol.hpp"

namespace widgetry {

Symbol::Symbol() {
	box.size = math::Vec();
}

void Symbol::wrap() {
	box.size = svg ? svg->getSize() : math::Vec{};
}

void Symbol::setSvg(std::shared_ptr<window::Svg> svg) {
	this->svg = svg;
	wrap();
}

void Symbol::draw(const DrawArgs& args) {
	if (!svg) return;

	window::svgDraw(args.vg, svg->handle);
}

}