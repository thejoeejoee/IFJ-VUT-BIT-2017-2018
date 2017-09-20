if (CMAKE_VERSION VERSION_LESS 3.2)
    set(UPDATE_DISCONNECTED_IF_AVAILABLE "")
else ()
    set(UPDATE_DISCONNECTED_IF_AVAILABLE "UPDATE_DISCONNECTED 1")
endif ()

include(utils/DownloadProject.cmake)

set(BUILD_GTEST ON CACHE BOOL "Builds the googletest subproject")
set(BUILD_GMOCK OFF CACHE BOOL "Builds the googlemock subproject")

download_project(
        PROJ googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG master
        ${UPDATE_DISCONNECTED_IF_AVAILABLE}
)
download_project(
        PROJ codecov
        GIT_REPOSITORY https://github.com/RWTH-HPC/CMake-codecov.git
        GIT_TAG master
        ${UPDATE_DISCONNECTED_IF_AVAILABLE}
)

# Prevent GoogleTest from overriding our compiler/linker options
# when building with Visual Studio
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)


add_subdirectory(${googletest_SOURCE_DIR} ${googletest_BINARY_DIR})

# When using CMake 2.8.11 or later, header path dependencies
# are automatically added to the gtest and gmock targets.
# For earlier CMake versions, we have to explicitly add the
# required directories to the header search path ourselves.
if (CMAKE_VERSION VERSION_LESS 2.8.11)
    include_directories("${gtest_SOURCE_DIR}/include"
            "${gmock_SOURCE_DIR}/include")
endif ()

# link downloaded codecov
set(CMAKE_MODULE_PATH "${CMAKE_BINARY_DIR}/codecov-src/cmake" ${CMAKE_MODULE_PATH})

# ifj2017_test
file(
        GLOB_RECURSE ifj2017_test_SRC
        LIST_DIRECTORIES false
        RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
        "test/*.cpp" "test/*.hpp"
)
add_executable(ifj2017_test ${ifj2017_SRC_no_main} ${ifj2017_test_SRC} ${VS_debug_visualizers})

target_link_libraries(ifj2017_test gtest)
add_test(NAME test COMMAND ifj2017_test)

enable_testing()
find_package(codecov)

# coverage
add_coverage(ifj2017_test)
coverage_evaluate()