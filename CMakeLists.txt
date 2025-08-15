cmake_minimum_required(VERSION 3.31)
project(chess_bot_new)

set(CMAKE_CXX_STANDARD 20)

add_executable(chess_bot_new
        Piece.cc
        engine.cc
        main.cc
        ChessGameDemo.hh
)

enable_testing()

include(FetchContent)
FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG        v1.15.2
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_executable(chessbot_tests
        tests/tests.cpp
        Piece.cc
        engine.cc
        board.cc
        Move.cc
        ChessGameDemo.cc
)

target_link_libraries(chessbot_tests
        GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(chessbot_tests)
