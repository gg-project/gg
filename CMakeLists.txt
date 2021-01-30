project(gg)
cmake_minimum_required(VERSION 3.16)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
list(APPEND CMAKE_PREFIX_PATH ${PROJECT_SOURCE_DIR}/third_party/install)

set(Protobuf_USE_STATIC_LIBS TRUE)
include(FindProtobuf)
message(STATUS "Protobuf binary at ${Protobuf_PROTOC_EXECUTABLE}")

set(BUILD_SHARED_LIBS FALSE)
option(CMAKE_BUILD_TYPE "Kind of Build" Release)
if (NOT CMAKE_BUILD_TYPE OR CMAKE_BUILD_TYPE STREQUAL "")
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "" FORCE)
endif()

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -s")

set(OPENSSL_USE_STATIC_LIBS TRUE)
if (IS_DIRECTORY "${PROJECT_SOURCE_DIR}/third_party/openssl/")
    set(OPENSSL_ROOT_DIR ${PROJECT_SOURCE_DIR}/third_party/install)
endif()
include(FindOpenSSL)
if(NOT ${OPENSSL_FOUND})
    message(FATAL_ERROR "No openssl")
endif()

add_subdirectory(third_party/hiredis EXCLUDE_FROM_ALL)

include_directories(src)
include_directories(${CMAKE_BINARY_DIR}/src)
link_directories(third_party/install/lib)
add_subdirectory(src)
