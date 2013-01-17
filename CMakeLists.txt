cmake_minimum_required(VERSION 2.8)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

find_package(Tomahawk REQUIRED)
include( ${TOMAHAWK_USE_FILE} )
include_directories( ${TOMAHAWK_INCLUDE_DIRS} )


find_package(WebSocketPP REQUIRED)
include_directories(${WEBSOCKETPP_INCLUDE_DIR})

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
        ${WEBSOCKETPP_LIBRARIES}
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
