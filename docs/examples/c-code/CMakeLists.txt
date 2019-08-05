cmake_minimum_required(VERSION 2.8)

set(CMAKE_VERBOSE_MAKEFILE ON)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  SET(CMAKE_C_FLAGS_DEBUG          "${CMAKE_C_FLAGS_DEBUG} -std=c99")
  SET(CMAKE_C_FLAGS_RELEASE        "${CMAKE_C_FLAGS_RELEASE} -std=c99")
  SET(CMAKE_C_FLAGS_RELWITHDEBINFO "${CMAKE_C_FLAGS_RELWITHDEBINFO} -std=c99")
  SET(CMAKE_C_FLAGS_MINSIZEREL     "${CMAKE_C_FLAGS_MINSIZEREL} -std=c99")
endif()

set(EXAMPLE_SOURCES
  ../../../headers/fmi3Functions.h
  ../../../headers/fmi3FunctionTypes.h
  ../../../headers/fmi3PlatformTypes.h
  ../../../headers/fmi3PlatformTypes.h
  VanDerPol/modelDescription.xml
)

set(MODEL_SOURCES
  VanDerPol/sources/fmi3Functions.c
  VanDerPol/sources/model.c
  VanDerPol/sources/slave.c
)

project (Examples)

add_executable (jacobian
  ${EXAMPLE_SOURCES}
  ${MODEL_SOURCES}
  jacobian.c
)

target_include_directories(jacobian PRIVATE
  ../../../headers
)

target_compile_definitions(jacobian PRIVATE
  FMI_VERSION=3
  DISABLE_PREFIX
)

add_library(slave1 STATIC
  ${MODEL_SOURCES}
)

target_compile_definitions(slave1 PRIVATE
  FMI_VERSION=3
  FMI3_FUNCTION_PREFIX=s1_
)

target_include_directories(slave1 PRIVATE
  ../../../headers
)

add_library(slave2 STATIC
  ${MODEL_SOURCES}
)

target_compile_definitions(slave2 PRIVATE
  FMI_VERSION=3
  FMI3_FUNCTION_PREFIX=s2_
)

target_include_directories(slave2 PRIVATE
  ../../../headers
)

add_executable (co_simulation
  ${EXAMPLE_SOURCES}
  co_simulation.c
)

target_include_directories(co_simulation PRIVATE
  ../../../headers
)

target_link_libraries(co_simulation
  slave1
  slave2
)

add_library(model STATIC
  ${MODEL_SOURCES}
)

target_compile_definitions(model PRIVATE
  FMI_VERSION=3
  FMI3_FUNCTION_PREFIX=M_
)

target_include_directories(model PRIVATE
  ../../../headers
)

add_executable (model_exchange
  ${EXAMPLE_SOURCES}
  model_exchange.c
)

target_include_directories(model_exchange PRIVATE
  ../../../headers
)

target_link_libraries(model_exchange
  model
)
