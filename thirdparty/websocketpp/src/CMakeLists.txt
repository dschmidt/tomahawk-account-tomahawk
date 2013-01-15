project(websocketpp)

include_directories(${Boost_INCLUDE_DIRS})

find_package(Boost COMPONENTS regex thread filesystem REQUIRED)
find_package(OpenSSL REQUIRED)
# find_package

set(websocketpp_SRCS base64/base64.cpp
                     md5/md5.c
                     messages/data.cpp
                     network_utilities.cpp
                     processors/hybi_header.cpp
                     processors/hybi_util.cpp
                     sha1/sha1.cpp
                     uri.cpp )


file(GLOB websocketpp_root_HDRS  *.hpp)
file(GLOB websocketpp_http_HDRS  http/*.hpp)
file(GLOB websocketpp_logger_HDRS  logger/*.hpp)
file(GLOB websocketpp_messages_HDRS  messages/*.hpp)
file(GLOB websocketpp_processors_HDRS  processors/*.hpp)
file(GLOB websocketpp_rng_HDRS  rng/*.hpp)
file(GLOB websocketpp_roles_HDRS  roles/*.hpp)
file(GLOB websocketpp_sockets_HDRS  sockets/*.hpp)
file(GLOB websocketpp_utf8_validator_HDRS  utf8_validator/*.hpp)

set(websocketpp_base64_HDRS base64/base64.h)
set(websocketpp_sha1_HDRS sha1/sha1.h)
set(websocketpp_md5_HDRS md5/md5.h md5/md5.hpp)

set(websocketpp_HDRS base64/base64.h sha1/sha1.h md5/md5.hpp ${websocketpp_HDRS})

set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wl,--no-undefined" )
add_library(tomahawk_websocketpp SHARED ${websocketpp_SRCS})
set_target_properties(tomahawk_websocketpp PROPERTIES COMPILE_FLAGS -fPIC)
target_link_libraries(tomahawk_websocketpp
    ${Boost_LIBRARIES}
    ${OPENSSL_LIBRARIES}
)

include(GNUInstallDirs)
install(TARGETS tomahawk_websocketpp
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    BUNDLE DESTINATION library
)

file(COPY ${websocketpp_root_HDRS} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/include/websocketpp)
file(COPY ${websocketpp_http_HDRS} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/include/websocketpp/http)
file(COPY ${websocketpp_logger_HDRS} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/include/websocketpp/logger)
file(COPY ${websocketpp_messages_HDRS} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/include/websocketpp/messages)
file(COPY ${websocketpp_processors_HDRS} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/include/websocketpp/processors)
file(COPY ${websocketpp_rng_HDRS} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/include/websocketpp/rng)
file(COPY ${websocketpp_roles_HDRS} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/include/websocketpp/roles)
file(COPY ${websocketpp_sockets_HDRS} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/include/websocketpp/sockets)
file(COPY ${websocketpp_utf8_validator_HDRS} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/include/websocketpp/utf8_validator)
file(COPY ${websocketpp_base64_HDRS} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/include/websocketpp/base64)
file(COPY ${websocketpp_sha1_HDRS} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/include/websocketpp/sha1)
file(COPY ${websocketpp_md5_HDRS} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/include/websocketpp/md5)
