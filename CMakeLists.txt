cmake_minimum_required(VERSION 3.28)
project(tetris)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_FLAGS
        "-lncurses"
)

add_executable(tetris
        main.cpp
        Tetromino.h
        Tetromino.cpp
        Tetris.h
        Tetris.cpp
)
