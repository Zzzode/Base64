# Copyright 2023 Zzzode. All rights reserved.

cmake_minimum_required(VERSION 3.4.1)

set(ROOT_SRC "${CMAKE_SOURCE_DIR}")
set(CMAKE_DIR "${ROOT_SRC}/cmake")
set(SRC_DIR "${ROOT_SRC}/src")

include(${CMAKE_DIR}/compile_flags.cmake)

project(base64-wasm)

set(BASE64_INC "${SRC_DIR}")

file(GLOB BASE64_SRC ${SRC_DIR}/base64.cc)

add_library(base64 STATIC ${BASE64_SRC})
target_include_directories(base64 PRIVATE ${BASE64_INC})

add_executable(main ${SRC_DIR}/main.cc)

target_link_libraries(main base64)