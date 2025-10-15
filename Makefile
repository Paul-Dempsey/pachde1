RACK_DIR ?= ../..

ifdef DEV_BUILD
FLAGS += -O0
FLAGS += -DHOT_SVG
else
FLAGS += -O3
FLAGS += -DNDEBUG
endif

SOURCES += src/colors.cpp
SOURCES += src/components.cpp
SOURCES += src/dsp.cpp
SOURCES += src/open_file.cpp
SOURCES += src/pic_button.cpp
SOURCES += src/plugin.cpp
SOURCES += src/resizable.cpp
SOURCES += src/small_push.cpp
SOURCES += src/text.cpp
SOURCES += src/theme.cpp

# widgets
SOURCES += src/widgets/draw-logo.cpp
SOURCES += src/widgets/pic.cpp
SOURCES += src/widgets/port.cpp
SOURCES += src/widgets/screws.cpp
SOURCES += src/widgets/switch.cpp
SOURCES += src/widgets/widgetry.cpp

# services
SOURCES += src/services/json-help.cpp
SOURCES += src/services/rack-help.cpp
SOURCES += src/services/svg-query.cpp

# Null module
SOURCES += src/Null/Null.cpp

# Info module
SOURCES += src/Info/Info_panel.cpp
SOURCES += src/Info/info_symbol.cpp
SOURCES += src/Info/info_theme.cpp
SOURCES += src/Info/Info_ui.cpp
SOURCES += src/Info/Info.cpp
SOURCES += src/Info/text_align.cpp

# Copper module
SOURCES += src/Copper/Copper.cpp
SOURCES += src/Copper/CopperUI.cpp
SOURCES += src/Copper/CopperMini.cpp

# Imagine module
SOURCES += src/Imagine/Imagine.cpp
SOURCES += src/Imagine/ImaginePanel.cpp
SOURCES += src/Imagine/ImagineUi.cpp
SOURCES += src/Imagine/pic_widget.cpp
SOURCES += src/Imagine/play_pause.cpp
SOURCES += src/Imagine/traversal.cpp

# Skiff module
SOURCES += src/Skiff/Skiff.cpp
SOURCES += src/Skiff/skiff-help.cpp
SOURCES += src/Skiff/cloak.cpp

# Rui module
SOURCES += src/Rui/Rui.cpp

DISTRIBUTABLES += res
DISTRIBUTABLES += $(wildcard LICENSE*)

DISTRIBUTABLES += presets
# DISTRIBUTABLES += selections

# Include the VCV Rack plugin Makefile framework
include $(RACK_DIR)/plugin.mk