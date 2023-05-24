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
    fix.cpp
    ../src/market.cpp
    ../src/order.cpp
    ../src/server.cpp
    ../src/Myfix_classes.cpp
    ../src/Myfix_traits.cpp
    ../src/Myfix_types.cpp
)

add_executable(test ${src} )

target_include_directories( test PRIVATE
    ${CMAKE_SOURCE_DIR}/liquibook/src
    ${FIX8_DIR}/include
)

target_link_libraries( test 
    GTest::gtest
    GTest::gtest_main
    ${FIX8_LIB_DIR}/libfix8.so
    ${POCO_LIB_DIR}/libPocoFoundation.so
    ${POCO_LIB_DIR}/libPocoNet.so
)
