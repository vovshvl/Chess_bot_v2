cmake_minimum_required(VERSION 3.31)
project(chess_bot_new)

set(CMAKE_CXX_STANDARD 20)

add_executable(chess_bot_new
        Piece.cc
        engine.cc
        main.cc
        ChessGameDemo.hh
        tests/tests.cpp
        tests/tests.cpp
        tests/tests.cpp
        tests/tests.cpp
)

enable_testing()
add_subdirectory(extern/googletest)
add_executable(chessbot_tests
        tests/tests.cpp
        Piece.cc
        engine.cc
        board.cc
        Move.cc
        ChessGameDemo.cc
        tests/tests.cpp
        tests/tests.cpp
        tests/tests.cpp
)
target_link_libraries(chessbot_tests
        gtest
        gtest_main
)
add_test(NAME chessbot_tests COMMAND chessbot_tests)