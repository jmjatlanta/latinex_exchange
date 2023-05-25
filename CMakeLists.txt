cmake_minimum_required(VERSION 3.2)
project(latinex_exchange)

set(FIX8_DIR /home/jmjatlanta/Development/cpp/fix8_install)
set(FIX8_LIB_DIR ${FIX8_DIR}/lib)
set(POCO_DIR /home/jmjatlanta/Development/cpp/poco)
set(POCO_LIB_DIR ${POCO_DIR}/cmake-build/lib)

add_subdirectory(liquibook)
add_subdirectory(test)
