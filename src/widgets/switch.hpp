#pragma once
#include "widgetry.hpp"
#include "../services/theme.hpp"
using namespace pachde;
namespace widgetry {

struct Switch : rack::Switch, ISetTheme {
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