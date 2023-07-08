#pragma once

namespace pachde {

constexpr const float PANEL_WIDTH = 300.F; // 20hp
constexpr const float PANEL_CENTER = PANEL_WIDTH / 2.F;
constexpr const float PANEL_MARGIN = 5.F;

// const float IMAGE_UNIT = 18.0f; // 18px image unit for 16x9 landscape
// const float PANEL_IMAGE_WIDTH = 16.0f * IMAGE_UNIT;
// const float PANEL_IMAGE_HEIGHT = 9.0f * IMAGE_UNIT;
constexpr const float PANEL_IMAGE_WIDTH = PANEL_WIDTH - 2.f;
constexpr const float PANEL_IMAGE_HEIGHT = 235.f;
constexpr const float PANEL_IMAGE_TOP = 17.f;
constexpr const float PANEL_IMAGE_LEFT = PANEL_CENTER - PANEL_IMAGE_WIDTH / 2.f;
constexpr const float PANEL_IMAGE_RIGHT = PANEL_CENTER + PANEL_IMAGE_WIDTH / 2.f;

constexpr const float SECTION_WIDTH = PANEL_WIDTH - PANEL_MARGIN * 2.0f;
constexpr const float ORIGIN_X = PANEL_CENTER;
constexpr const float ORIGIN_Y = PANEL_IMAGE_TOP + PANEL_IMAGE_HEIGHT;

constexpr const float CONTROL_ROW = 267.f;
constexpr const float CONTROL_ROW_TEXT = CONTROL_ROW + 22.f;
constexpr const float CONTROL_ROW_2 = 308.f;
constexpr const float CONTROL_ROW_2_TEXT = CONTROL_ROW_2 + 22.f;

constexpr const float OUTPUT_ROW = 340.f;
constexpr const float OUTPUT_ROW_TEXT = 373.f;

}