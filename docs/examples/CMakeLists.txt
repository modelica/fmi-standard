# configuration to build the code examples

cmake_minimum_required (VERSION 3.12)

project (examples)

add_executable(dummy
    ../../headers/fmi3Functions.h
    ../../headers/fmi3FunctionTypes.h
    ../../headers/fmi3PlatformTypes.h
    dummy.c
)

target_include_directories(dummy PRIVATE
    ../../headers
)
