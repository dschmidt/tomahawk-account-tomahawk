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

tomahawk_add_plugin(tomahawk
    TYPE account
    EXPORT_MACRO ACCOUNTDLLEXPORT_PRO
    SOURCES
        TomahawkAccount.cpp
        TomahawkAccountConfig.cpp
        sip/WebSocketWrapper.cpp
        sip/TomahawkSip.cpp
    UI
        TomahawkAccountConfig.ui
    LINK_LIBRARIES
        ${TOMAHAWK_LIBRARIES}
        ${QCA2_LIBRARIES}
)


#add_tomahawk_plugin(facebook
#    TYPE account
#    EXPORT_MACRO ACCOUNTDLLEXPORT_PRO
#    SOURCES
#        facebook/FacebookAccount.cpp
#        facebook/FacebookAccountConfig.cpp
#        facebook/FacebookSip.cpp
#        facebook/FacebookAuthDialog.cpp
#    UI
#        facebook/FacebookAccountConfig.ui
#    LINK_LIBRARIES
#        ${TOMAHAWK_LIBRARIES}
#        tomahawk_account_tomahawk
#)
