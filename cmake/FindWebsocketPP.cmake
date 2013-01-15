set( WEBSOCKETPP_INCLUDE_DIR ${CMAKE_BINARY_DIR}/thirdparty/websocketpp/src/include )
set( WEBSOCKETPP_LIBRARY tomahawk_websocketpp )
set( WEBSOCKETPP_LIBRARIES ${WEBSOCKETPP_LIBRARY} )

add_subdirectory( ${CMAKE_CURRENT_LIST_DIR}/../thirdparty/websocketpp/src )
