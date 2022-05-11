# ----------------------------------------------------
# This file is generated by the Qt Visual Studio Add-in.
# ------------------------------------------------------

TEMPLATE = lib
TARGET = SVDGeneratorPlugin

QT += core xml widgets gui
CONFIG += c++11 release
DEFINES += SVDGENERATORPLUGIN_LIB

INCLUDEPATH += ./generatedFiles \
    ./../.. \
    . \
    ./generatedFiles/Debug

LIBS += -L"./../../executable" \
    -lIPXACTmodels \
	-lKactusAPI

DESTDIR = ../../executable/Plugins

DEPENDPATH += .
MOC_DIR += ./generatedFiles/debug
OBJECTS_DIR += debug
UI_DIR += ./generatedFiles
RCC_DIR += ./generatedFiles

include(SVDGeneratorPlugin.pri)

target.path = $$plugin_path
INSTALLS += target
