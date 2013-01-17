cmake_minimum_required(VERSION 2.8)

if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_CURRENT_SOURCE_DIR})
    include(CMakeLists.internal)
else()
    include(CMakeLists.tomahawk)
endif()
