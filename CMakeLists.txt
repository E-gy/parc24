cmake_minimum_required(VERSION 3.11)

Include(FetchContent)

project(parc24 VERSION 0.0.1)
set(CMAKE_C_STANDARD 99)

# hmmm
# include(CheckPIESupported)
# check_pie_supported()

# CALP import

file(GLOB_RECURSE src_calp "${PROJECT_SOURCE_DIR}/src/calp/main/*.h" "${PROJECT_SOURCE_DIR}/src/calp/main/*.c")
add_library(calp OBJECT ${src_calp})
include_directories(calp ${PROJECT_SOURCE_DIR}/src/calp/api)
include_directories(calp ${PROJECT_SOURCE_DIR}/src/calp/main)
set_property(TARGET calp PROPERTY POSITION_INDEPENDENT_CODE TRUE)

# Core

find_package(Threads)

file(GLOB_RECURSE src_main "${PROJECT_SOURCE_DIR}/src/main/*.h" "${PROJECT_SOURCE_DIR}/src/main/*.c")
add_library(parc24 SHARED ${src_main})
include_directories(parc24 ${PROJECT_SOURCE_DIR}/src/api)
include_directories(parc24 ${PROJECT_SOURCE_DIR}/src/main)
include_directories(parc24 ${PROJECT_SOURCE_DIR}/src/calp/api)
target_compile_options(parc24 PRIVATE
	$<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
	$<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wno-unused -Werror>
)
target_link_libraries(parc24 calp ${CMAKE_THREAD_LIBS_INIT})
target_compile_definitions(parc24 PRIVATE _GNU_SOURCE)

add_library(parc24cov STATIC ${src_main})
include_directories(parc24cov ${PROJECT_SOURCE_DIR}/src/api)
include_directories(parc24cov ${PROJECT_SOURCE_DIR}/src/main)
include_directories(parc24cov ${PROJECT_SOURCE_DIR}/src/calp/api)
target_compile_options(parc24cov PRIVATE
	$<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
	$<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wno-unused -Werror>
)
target_compile_options(parc24cov PUBLIC --coverage)
target_link_libraries(parc24cov calp ${CMAKE_THREAD_LIBS_INIT} gcov)
target_compile_definitions(parc24cov PRIVATE _GNU_SOURCE)

# Exe

file(GLOB_RECURSE src_exe "${PROJECT_SOURCE_DIR}/src/exe/*.h" "${PROJECT_SOURCE_DIR}/src/exe/*.c")

add_executable(42sh ${src_exe})
include_directories(42sh ${PROJECT_SOURCE_DIR}/src/api)
include_directories(42sh ${PROJECT_SOURCE_DIR}/src/main)
include_directories(42sh ${PROJECT_SOURCE_DIR}/src/exe)
target_compile_options(42sh PRIVATE
	$<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
	$<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wno-unused -Werror>
)
target_link_libraries(42sh parc24)
set_property(TARGET 42sh PROPERTY POSITION_INDEPENDENT_CODE TRUE)

add_executable(42shcov ${src_exe})
include_directories(42shcov ${PROJECT_SOURCE_DIR}/src/api)
include_directories(42shcov ${PROJECT_SOURCE_DIR}/src/main)
include_directories(42shcov ${PROJECT_SOURCE_DIR}/src/exe)
target_compile_options(42shcov PRIVATE
	$<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
	$<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wno-unused -Werror>
)
target_link_libraries(42shcov parc24cov)

# Tests

file(GLOB_RECURSE src_test "${PROJECT_SOURCE_DIR}/src/test/*.h" "${PROJECT_SOURCE_DIR}/src/test/*.c" "${PROJECT_SOURCE_DIR}/src/test/*.hpp" "${PROJECT_SOURCE_DIR}/src/test/*.cpp")
add_executable(tests ${src_test})
target_compile_options(tests PRIVATE
	$<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
	$<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wno-unused -Werror>
)

# Catch2 from Fetch works properly, so 
FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v2.13.1)
FetchContent_MakeAvailable(Catch2)
FetchContent_GetProperties(Catch2 SOURCE_DIR src_Catch2)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${src_Catch2}/contrib/")
target_link_libraries(tests Catch2::Catch2)

# no uniheaders ~~for now~~
# list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake/Modules")
# include_directories(tests ${PROJECT_SOURCE_DIR}/src/test)

include_directories(tests ${PROJECT_SOURCE_DIR}/src/api)
include_directories(tests ${PROJECT_SOURCE_DIR}/src/main)
include_directories(tests ${PROJECT_SOURCE_DIR}/src/test)
target_link_libraries(tests $<IF:$<CONFIG:DEBUG>,parc24cov,parc24>)

set(CTEST_OUTPUT_ON_FAILURE 1)
include(CTest)
include(Catch)
catch_discover_tests(tests)
