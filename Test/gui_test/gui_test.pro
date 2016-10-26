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
    ../../FW/gui/sm_desktop.h \
    ../../FW/hw/inc/sm_hw_storage.h \
    ../../FW/hw/inc/sm_hw_keyboard.h \
    ../../FW/hw/inc/sm_hw_battery.h \
    ../../FW/inc/sm_crc.h \
    ../../FW/hal/inc/sm_hal_sys_timer.h \
    ../../FW/gui/sm_display.h \
    ../../FW/gui/sm_main_menu.h \
    ../../FW/gui/sm_edit_menu.h \
    ../../FW/gui/sm_editor.h
    ../../FW/gui/sm_main_menu.h

SOURCES       = \
    main.cpp \
    renderarea.cpp \
    window.cpp \
    ../../FW/gui/sm_canvas.cpp \
    ../../FW/gui/sm_texture.cpp \
    ../../FW/gui/sm_animator.cpp \
    ../../FW/gui/sm_font.cpp \
    ../../FW/gui/sm_image.cpp \
    ../../FW/gui/sm_desktop.cpp \
    ../../FW/hw/src/sm_hw_storage.cpp \
    ../../FW/hw/src/sm_hw_keyboard.cpp \
    ../../FW/hw/src/sm_hw_battery.cpp \
    ../../FW/src/sm_crc.cpp \
    ../../FW/hal/src/sm_hal_sys_timer.cpp \
    ../../FW/gui/sm_display.cpp \
    ../../FW/gui/sm_main_menu.cpp \
    ../../FW/hal/src/sm_hal_rtc.cpp \
    ../../FW/gui/sm_edit_menu.cpp \
    ../../FW/gui/sm_editor.cpp

RESOURCES     =
