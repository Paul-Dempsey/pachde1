#pragma once
#include "widgetry.hpp"
using namespace pachde;
namespace widgetry {

struct Switch : rack::Switch, IBasicTheme {
    int value = 0;
    int units = 2;
    NVGcolor background, frame, thumb, thumb_top, thumb_bottom;

    Switch();

	void initParamQuantity() override;
    void draw(const DrawArgs &args) override;
    void onChange(const ChangeEvent& e) override;
    void setTheme(Theme theme) override;
};

}