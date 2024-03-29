cmake_minimum_required(VERSION 3.10.0)
project(kachow)

# Make sure developers do not run cmake in the main project directory, to keep
# build artifacts from becoming clutter
if (${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
    message(FATAL_ERROR "In-source builds not allowed.
    Please make a new directory (called a build directory) and run CMake from there.
    You may need to remove CMakeCache.txt." )
endif()

# Turn on debug builds if we are building in a devel shell
if (CMAKE_BUILD_TYPE STREQUAL "" AND "$ENV{DEVEL_SHELL}" STREQUAL "1")
    message(STATUS "Setting debug build type by default in devel shell")
    set(CMAKE_BUILD_TYPE Debug)
endif()

# Output binaries to a sub directory "bin"
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Enable cmake testing
include(CTest)
enable_testing()

# Enable GoogleTest
include(GoogleTest)
add_subdirectory(/usr/src/googletest googletest)

# Enable Boost
# Use static libraries so binaries can be deployed without a full boost install
set(Boost_USE_STATIC_LIBS ON)
find_package(Boost 1.50 REQUIRED COMPONENTS filesystem system log_setup log thread)
message(STATUS "Boost version: ${Boost_VERSION}")

# jwt-cpp
set(OPENSSL_USE_STATIC_LIBS TRUE)
find_package(OpenSSL REQUIRED)
add_subdirectory(jwt-cpp jwt-cpp)

include_directories(include jwt-cpp/include)

add_library(session_lib src/session.cc)
add_library(server_lib src/server.cc)
add_library(config_parser_lib src/config_parser.cc)
add_library(request_handler_lib src/request_handler.cc src/request_handler_factory.cc src/filesystem.cc)
add_library(token_auth_lib src/token_auth.cc)
target_link_libraries(server_lib session_lib config_parser_lib request_handler_lib token_auth_lib jwt-cpp::jwt-cpp Boost::log OpenSSL::Crypto)

# Include -pthread when compiling
# https://stackoverflow.com/questions/5395309/how-do-i-force-cmake-to-include-pthread-option-during-compilation
set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package(Threads REQUIRED)

add_executable(server src/server_main.cc)
target_link_libraries(server session_lib server_lib config_parser_lib Boost::filesystem Boost::system Boost::log_setup Boost::log Boost::thread Threads::Threads)

# TODO(!): Update test executable name, srcs, and deps
add_executable(config_parser_test tests/config_parser_test.cc)
target_link_libraries(config_parser_test config_parser_lib gtest_main)

add_executable(server_test tests/server_test.cc)
target_link_libraries(server_test session_lib server_lib config_parser_lib request_handler_lib Boost::filesystem Boost::log  Boost::system Threads::Threads Boost::thread gtest_main gcov)

add_executable(session_test tests/session_test.cc)
target_link_libraries(session_test session_lib server_lib request_handler_lib Boost::log  Boost::filesystem gtest_main gcov)

add_executable(request_handler_test tests/request_handler_test.cc)
target_link_libraries(request_handler_test request_handler_lib session_lib server_lib config_parser_lib request_handler_lib Boost::log Boost::filesystem Boost::system Boost::thread Threads::Threads gtest_main gcov)

add_executable(token_auth_test tests/token_auth_test.cc)
target_link_libraries(token_auth_test token_auth_lib OpenSSL::Crypto gtest_main gcov Boost::log)

# add_executable(example_lib_test tests/example_lib_test.cc)
# target_link_libraries(example_lib_test example_lib gtest_main)

# TODO(!): Update with test binary
add_test(NAME integration_test
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/tests/integration_test.sh
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
add_test(NAME 404_test
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/tests/404_test.sh
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
add_test(NAME api_test
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/tests/api_test.sh
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
add_test(NAME multithreading_test
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/tests/multithread_test.sh
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

gtest_discover_tests(config_parser_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
gtest_discover_tests(server_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
gtest_discover_tests(session_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
gtest_discover_tests(request_handler_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
gtest_discover_tests(token_auth_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
# gtest_discover_tests(example_lib_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)

# TODO(!): Update with target/test targets
include(cmake/CodeCoverageReportConfig.cmake)
generate_coverage_report(TARGETS server session_lib server_lib config_parser_lib request_handler_lib token_auth_lib TESTS config_parser_test server_test session_test request_handler_test token_auth_test)
