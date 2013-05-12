cmake_minimum_required(VERSION 2.8)
CMAKE_POLICY(SET CMP0017 NEW)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake/Modules")

if(NOT TOMAHAWK_LIBRARIES)
    message(STATUS "BUILDING OUTSIDE TOMAHAWK")
    find_package(Tomahawk REQUIRED)
else()
    message(STATUS "BUILDING INSIDE TOMAHAWK")
    set(TOMAHAWK_USE_FILE "${CMAKE_SOURCE_DIR}/TomahawkUse.cmake")
endif()
include( ${TOMAHAWK_USE_FILE} )

find_package(OpenSSL REQUIRED)
find_package(QCA2 REQUIRED)

find_package(Boost COMPONENTS regex thread filesystem random system date_time REQUIRED)

if(${Boost_VERSION} LESS 105300)
    add_definitions(-D BOOST_NO_LOCK_GUARD)
endif()

include_directories(
    ${WEBSOCKETPP_INCLUDE_DIR}
    ${TOMAHAWK_INCLUDE_DIRS}
    ${QCA2_INCLUDE_DIR}
)

add_definitions (-D_WEBSOCKETPP_CPP11_STL_)

if(APPLE)
    # http://stackoverflow.com/questions/7226753/osx-lion-xcode-4-1-how-do-i-setup-a-c0x-project/7236451#7236451
    add_definitions(-std=c++11 -stdlib=libc++ -U__STRICT_ANSI__)
    set(PLATFORM_SPECIFIC_LINK_LIBRARIES "/usr/lib/libc++.dylib")
else()
    add_definitions(-std=c++0x)
endif()

tomahawk_add_plugin(hatchet
    TYPE account
    EXPORT_MACRO ACCOUNTDLLEXPORT_PRO
    SOURCES
        HatchetAccount.cpp
        HatchetAccountConfig.cpp
        sip/WebSocket.cpp
        sip/WebSocketThreadController.cpp
        sip/HatchetSip.cpp
    UI
        HatchetAccountConfig.ui
    LINK_LIBRARIES
        ${TOMAHAWK_LIBRARIES}
        ${QCA2_LIBRARIES}
        ${PLATFORM_SPECIFIC_LINK_LIBRARIES}
)

