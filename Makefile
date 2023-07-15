RACK_DIR ?= ../..

FLAGS +=

SOURCES += src/plugin.cpp
SOURCES += src/components.cpp
SOURCES += src/resizable.cpp
SOURCES += src/colors.cpp
SOURCES += src/text.cpp
SOURCES += src/dsp.cpp
SOURCES += src/pic.cpp
SOURCES += src/pic_button.cpp
SOURCES += src/play_pause.cpp
SOURCES += src/port.cpp
SOURCES += src/switch.cpp
SOURCES += src/small_push.cpp
SOURCES += src/theme.cpp

# Null module
SOURCES += src/Null/Null.cpp

# Info module
SOURCES += src/Info/Info.cpp
SOURCES += src/Info/info_symbol.cpp

# Copper module
SOURCES += src/Copper/Copper.cpp

# Imagine module
SOURCES += src/Imagine/Imagine.cpp
SOURCES += src/Imagine/ImaginePanel.cpp
SOURCES += src/Imagine/ImagineUi.cpp
SOURCES += src/Imagine/pic_widget.cpp
SOURCES += src/Imagine/traversal.cpp

DISTRIBUTABLES += res
DISTRIBUTABLES += $(wildcard LICENSE*)

DISTRIBUTABLES += presets
# DISTRIBUTABLES += selections

# Include the VCV Rack plugin Makefile framework
include $(RACK_DIR)/plugin.mk