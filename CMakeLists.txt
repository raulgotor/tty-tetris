cmake_minimum_required(VERSION 3.28)
project(tetris)

set(CMAKE_CXX_STANDARD 17)


find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIR})

add_executable(tetris
        main.cpp
        Tetromino.h
        Tetromino.cpp
        Tetris.h
        Tetris.cpp
)

target_link_libraries(tetris PRIVATE ${CURSES_LIBRARIES})

include(GNUInstallDirs)
install(TARGETS tetris
        RUNTIME
        DESTINATION "${CMAKE_INSTALL_BINDIR}"
        COMPONENT Runtime
)