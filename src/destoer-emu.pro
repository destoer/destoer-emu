######################################################################
# Automatically generated by qmake (3.1) Mon Nov 25 15:39:42 2019
######################################################################

TEMPLATE = app
TARGET = destoer-emu
INCLUDEPATH += .

# You can make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# Please consult the documentation of the deprecated API in order to know
# how to port your code away from it.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

# Input
HEADERS += src/fmt/chrono.h \
           src/fmt/color.h \
           src/fmt/core.h \
           src/fmt/format-inl.h \
           src/fmt/format.h \
           src/fmt/locale.h \
           src/fmt/ostream.h \
           src/fmt/posix.h \
           src/fmt/printf.h \
           src/fmt/ranges.h \
           src/fmt/time.h \
           src/headers/cpu.h \
           src/headers/forward_def.h \
           src/headers/gb.h \
           src/headers/lib.h \
           src/headers/mem_constants.h \
           src/headers/memory.h \
           src/headers/disass.h \
           src/headers/ppu.h \
           src/headers/rom.h \
           src/qt/qt_window.h \
           src/qt/framebuffer.h \
           src/sdl/sdl_window.h
SOURCES += src/cpu.cpp \
           src/gb.cpp \
           src/lib.cpp \
           src/main.cpp \
           src/memory.cpp \
           src/opcode.cpp \
           src/opcode_cb.cpp \
           src/banking.cpp \
           src/instr.cpp \
           src/disass.cpp \
           src/ppu.cpp \
           src/fmt/format.cpp \
           src/fmt/posix.cpp \
           src/sdl/sdl_window.cpp \
           src/qt/qt_window.cpp \
           src/qt/framebuffer.cpp
	

QMAKE_CXXFLAGS += -Wall -Werror -std=c++17 -g -DFRONTEND_QT
QMAKE_LFLAGS += -lSDL2
QT += widgets opengl
