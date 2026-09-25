cmake_minimum_required(VERSION 3.15)

project(Logging VERSION 1.0)

option(COVERAGE "Enable coverage reporting" OFF)
option(THREAD_SANITIZER "Enable ThreadSanitizer" OFF)

if(COVERAGE AND THREAD_SANITIZER)
    message(FATAL_ERROR "COVERAGE and THREAD_SANITIZER cannot be enabled together.")
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/submodules/configuration/cmake")
include(ProjectOptions)
include(CoverageSupport)

option(COVERAGE "Enable coverage reporting" OFF)

enable_project_options()
enable_project_coverage()

if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 19.20)
        message(FATAL_ERROR "MSVC version must be at least 19.20!")
    endif()
elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    if (NOT EXISTS "/etc/nv_tegra_release")
        if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 10.2)
            message(FATAL_ERROR "GCC version must be at least 10.2!")
        endif()
    endif()
elseif (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 11.0)
        message(FATAL_ERROR "Clang version must be at least 11.0!")
    endif()
else()
    message(WARNING "You are using an unsupported compiler! MSVC, GCC and Clang are supported.")
endif()

if(WIN32)
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS TRUE)
    set(LIBRARY_KIND STATIC)
elseif(COVERAGE)
    set(LIBRARY_KIND STATIC)
else()
    set(LIBRARY_KIND SHARED)	
endif()

find_package(Threads REQUIRED)

include_directories(SYSTEM ${PROJECT_SOURCE_DIR}/include)

if(NOT TARGET logging)
    add_subdirectory(src)

    add_library(logging ${LIBRARY_KIND} $<TARGET_OBJECTS:LOGGING_SRC>)
    target_link_libraries(logging ${CMAKE_THREAD_LIBS_INIT})

    if(CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)
        if(NOT TARGET tests)
            enable_testing()
            add_subdirectory(test)
        endif()

        if(COVERAGE)
            if(APPLE AND CMAKE_CXX_COMPILER_ID MATCHES "AppleClang|Clang")
            setup_target_for_coverage_llvm(
                NAME coverage
                TARGET logging
                OBJECTS
                    test_logging
                DEPENDENCIES tests
                SOURCES
                    "${PROJECT_SOURCE_DIR}/include"
                    "${PROJECT_SOURCE_DIR}/src"
            )
        else()
                setup_target_for_coverage_gcc(
                    NAME coverage
                    DEPENDENCIES tests
                )
            endif()
        endif()
    endif()

endif()
