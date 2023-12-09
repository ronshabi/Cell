# -----------------------------------------------------------------------------
MESSAGE(STATUS "[CELL] Library Directory: " ${CMAKE_CURRENT_SOURCE_DIR})
MESSAGE(STATUS "[CELL] Library Include Directory: " ${CMAKE_CURRENT_SOURCE_DIR}/../)
# -----------------------------------------------------------------------------

add_compile_definitions(DEBUG)

add_library(cell STATIC
        Assert.hpp
        Base.hpp
        Memory.hpp
        Charset.hpp
        String.cpp
        String.hpp
        Scanner.cpp
        Scanner.hpp)


target_include_directories(cell PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../)


# CompileOptions:
# -----------------
#   o Sanitizer: define env-var CELL_COMPILE_OPTIONS_SANITIZER
#   o Fuzzer: define env-var CELL_COMPILE_OPTIONS_FUZZER
#

if (DEFINED ENV{CELL_COMPILE_OPTIONS_SANITIZER})
    set(ENV{ASAN_OPTIONS=check_initialization_order} 1)
    set(Cell_Sanitizer_Flags -fsanitize=signed-integer-overflow,null,alignment,undefined,address -fsanitize-trap=undefined -fno-omit-frame-pointer -fno-optimize-sibling-calls)
    message(STATUS "[CELL] [CompileOptions] Compiling with sanitizer (${Cell_Sanitizer_Flags})")
    target_compile_options(cell PUBLIC ${Cell_Sanitizer_Flags})
    target_link_options(cell PUBLIC ${Cell_Sanitizer_Flags})
endif ()