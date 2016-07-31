TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

DEFINES += BPP=1
DEFINES += PACK_VERT

SOURCES += main.cpp \
    parser_bin.cpp \
    parser_txt.cpp \
    parser_pbm.cpp \
    parser_font.cpp \
    pbm_image.cpp \
    flash_image.cpp

HEADERS += \
    global.h \
    pbm_image.h \
    flash_image.h
