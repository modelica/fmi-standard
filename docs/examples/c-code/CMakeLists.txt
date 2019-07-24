cmake_minimum_required(VERSION 2.8)

set(CMAKE_VERBOSE_MAKEFILE ON)

message("CMAKE_C_FLAGS_DEBUG: ${CMAKE_C_FLAGS_DEBUG}")
message("CMAKE_C_FLAGS_RELEASE: ${CMAKE_C_FLAGS_RELEASE}")

SET(CMAKE_C_FLAGS_DEBUG          "${CMAKE_C_FLAGS_DEBUG} -std=c99")
SET(CMAKE_C_FLAGS_RELEASE        "${CMAKE_C_FLAGS_RELEASE} -std=c99")
SET(CMAKE_C_FLAGS_RELWITHDEBINFO "${CMAKE_C_FLAGS_RELWITHDEBINFO} -std=c99")
SET(CMAKE_C_FLAGS_MINSIZEREL     "${CMAKE_C_FLAGS_MINSIZEREL} -std=c99")

message("CMAKE_C_FLAGS_DEBUG: ${CMAKE_C_FLAGS_DEBUG}")
message("CMAKE_C_FLAGS_RELEASE: ${CMAKE_C_FLAGS_RELEASE}")

project (Examples)

add_executable (jacobian
  ../../../headers/fmi3Functions.h
  ../../../headers/fmi3FunctionTypes.h
  ../../../headers/fmi3PlatformTypes.h
  jacobian.c
  ../../../headers/fmi3PlatformTypes.h
  VanDerPol/modelDescription.xml
)

target_include_directories(jacobian PRIVATE
  ../../../headers
)

add_library(slave1 STATIC
  VanDerPol/sources/fmi3Functions.c
  VanDerPol/sources/model.c
  VanDerPol/sources/slave.c
)

target_compile_definitions(slave1 PRIVATE
  FMI_VERSION=3
  FMI3_FUNCTION_PREFIX=s1_
)

target_include_directories(slave1 PRIVATE
  ../../../headers
)

add_library(slave2 STATIC
  VanDerPol/sources/fmi3Functions.c
  VanDerPol/sources/model.c
  VanDerPol/sources/slave.c
)

target_compile_definitions(slave2 PRIVATE
  FMI_VERSION=3
  FMI3_FUNCTION_PREFIX=s2_
)

target_include_directories(slave2 PRIVATE
  ../../../headers
)

add_executable (co_simulation
  ../../../headers/fmi3Functions.h
  ../../../headers/fmi3FunctionTypes.h
  ../../../headers/fmi3PlatformTypes.h
  co_simulation.c
  ../../../headers/fmi3PlatformTypes.h
  VanDerPol/modelDescription.xml
)

target_include_directories(co_simulation PRIVATE
  ../../../headers
)

target_link_libraries(co_simulation
  slave1
  slave2
)
