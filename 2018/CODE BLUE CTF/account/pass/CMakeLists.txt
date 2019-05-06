cmake_minimum_required(VERSION 3.10)

add_library(SSPPass MODULE
    # List your source files here.
    p-ssp.cpp
)

# Use C++11 to compile our pass (i.e., supply -std=c++11).
target_compile_features(SSPPass PRIVATE cxx_range_for cxx_auto_type)

# LLVM is (typically) built with no C++ RTTI. We need to match that;
# otherwise, we'll get linker errors about missing RTTI data.
set_target_properties(SSPPass PROPERTIES
    COMPILE_FLAGS "-fno-rtti"
)

include_directories(/usr/include/llvm-6.0)
include_directories(/usr/include/llvm-c-6.0)
