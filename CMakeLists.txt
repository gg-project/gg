project(gg)
cmake_minimum_required(VERSION 2.8)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(FindProtobuf)
if(NOT ${Protobuf_FOUND})
    message(FATAL_ERROR "No openssl")
endif()
message(STATUS "Protobuf binary at ${Protobuf_PROTOC_EXECUTABLE}")

set(BUILD_SHARED_LIBS FALSE)
option(CMAKE_BUILD_TYPE "Kind of Build" Release)
if (NOT CMAKE_BUILD_TYPE OR CMAKE_BUILD_TYPE STREQUAL "")
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "" FORCE)
endif()

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -s")

include(FindOpenSSL)
if(NOT ${OPENSSL_FOUND})
    message(FATAL_ERROR "No openssl")
endif()

#add_subdirectory(third_party/hiredis EXCLUDE_FROM_ALL)

include_directories(src)
include_directories(${CMAKE_BINARY_DIR}/src)
add_subdirectory(src)
