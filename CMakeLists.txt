cmake_minimum_required(VERSION 3.15)

project(littlefs-utils C CXX)
include(cmake/StandardProjectSettings.cmake)

option(STATIC_CRT "Link against the static CRT when building with MSVC" OFF)
if (MSVC)
    if(CMAKE_BUILD_TYPE)
        if (STATIC_CRT)
            if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
                set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreadedDebug")
                set(CONAN_EXTRA_SETTINGS "compiler.runtime=MTd")
            else ()
                set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded")
                set(CONAN_EXTRA_SETTINGS "compiler.runtime=MT")
            endif ()
        else ()
            if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
                set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreadedDebugDLL")
                set(CONAN_EXTRA_SETTINGS "compiler.runtime=MDd")
            else ()
                set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")
                set(CONAN_EXTRA_SETTINGS "compiler.runtime=MD")
            endif ()
        endif ()
    else()
        if(STATIC_CRT)
            message(FATAL_ERROR "Static CRT not supported with multi-configuration builds")
        endif()
    endif()
endif ()

# Link this 'library' to set the c++ standard / compile-time options requested
add_library(project_options INTERFACE)
target_compile_features(project_options INTERFACE cxx_std_17 c_std_99)
if(MSVC)
    target_compile_definitions(project_options INTERFACE -DNOMINMAX)
    target_compile_definitions(project_options INTERFACE -DUNICODE -D_UNICODE)
endif()

set(LITTLEFS_UTILS_VERSION $ENV{LITTLEFS_UTILS_VERSION})
configure_file(littlefs_utils_config.h.in littlefs_utils_config.h @ONLY)
target_include_directories(project_options INTERFACE ${CMAKE_CURRENT_BINARY_DIR})

# Link this 'library' to use the warnings specified in CompilerWarnings.cmake
add_library(project_warnings INTERFACE)

# standard compiler warnings
include(cmake/CompilerWarnings.cmake)
set_project_warnings(project_warnings)

# sanitizer options if supported by compiler
include(cmake/Sanitizers.cmake)
enable_sanitizers(project_options)

# enable doxygen
include(cmake/Doxygen.cmake)
enable_doxygen()

# allow for static analysis options
include(cmake/StaticAnalyzers.cmake)

option(BUILD_SHARED_LIBS "Enable compilation of shared libraries" OFF)
option(ENABLE_TESTING "Enable Test Builds" ON)

include(cmake/Conan.cmake)
run_conan()

#if(ENABLE_TESTING)
#  enable_testing()
#  message(
#    "Building Tests. Be sure to check out test/constexpr_tests for constexpr testing"
#  )
#  add_subdirectory(test)
#endif()

add_subdirectory(littlefs1)
add_subdirectory(littlefs2)
add_subdirectory(littlefs)
add_subdirectory(littlefs-extract)
