if (CMAKE_VERSION VERSION_LESS 3.2)
    set(UPDATE_DISCONNECTED_IF_AVAILABLE "")
else ()
    set(UPDATE_DISCONNECTED_IF_AVAILABLE "UPDATE_DISCONNECTED 1")
endif ()

set(BUILD_GTEST ON CACHE INTERNAL "Builds the googletest subproject")
set(BUILD_GMOCK OFF CACHE INTERNAL "Builds the googlemock subproject")

download_project(
        PROJ googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
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

# ifj2017_test
file(
        GLOB_RECURSE ifj2017_test_SRC
        LIST_DIRECTORIES false
        RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
        "test/*.cpp" "test/*.hpp" "test/*.h" "test/*.tpp"
)
add_executable(ifj2017_test ${ifj2017_SRC_no_main} ${ifj2017_test_SRC} ${VS_debug_visualizers})

IF (NOT MSVC)
    target_link_libraries(ifj2017_test m)
ENDIF ()
target_link_libraries(ifj2017_test gtest)
add_test(NAME test COMMAND ifj2017_test)

enable_testing()

# coverage
add_coverage(ifj2017_test)