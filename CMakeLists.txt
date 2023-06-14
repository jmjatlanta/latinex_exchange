cmake_minimum_required(VERSION 3.2)
cmake_policy(VERSION 3.25)
project(latinex_exchange)

set(FIX8_DIR /home/jmjatlanta/Development/cpp/fix8_install)
set(FIX8_LIB_DIR ${FIX8_DIR}/lib)
set(POCO_DIR /home/jmjatlanta/Development/cpp/poco)
set(POCO_LIB_DIR ${POCO_DIR}/cmake-build/lib)

add_subdirectory(liquibook)
add_subdirectory(test)

file(GLOB src
    src/market.cpp
    src/order.cpp
    src/server.cpp
    src/client.cpp
    src/helper.cpp
    src/exchange_client.cpp
    src/exchange_server.cpp
    src/Myfix_classes.cpp
    src/Myfix_traits.cpp
    src/Myfix_types.cpp
    src/main.cpp
)

add_executable(latinex_exchange ${src} )

target_include_directories( latinex_exchange PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/liquibook/src
    ${CMAKE_CURRENT_LIST_DIR}/nasdaq_itch_ouch/include
    ${POCO_INCLUDE_DIR}
    ${FIX8_DIR}/include
)

target_link_libraries( latinex_exchange
    libzmq${CMAKE_SHARED_LIBRARY_SUFFIX}
    ${FIX8_LIB_DIR}/libfix8${CMAKE_SHARED_LIBRARY_SUFFIX}
    ${POCO_LIB_DIR}/libPocoFoundation${CMAKE_SHARED_LIBRARY_SUFFIX}
    ${POCO_LIB_DIR}/libPocoNet${CMAKE_SHARED_LIBRARY_SUFFIX}
)

