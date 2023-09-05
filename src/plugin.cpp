#include "plugin.hpp"
using namespace ::rack;

Plugin* pluginInstance;

void init(Plugin* p) {
	pluginInstance = p;

	// Add modules here
	p->addModel(modelImagine);
	p->addModel(modelCopper);
	p->addModel(modelCopperMini);
	p->addModel(modelInfo);
	p->addModel(modelBlank);

	// Any other plugin initialization may go here.
	// As an alternative, consider lazy-loading assets and lookup tables when your module is created to reduce startup times of Rack.
}
