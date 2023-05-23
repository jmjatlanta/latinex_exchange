cmake_minimum_required(VERSION 3.2)

project(test)

include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
FetchContent_MakeAvailable(googletest)

file(GLOB src
    exchange.cpp
    ../src/market.cpp
    ../src/order.cpp
)

add_executable(test ${src} )

target_include_directories( test PRIVATE
    ${CMAKE_SOURCE_DIR}/liquibook/src
)

target_link_libraries( test 
    GTest::gtest
    GTest::gtest_main
)
