#pragma once

namespace pachde {

constexpr const float PANEL_WIDTH = 300.0f; // 20hp
constexpr const float PANEL_CENTER = PANEL_WIDTH / 2.0f;
constexpr const float PANEL_MARGIN = 5.0f;

// const float IMAGE_UNIT = 18.0f; // 18px image unit for 16x9 landscape
// const float PANEL_IMAGE_WIDTH = 16.0f * IMAGE_UNIT;
// const float PANEL_IMAGE_HEIGHT = 9.0f * IMAGE_UNIT;
constexpr const float PANEL_IMAGE_WIDTH = PANEL_WIDTH - 2.f;
constexpr const float PANEL_IMAGE_HEIGHT = 250.f;
constexpr const float PANEL_IMAGE_TOP = 17.f;
constexpr const float SECTION_WIDTH = PANEL_WIDTH - PANEL_MARGIN * 2.0f;
constexpr const float ORIGIN_X = PANEL_CENTER;
constexpr const float ORIGIN_Y = PANEL_IMAGE_TOP + PANEL_IMAGE_HEIGHT;

constexpr const float CONTROL_ROW = 282.f;
constexpr const float CONTROL_ROW_TEXT = 307.f;
constexpr const float OUTPUT_ROW = 332.f;
constexpr const float OUTPUT_ROW_TEXT = 365.f;

}