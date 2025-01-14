####################################################################################################
 #
 #
 #   SST39SF0x0-programmer
 #   Copyright (C) 2021 Ivo Filot <i.a.w.filot@tue.nl>
 #
 #   This program is free software: you can redistribute it and/or modify
 #   it under the terms of the GNU Lesser General Public License as
 #   published by the Free Software Foundation, either version 3 of the
 #   License, or (at your option) any later version.
 #
 #   This program is distributed in the hope that it will be useful,
 #   but WITHOUT ANY WARRANTY; without even the implied warranty of
 #   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 #  GNU General Public License for more details.
 #
 #   You should have received a copy of the GNU General Public license
 #   along with this program.  If not, see <https://www.gnu.org/licenses/>.
 #
 #
####################################################################################################

HEADERS       = src/mainwindow.h \
                src/flashthread.h \
                src/gameboycamera.h \
                src/gameboydata.h \
                src/ioworker.h \
                src/logwindow.h \
                src/readramthread.h \
                src/readthread.h \
                src/serial_interface.h \
                src/config.h \
                src/writeramthread.h

SOURCES       = src/main.cpp \
                src/flashthread.cpp \
                src/gameboycamera.cpp \
                src/gameboydata.cpp \
                src/ioworker.cpp \
                src/logwindow.cpp \
                src/mainwindow.cpp \
                src/readramthread.cpp \
                src/readthread.cpp \
                src/serial_interface.cpp \
                src/writeramthread.cpp

QT           += core gui widgets serialport
CONFIG       += force_debug_info

# ensures that help files can be accessed during development
DEFINES += SRCDIR=\\\"$$PWD/\\\"

# documentation files
docs.files += $$PWD/doc
docs.path   = $$target.path

RESOURCES += \
    resources.qrc
