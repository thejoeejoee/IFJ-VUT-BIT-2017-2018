if (CMAKE_VERSION VERSION_LESS 3.2)
    set(UPDATE_DISCONNECTED_IF_AVAILABLE "")
else ()
    set(UPDATE_DISCONNECTED_IF_AVAILABLE "UPDATE_DISCONNECTED 1")
endif ()

include(utils/DownloadProject.cmake)

set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "Enable testing of the benchmark library.")

download_project(
        PROJ googlebenchmark
        GIT_REPOSITORY https://github.com/google/benchmark.git
        GIT_TAG master
        ${UPDATE_DISCONNECTED_IF_AVAILABLE}
)


add_subdirectory(${googlebenchmark_SOURCE_DIR} ${googlebenchmark_BINARY_DIR})

# When using CMake 2.8.11 or later, header path dependencies
# are automatically added to targets.
# For earlier CMake versions, we have to explicitly add the
# required directories to the header search path ourselves.
if (CMAKE_VERSION VERSION_LESS 2.8.11)
    include_directories("${googlebenchmark_SOURCE_DIR}/include")
endif ()


# ifj2017_benchmark
file(
        GLOB_RECURSE ifj2017_benchmark_SRC
        LIST_DIRECTORIES false
        RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
        "benchmark/*.cpp" "benchmark/*.hpp" "test/utils/*.cpp"
)

add_executable(ifj2017_benchmark ${ifj2017_SRC_no_main} ${ifj2017_benchmark_SRC} ${VS_debug_visualizers})

target_link_libraries(ifj2017_benchmark benchmark)
