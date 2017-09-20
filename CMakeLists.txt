cmake_minimum_required(VERSION 3.0)

project(ifj2017)

#init
IF (CMAKE_COMPILER_IS_GNUCC)
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Werror -pedantic -std=c99")
ENDIF ()
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_C_STANDARD 99)

# Visual Studio debug visualizers
IF (MSVC)
    file(
            GLOB_RECURSE VS_debug_visualizers
            LIST_DIRECTORIES false
            RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
            "dev/debug/vs/*.natvis"
    )
ELSE ()
    set(VS_visualizers "")
ENDIF ()

# ifj2017
file(
        GLOB_RECURSE ifj2017_SRC
        LIST_DIRECTORIES false
        RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
        "src/*.c" "src/*.h"
)
add_executable(ifj2017 ${ifj2017_SRC} ${VS_debug_visualizers})

set(ifj2017_SRC_no_main ${ifj2017_SRC})
list(REMOVE_ITEM ifj2017_SRC_no_main "src/ifj2017.c")

# ifj2017_test
include(test/CMakeLists.txt)

# ifj2017_benchmark
include(benchmark/CMakeLists.txt)

# automatic source groups by folder structure
set(_all_sources ${ifj2017_test_SRC} ${ifj2017_benchmark_SRC} ${ifj2017_SRC} ${VS_debug_visualizers})
foreach (_source IN ITEMS ${_all_sources})
    get_filename_component(_source_path "${_source}" PATH)
    IF (MSVC)
        string(REPLACE "/" "\\" _source_path_msvc "${_source_path}")
        set(_source_path ${_source_path_msvc})
    ENDIF ()
    source_group("${_source_path}" FILES "${_source}")
endforeach ()