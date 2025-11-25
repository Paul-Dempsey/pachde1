RACK_DIR ?= ../..

ifdef DEV_BUILD
FLAGS += -O0
FLAGS += -DHOT_SVG
FLAGS += -DDEV_BUILD
else
FLAGS += -O3
FLAGS += -DNDEBUG
endif

FLAGS += -Isrc

SOURCES += src/myplugin.cpp
SOURCES += src/resizable.cpp

# widgets
SOURCES += src/widgets/color-widgets.cpp
SOURCES += src/widgets/draw-logo.cpp
SOURCES += src/widgets/pic.cpp
SOURCES += src/widgets/pic_button.cpp
SOURCES += src/widgets/components.cpp
SOURCES += src/widgets/color-picker.cpp
SOURCES += src/widgets/element-style.cpp
SOURCES += src/widgets/port.cpp
SOURCES += src/widgets/screws.cpp
SOURCES += src/widgets/small_push.cpp
SOURCES += src/widgets/switch.cpp
SOURCES += src/widgets/text-input.cpp
SOURCES += src/widgets/text-button.cpp
SOURCES += src/widgets/widgetry.cpp

# services
SOURCES += src/services/colors.cpp
SOURCES += src/services/dsp.cpp
SOURCES += src/services/json-help.cpp
SOURCES += src/services/open-file.cpp
SOURCES += src/services/packed-color.cpp
SOURCES += src/services/rack-help.cpp
SOURCES += src/services/svg-query.cpp
SOURCES += src/services/svg-theme.cpp
SOURCES += src/services/svg-theme-load.cpp
SOURCES += src/services/text.cpp
SOURCES += src/services/text-align.cpp
SOURCES += src/services/theme.cpp
SOURCES += src/services/theme-module.cpp

# Null module
SOURCES += src/modules/Null/Null.cpp

# Info module
SOURCES += src/modules/Info/Info_panel.cpp
SOURCES += src/modules/Info/info_symbol.cpp
SOURCES += src/modules/Info/info_settings.cpp
SOURCES += src/modules/Info/settings-dialog.cpp
SOURCES += src/modules/Info/Info_ui.cpp
SOURCES += src/modules/Info/Info.cpp

# Copper module
SOURCES += src/modules/Copper/Copper.cpp
SOURCES += src/modules/Copper/CopperUI.cpp
SOURCES += src/modules/Copper/CopperMini.cpp

# Imagine module
SOURCES += src/modules/Imagine/Imagine.cpp
SOURCES += src/modules/Imagine/ImaginePanel.cpp
SOURCES += src/modules/Imagine/ImagineUi.cpp
SOURCES += src/modules/Imagine/pic_widget.cpp
SOURCES += src/modules/Imagine/play_pause.cpp
SOURCES += src/modules/Imagine/traversal.cpp

# Skiff module
SOURCES += src/modules/Skiff/Skiff.cpp
SOURCES += src/modules/Skiff/SkiffUi.cpp
SOURCES += src/modules/Skiff/skiff-help.cpp
SOURCES += src/modules/Skiff/rails.cpp

# FancyBox module
SOURCES += src/modules/FancyBox/Fancy.cpp
SOURCES += src/modules/FancyBox/FancyUi.cpp
SOURCES += src/modules/FancyBox/cloak.cpp

# Rui module
SOURCES += src/modules/Rui/Rui.cpp

# Guide module
SOURCES += src/modules/Guide/Guide.cpp
SOURCES += src/modules/Guide/guide-data.cpp
SOURCES += src/modules/Guide/guide-list.cpp
SOURCES += src/modules/Guide/panel-guides.cpp

# PanelTone module
SOURCES += src/modules/PanelTone/PanelTone.cpp
SOURCES += src/modules/PanelTone/fader.cpp
SOURCES += src/modules/PanelTone/panel-overlay.cpp

DISTRIBUTABLES += res
DISTRIBUTABLES += $(wildcard LICENSE*)

DISTRIBUTABLES += presets
# DISTRIBUTABLES += selections

# Include the VCV Rack plugin Makefile framework
include $(RACK_DIR)/plugin.mk