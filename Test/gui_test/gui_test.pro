QT += widgets

CONFIG += c++11

INCLUDEPATH += ../../FW/gui \
               ../../FW/inc \
               ../../FW/hw/inc \
               ../../FW/hal/inc \

DEFINES += BPP=1 PACK_VERT PC_SOFTWARE

HEADERS       = \
    renderarea.h \
    window.h \
    ../../FW/gui/sm_canvas.h \
    ../../FW/gui/sm_texture.h \
    ../../FW/gui/sm_animator.h \
    ../../FW/gui/sm_font.h \
    ../../FW/gui/sm_image.h \
    ../../FW/hw/inc/sm_hw_storage.h \
    ../../FW/hw/inc/sm_hw_keyboard.h \
    ../../FW/inc/sm_crc.h \
    ../../FW/hal/inc/sm_hal_sys_timer.h

SOURCES       = \
    main.cpp \
    renderarea.cpp \
    window.cpp \
    ../../FW/gui/sm_canvas.cpp \
    ../../FW/gui/sm_texture.cpp \
    ../../FW/gui/sm_animator.cpp \
    ../../FW/gui/sm_font.cpp \
    ../../FW/gui/sm_image.cpp \
    ../../FW/hw/src/sm_hw_storage.cpp \
    ../../FW/hw/src/sm_hw_keyboard.cpp \
    ../../FW/src/sm_crc.cpp \
    ../../FW/hal/src/sm_hal_sys_timer.cpp

RESOURCES     =
