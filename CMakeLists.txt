cmake_minimum_required(VERSION 3.16)
project(sylar VERSION 0.1.0)
include(cmake/utils.cmake)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_definitions(-Wall -Werror -Wno-unused-function -Wno-builtin-macro-redefined -Wno-deprecated-declarations)
include_directories(/usr/local/include)
find_library(YAMLCPP yaml-cpp)
find_library(PTHREAD pthread)
find_library(PTHREAD pthread)
include(CTest)
enable_testing()


set(LIB_SRC
    src/util.cc
    src/fiber.cc
    src/fdmanager.cc
    src/log.cc
    src/config.cc
    src/thread.cc
    src/mutex.cc
    src/scheduler.cc
    src/iomanager.cc
    src/timer.cc
    src/hook.cc
    src/address.cc
    src/socket.cc
    src/bytearray.cc
    src/http/http.cc
    src/http/http11_parser.rl.cc
    src/http/httpclient_parser.rl.cc
    src/http/http_parser.cc
    src/tcp_server.cc
    src/stream.cc
    src/socket_stream.cc
    src/http/http_session.cc
    src/http/http_server.cc)

add_library(sylar SHARED ${LIB_SRC})
force_redefine_file_macro_for_sources(sylar)

set(LIB_LIB
    sylar
    dl
    yaml-cpp
    pthread
    )

add_executable(test_log tests/test_log.cc)
add_dependencies(test_log sylar)
force_redefine_file_macro_for_sources(test_log)
target_link_libraries(test_log ${LIB_LIB})

add_executable(test_config tests/test_config.cc)
add_dependencies(test_config sylar)
force_redefine_file_macro_for_sources(test_config)
target_link_libraries(test_config ${LIB_LIB})

add_executable(test_pthread tests/test_pthread.cc)
add_dependencies(test_pthread sylar)
force_redefine_file_macro_for_sources(test_pthread)
target_link_libraries(test_pthread ${LIB_LIB})

add_executable(test_util tests/test_util.cc)
add_dependencies(test_util sylar)
force_redefine_file_macro_for_sources(test_util)
target_link_libraries(test_util ${LIB_LIB})

add_executable(test_fiber tests/test_fiber.cc)
add_dependencies(test_fiber sylar)
force_redefine_file_macro_for_sources(test_fiber)
target_link_libraries(test_fiber ${LIB_LIB})

add_executable(test_scheduler tests/test_scheduler.cc)
add_dependencies(test_scheduler sylar)
force_redefine_file_macro_for_sources(test_scheduler)
target_link_libraries(test_scheduler ${LIB_LIB})

add_executable(test_iomanager tests/test_iomanager.cc)
add_dependencies(test_iomanager sylar)
force_redefine_file_macro_for_sources(test_iomanager)
target_link_libraries(test_iomanager ${LIB_LIB})

add_executable(test_hook tests/test_hook.cc)
add_dependencies(test_hook sylar)
force_redefine_file_macro_for_sources(test_hook)
target_link_libraries(test_hook ${LIB_LIB})

add_executable(test_address tests/test_address.cc)
add_dependencies(test_address sylar)
force_redefine_file_macro_for_sources(test_address)
target_link_libraries(test_address ${LIB_LIB})

add_executable(test_socket tests/test_socket.cc)
add_dependencies(test_socket sylar)
force_redefine_file_macro_for_sources(test_socket)
target_link_libraries(test_socket ${LIB_LIB})

add_executable(test_bytearray tests/test_bytearray.cc)
add_dependencies(test_bytearray sylar)
force_redefine_file_macro_for_sources(test_bytearray)
target_link_libraries(test_bytearray ${LIB_LIB})

add_executable(test_http tests/test_http.cc)
add_dependencies(test_http sylar)
force_redefine_file_macro_for_sources(test_http)
target_link_libraries(test_http ${LIB_LIB})

add_executable(test_http_parser tests/test_http_parser.cc)
add_dependencies(test_http_parser sylar)
force_redefine_file_macro_for_sources(test_http_parser)
target_link_libraries(test_http_parser ${LIB_LIB})

add_executable(test_tcp_server tests/test_tcp_server.cc)
add_dependencies(test_tcp_server sylar)
force_redefine_file_macro_for_sources(test_tcp_server)
target_link_libraries(test_tcp_server ${LIB_LIB})

add_executable(echo_server example/echo_server.cc)
add_dependencies(echo_server sylar)
force_redefine_file_macro_for_sources(echo_server)
target_link_libraries(echo_server ${LIB_LIB})

add_executable(test_http_server tests/test_http_server.cc)
add_dependencies(test_http_server sylar)
force_redefine_file_macro_for_sources(test_http_server)
target_link_libraries(test_http_server ${LIB_LIB})

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
