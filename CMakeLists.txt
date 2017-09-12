cmake_minimum_required(VERSION 3.0)

project(ifj2017)

#init
include(test/CMakeLists.txt)
enable_testing()
find_package(codecov)

# ifj2017_test
file(	GLOB_RECURSE ifj2017_test_SRC 
	LIST_DIRECTORIES false
	RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
	"test/*.cpp" "test/*.hpp"	)
add_executable(ifj2017_test ${ifj2017_test_SRC})
target_link_libraries(ifj2017_test gtest)
add_test(NAME test COMMAND ifj2017_test)

# ifj2017
file(	GLOB_RECURSE ifj2017_SRC
	LIST_DIRECTORIES false
	RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
	"src/*.c" "src/*.h"	)
add_executable(ifj2017 ${ifj2017_SRC})

# coverage
add_coverage(ifj2017_test)
coverage_evaluate()

# automatic source groups by folder structure
set(_all_sources ${ifj2017_test_SRC} ${ifj2017_SRC})
foreach(_source IN ITEMS ${_all_sources})
    get_filename_component(_source_path "${_source}" PATH)
    IF(MSVC)
        string(REPLACE "/" "\\" _source_path_msvc "${_source_path}")
        set(_source_path ${_source_path_msvc})
    ENDIF()
    source_group("${_source_path}" FILES "${_source}")
endforeach()