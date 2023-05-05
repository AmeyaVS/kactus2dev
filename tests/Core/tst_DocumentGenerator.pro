TEMPLATE = app

TARGET = tst_DocumentGenerator

QT += core xml gui widgets testlib printsupport svg
CONFIG += c++11 testcase console

win32:CONFIG(release, debug|release) {
    LIBS += -L$$PWD/../../x64/executable/ -lIPXACTmodels
    LIBS += -L$$PWD/../../x64/executable/ -lKactusAPI
    DESTDIR = ./release
}
else:win32:CONFIG(debug, debug|release) {
    LIBS += -L$$PWD/../../x64/executable/ -lIPXACTmodelsd
    LIBS += -L$$PWD/../../x64/executable/ -lKactusAPId
    DESTDIR = ./debug
}
else:unix {
    LIBS += -L$$PWD/../../executable/ -lIPXACTmodels
    LIBS += -L$$PWD/../../executable/ -lKactusAPI
    DESTDIR = ./release
}

#DESTDIR = ./release

INCLUDEPATH += $$DESTDIR
INCLUDEPATH += $$PWD/../../
INCLUDEPATH += $$PWD/../../KactusAPI/include

DEPENDPATH += .
DEPENDPATH += ../../

OBJECTS_DIR += $$DESTDIR

MOC_DIR += ./generatedFiles
UI_DIR += ./generatedFiles
RCC_DIR += ./generatedFiles
include(tst_DocumentGenerator.pri)
