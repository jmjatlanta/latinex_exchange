cmake_minimum_required(VERSION 3.2)
cmake_policy(VERSION 3.25)
project(test)

include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
FetchContent_MakeAvailable(googletest)

file(GLOB src
    ../src/market.cpp
    ../src/order.cpp
    ../src/server.cpp
    ../src/client.cpp
    ../src/helper.cpp
    ../src/logger.cpp
    ../src/exchange_client.cpp
    ../src/exchange_server.cpp
    ../src/Myfix_classes.cpp
    ../src/Myfix_traits.cpp
    ../src/Myfix_types.cpp
    ../src/latinex_config.cpp
)

add_executable(test exchange.cpp fix.cpp data_feed.cpp ${src} )

target_include_directories( test PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/../liquibook/src
    ${CMAKE_CURRENT_LIST_DIR}/../nasdaq_itch_ouch/include
    ${POCO_INCLUDE_DIR}
    ${FIX8_DIR}/include
)

target_link_libraries( test 
    GTest::gtest
    GTest::gtest_main
    libzmq${CMAKE_SHARED_LIBRARY_SUFFIX}
    ${FIX8_LIB_DIR}/libfix8${CMAKE_SHARED_LIBRARY_SUFFIX}
    ${POCO_LIB_DIR}/libPocoFoundation${CMAKE_SHARED_LIBRARY_SUFFIX}
    ${POCO_LIB_DIR}/libPocoNet${CMAKE_SHARED_LIBRARY_SUFFIX}
)

add_executable( trade_bot trade_bot.cpp ${src} )

target_include_directories( trade_bot PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/../liquibook/src
    ${CMAKE_CURRENT_LIST_DIR}/../nasdaq_itch_ouch/include
    ${POCO_INCLUDE_DIR}
    ${FIX8_DIR}/include
)

target_link_libraries( trade_bot
    libzmq${CMAKE_SHARED_LIBRARY_SUFFIX}
    ${FIX8_LIB_DIR}/libfix8${CMAKE_SHARED_LIBRARY_SUFFIX}
    ${POCO_LIB_DIR}/libPocoFoundation${CMAKE_SHARED_LIBRARY_SUFFIX}
    ${POCO_LIB_DIR}/libPocoNet${CMAKE_SHARED_LIBRARY_SUFFIX}
)

