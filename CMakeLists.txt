cmake_minimum_required(VERSION 3.15)

project(ConcLog VERSION 1.0)

option(COVERAGE "Enable coverage reporting" OFF)
option(THREAD_SANITIZER "Enable ThreadSanitizer" OFF)

if(COVERAGE AND THREAD_SANITIZER)
    message(FATAL_ERROR "COVERAGE and THREAD_SANITIZER cannot be enabled together.")
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

if(COVERAGE)
    if(APPLE AND CMAKE_CXX_COMPILER_ID MATCHES "AppleClang|Clang")
        include(LLVMCodeCoverage)
        append_llvm_coverage_compiler_flags()
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
        include(GCCCodeCoverage)
        append_gcc_coverage_compiler_flags()
    else()
        message(FATAL_ERROR "Coverage is supported only with GCC or macOS AppleClang/Clang.")
    endif()
endif()

if(THREAD_SANITIZER)
    if(WIN32 OR NOT CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        message(FATAL_ERROR "ThreadSanitizer is supported only with GCC or Clang on Unix-like systems.")
    endif()
    add_compile_options(-fsanitize=thread -fno-omit-frame-pointer -g)
    add_link_options(-fsanitize=thread)
endif()

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

set(CMAKE_MACOSX_RPATH 1)
set(BUILD_SHARED_LIBS ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT WIN32)
    set(ANY_TARGET_WARN all extra pedantic sign-conversion cast-qual disabled-optimization
        init-self missing-include-dirs sign-promo switch-default undef redundant-decls
        strict-aliasing unused-parameter shadow error)
    foreach(WARN ${ANY_TARGET_WARN})
        add_compile_options(-W${WARN})
    endforeach()
else()
    add_compile_options(/WX)
endif()

find_package(Threads REQUIRED)

include_directories(SYSTEM ${PROJECT_SOURCE_DIR}/include)

if(NOT TARGET conclog)
    add_subdirectory(src)

    add_library(conclog ${LIBRARY_KIND} $<TARGET_OBJECTS:CONCLOG_SRC>)
    target_link_libraries(conclog ${CMAKE_THREAD_LIBS_INIT})

    if(CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)
        if(NOT TARGET tests)
            enable_testing()
            add_subdirectory(test)
        endif()

        if(COVERAGE)
            if(APPLE AND CMAKE_CXX_COMPILER_ID MATCHES "AppleClang|Clang")
            setup_target_for_coverage_llvm(
                NAME coverage
                TARGET conclog
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
