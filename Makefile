RACK_DIR ?= ../..

FLAGS +=

SOURCES += src/plugin.cpp
SOURCES += src/components.cpp
SOURCES += src/resizable.cpp
SOURCES += src/colors.cpp
SOURCES += src/text.cpp
SOURCES += src/dsp.cpp
SOURCES += src/pic.cpp
SOURCES += src/pic_widget.cpp
SOURCES += src/pic_button.cpp
SOURCES += src/switch.cpp
SOURCES += src/theme_module.cpp
SOURCES += src/traversal.cpp

# Blank module
SOURCES += src/Blank.cpp

# Info module
SOURCES += src/Info.cpp

# Imagine module
SOURCES += src/Imagine.cpp
SOURCES += src/ImaginePanel.cpp
SOURCES += src/ImagineUi.cpp

DISTRIBUTABLES += res

# DISTRIBUTABLES += presets
# DISTRIBUTABLES += selections

# Include the VCV Rack plugin Makefile framework
include $(RACK_DIR)/plugin.mk