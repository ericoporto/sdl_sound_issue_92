cmake_minimum_required(VERSION 3.13..3.14)

set(CMAKE_USER_MAKE_RULES_OVERRIDE ${CMAKE_CURRENT_SOURCE_DIR}/CMake/c_flag_overrides.cmake)
set(CMAKE_USER_MAKE_RULES_OVERRIDE_CXX ${CMAKE_CURRENT_SOURCE_DIR}/CMake/cxx_flag_overrides.cmake)

project(SDL_SOUND_ISSUE_92
    VERSION 1.0.0
    LANGUAGES CXX C)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/CMake")

include(FetchContent)
set(FETCHCONTENT_UPDATES_DISCONNECTED on)

if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    # WIN32 is set by CMake for any Windows platform
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    set(LINUX TRUE)
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set(MACOS TRUE)
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Android")
    set(ANDROID TRUE)
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Emscripten")
    set(EMSCRIPTEN TRUE)
elseif (${CMAKE_SYSTEM_NAME} MATCHES "FreeBSD")
    set(FREEBSD TRUE)
else()
    message(FATAL_ERROR "Unsupported system: ${CMAKE_SYSTEM_NAME}")
endif ()

# Unfortunately there's an upstream bug in Android NDK with Clang
# https://github.com/android/ndk/issues/721
# https://reviews.llvm.org/D79919
# The issue has been fixed upstream but hasn't been rolled in Android Studio yet
# Once it gets fixed, we can remove the Android special case.
if(NOT ANDROID)
    include(CheckIPOSupported)
    check_ipo_supported(RESULT ipo_supported OUTPUT ipo_not_supported_reason)
    if(EMSCRIPTEN)
        set(ipo_supported FALSE)
    endif()

    if(ipo_supported)
        if(${CMAKE_BUILD_TYPE} MATCHES Release)
            set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE          TRUE)
            set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELWITHDEBINFO   TRUE)
        endif()
    else()
        message(STATUS "Interprocedural optimisation (IPO/LTO) not supported: <${ipo_not_supported_reason}>")
    endif()
else()
    # it is Android NDK, so we apply LTO on Release Builds
    if(${CMAKE_BUILD_TYPE} MATCHES Release)
        set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -flto=full")
    endif()
endif()

add_compile_definitions("_FILE_OFFSET_BITS=64")

add_compile_definitions("$<$<CONFIG:DEBUG>:_DEBUG>")
add_compile_definitions("$<$<CONFIG:RELEASE>:NDEBUG>")

if(MSVC)
    add_compile_options(/MP)    # Build with Multiple Processes
    add_compile_definitions(_CRT_SECURE_NO_DEPRECATE)
    add_compile_definitions(_CRT_NONSTDC_NO_DEPRECATE)
else()
    add_compile_options(
        -fsigned-char
        -fno-strict-aliasing
        -fwrapv

        -Wall
        -Wextra

        -Wendif-labels
        -Wfloat-equal
        -Wformat
        -Wformat-security
        -Winit-self
        -Winline
        -Wmissing-noreturn
        -Wpointer-arith
        -Wshadow
        -Wundef
        -Wwrite-strings
        -Wunused-result

        # probably need fixing but disable until we have time
        -Wno-unknown-pragmas
        -Wno-deprecated-declarations
        -Wno-unused-parameter
        -Wno-sign-compare
        -Wno-cast-align
        -Wno-cast-qual
        -Wno-missing-declarations
        -Wno-switch-enum
        # -Wlarger-than-4096
        -Wno-redundant-decls

        -Werror=write-strings
        #-Werror=implicit-function-declaration
        #-Werror=unused-result
    )

    add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-Wno-old-style-cast>)
    add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-Werror=delete-non-virtual-dtor>)

    add_compile_options($<$<COMPILE_LANGUAGE:C>:-Wbad-function-cast>)
    add_compile_options($<$<COMPILE_LANGUAGE:C>:-Wdeclaration-after-statement>)
    add_compile_options($<$<COMPILE_LANGUAGE:C>:-Wno-missing-prototypes>)
    add_compile_options($<$<COMPILE_LANGUAGE:C>:-Wold-style-definition>)
    add_compile_options($<$<COMPILE_LANGUAGE:C>:-Wstrict-prototypes>)

endif()

find_package(PkgConfig)

include(FetchSDL2)

include(FetchSDL_Sound)

    add_executable(sdl_sound_issue_92)