RACK_DIR ?= ../..

FLAGS +=

SOURCES += src/plugin.cpp
SOURCES += src/colors.cpp
SOURCES += src/Info.cpp

DISTRIBUTABLES += res/ScrewCap.svg
DISTRIBUTABLES += res/ScrewCapMed.svg
DISTRIBUTABLES += res/ScrewCapDark.svg

# DISTRIBUTABLES += presets
# DISTRIBUTABLES += selections

# Include the VCV Rack plugin Makefile framework
include $(RACK_DIR)/plugin.mk