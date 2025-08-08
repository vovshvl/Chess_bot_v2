cmake_minimum_required(VERSION 3.31)
project(chess_bot_new)

set(CMAKE_CXX_STANDARD 20)

add_executable(chess_bot_new main.cpp
        Piece.cc
        engine.cc)
