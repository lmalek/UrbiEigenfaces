cmake_minimum_required (VERSION 2.8)

project (ueigenfaces)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
set(SRC_PATH ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(TESTS_PATH ${CMAKE_CURRENT_SOURCE_DIR}/tests)

add_subdirectory (${SRC_PATH})
