# Copyright 2023 Zzzode. All rights reserved.

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(RUST_BUILD_TYPE "Debug")
    add_definitions(-DENABLE_DEBUG)
else ()
    set(RUST_BUILD_TYPE "Release")
endif ()

# set(CMAKE_C_COMPILER "${EMCC}" CACHE FILEPATH "emscripten c compiler" FORCE)
# set(CMAKE_CXX_COMPILER "${EMXX}" CACHE FILEPATH "emscripten c++ compiler" FORCE)

# The common options using by both c and cxx
set(COMPILE_C_FLAGS "")
set(COMPILE_CXX_FLAGS "-std=gnu++17")

set(CMAKE_C_FLAGS "${COMPILE_C_FLAGS}")
set(CMAKE_C_FLAGS_DEBUG "-g")
set(CMAKE_C_FLAGS_RELEASE "-O3 -DNDEBUG")

# the actual options for cxx
set(CMAKE_CXX_FLAGS "${COMPILE_CXX_FLAGS}")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")

# The linker options
set(CMAKE_C_LINK_FLAGS "")
set(CMAKE_CXX_LINK_FLAGS "")
set(CMAKE_SHARED_LINKER_FLAGS "${LINK_FLAGS}" CACHE STRING "")
set(CMAKE_EXE_LINKER_FLAGS "${LINK_FLAGS}" CACHE STRING "")
set(CMAKE_MODULE_LINKER_FLAGS "${LINK_FLAGS}" CACHE STRING "")

#-sEXPORTED_FUNCTIONS=['_malloc'] \
#-sEXPORTED_RUNTIME_METHODS=['ccall','cwrap','intArrayFromString','intArrayToString','allocateUTF8','stringToUTF8Array'] \
