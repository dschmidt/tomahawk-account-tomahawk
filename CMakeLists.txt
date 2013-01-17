cmake_minimum_required(VERSION 2.8)

find_package(OpenSSL REQUIRED)

set(WEBSOCKETPP_MAJOR_VERSION 0)
set(WEBSOCKETPP_MINOR_VERSION 2)
set(WEBSOCKETPP_PATCH_VERSION 0)
set(WEBSOCKETPP_VERSION "${WEBSOCKETPP_MAJOR_VERSION}.${WEBSOCKETPP_MINOR_VERSION}.${WEBSOCKETPP_PATCH_VERSION}" )
set(WEBSOCKETPP_VERSION_SONAME "${WEBSOCKETPP_MAJOR_VERSION}.${WEBSOCKETPP_MINOR_VERSION}")
set( WEBSOCKETPP_LIBRARY tomahawk_websocketpp )
set( WEBSOCKETPP_LIBRARIES ${WEBSOCKETPP_LIBRARY} )

# if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_CURRENT_SOURCE_DIR})
#     include(CMakeLists.internal)
# else()
#     include(CMakeLists.tomahawk)
# endif()

find_package(Tomahawk REQUIRED)

find_package(Boost COMPONENTS regex thread filesystem random system REQUIRED)

set( WEBSOCKETPP_INCLUDE_DIR ${CMAKE_BINARY_DIR}/thirdparty/websocketpp/src/include )
add_subdirectory( ${CMAKE_SOURCE_DIR}/thirdparty/websocketpp )

include_directories(${WEBSOCKETPP_INCLUDE_DIR})

# message(FATAL_ERROR "tomahawk: ${TOMAHAWK_INCLUDE_DIRS}")

include( ${TOMAHAWK_USE_FILE} )

include_directories( ${TOMAHAWK_INCLUDE_DIRS} )

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
