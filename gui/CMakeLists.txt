cmake_minimum_required(VERSION 3.16)

# Set the project name
project(gbcr LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# get Git HASH
execute_process(
    COMMAND git log -1 --format=%h
    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
    OUTPUT_VARIABLE GIT_HASH
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
add_compile_definitions(GIT_HASH="${GIT_HASH}")

# Find Qt6 packages
find_package(Qt5 REQUIRED COMPONENTS Widgets SerialPort)

add_executable(gbcr
    src/main.cpp
    src/flashthread.cpp
    src/gameboycamera.cpp
    src/gameboydata.cpp
    src/ioworker.cpp
    src/logwindow.cpp
    src/mainwindow.cpp
    src/readramthread.cpp
    src/readthread.cpp
    src/serial_interface.cpp
    src/writeramthread.cpp
    resources.qrc
)

if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set(MACOSX TRUE)
    set_target_properties(gbcr PROPERTIES
        MACOSX_BUNDLE TRUE
    )
endif()

target_link_libraries(gbcr PRIVATE Qt5::Core Qt5::Widgets Qt5::SerialPort)

set_target_properties(gbcr PROPERTIES
    WIN32_EXECUTABLE ON
    MACOSX_BUNDLE ON
)
