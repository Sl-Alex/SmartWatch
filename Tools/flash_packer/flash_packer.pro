TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

DEFINES += BPP=1
DEFINES += PACK_VERT

SOURCES += main.cpp \
    parser_bin.cpp \
    parser_txt.cpp \
    parser_pbm.cpp

HEADERS += \
    global.h
