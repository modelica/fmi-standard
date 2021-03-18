# configuration to build the code examples

cmake_minimum_required (VERSION 3.12)

project (snippets)

add_executable(snippets
    ../../headers/fmi3Functions.h
    ../../headers/fmi3FunctionTypes.h
    ../../headers/fmi3PlatformTypes.h
    snippets.c
)

target_include_directories(snippets PRIVATE
    ../../headers
)
