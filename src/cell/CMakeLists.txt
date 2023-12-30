# -----------------------------------------------------------------------------
MESSAGE(STATUS "[CELL] Library Directory: " ${CMAKE_CURRENT_SOURCE_DIR})
MESSAGE(STATUS "[CELL] Library Include Directory: " ${CMAKE_CURRENT_SOURCE_DIR}/../)
# -----------------------------------------------------------------------------

add_compile_definitions(DEBUG)
add_compile_definitions(LOGGER_DEBUG)

add_library(cell STATIC
        core/assert.hpp
        core/base.hpp
        core/memory.hpp
        core/charset.hpp
        core/string.cpp
        core/string.hpp
        core/scanner.cpp
        core/scanner.hpp
        core/string_slice.cpp
        core/string_slice.hpp
        core/weak_string_cache.cpp
        core/weak_string_cache.hpp

        log/Log.hpp

        http/version.cpp
        http/version.hpp
        http/method.cpp
        http/method.hpp
        http/request.cpp
        http/request.hpp
        http/encoding.hpp
        http/connection.hpp
        http/mime_type.cpp
        http/mime_type.hpp
        http/uri.cpp
        http/uri.hpp
        core/router.cpp
        core/router.hpp http/encoding.cpp core/types.hpp encoding/Gzip.cpp encoding/Gzip.hpp)


target_include_directories(cell PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../)


# CompileOptions:
# -----------------
#   o Sanitizer: define env-var CELL_COMPILE_OPTIONS_SANITIZER
#

if (DEFINED ENV{CELL_COMPILE_OPTIONS_SANITIZER})
    set(ENV{ASAN_OPTIONS=check_initialization_order} 1)
    set(Cell_Sanitizer_Flags -fsanitize=signed-integer-overflow,null,alignment,undefined,address -fsanitize-trap=undefined -fno-omit-frame-pointer -fno-optimize-sibling-calls)
    message(STATUS "[CELL] [CompileOptions] Compiling with sanitizer (${Cell_Sanitizer_Flags})")
    target_compile_options(cell PUBLIC ${Cell_Sanitizer_Flags})
    target_link_options(cell PUBLIC ${Cell_Sanitizer_Flags})
endif ()