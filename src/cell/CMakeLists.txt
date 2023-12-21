# -----------------------------------------------------------------------------
MESSAGE(STATUS "[CELL] Library Directory: " ${CMAKE_CURRENT_SOURCE_DIR})
MESSAGE(STATUS "[CELL] Library Include Directory: " ${CMAKE_CURRENT_SOURCE_DIR}/../)
# -----------------------------------------------------------------------------

add_compile_definitions(DEBUG)
add_compile_definitions(LOGGER_DEBUG)

add_library(cell STATIC
        core/Assert.hpp
        core/Base.hpp
        core/Memory.hpp
        core/Charset.hpp
        core/String.cpp
        core/String.hpp
        core/Scanner.cpp
        core/Scanner.hpp
        core/StringSlice.cpp
        core/StringSlice.hpp
        core/WeakStringCache.cpp
        core/WeakStringCache.hpp

        log/Log.hpp

        http/Version.cpp
        http/Version.hpp
        http/Method.cpp
        http/Method.hpp
        http/Request.cpp
        http/Request.hpp
        http/Encoding.hpp
        http/Connection.hpp
        http/MimeType.cpp
        http/MimeType.hpp
        http/Uri.cpp
        http/Uri.hpp
        core/Router.cpp
        core/Router.hpp http/Encoding.cpp)


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