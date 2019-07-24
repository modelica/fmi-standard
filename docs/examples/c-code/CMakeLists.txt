cmake_minimum_required(VERSION 2.8)

message("CMAKE_C_FLAGS_DEBUG: ${CMAKE_C_FLAGS_DEBUG}")
message("CMAKE_C_FLAGS_RELEASE: ${CMAKE_C_FLAGS_RELEASE}")

SET(CMAKE_C_FLAGS_DEBUG          "${CMAKE_C_FLAGS_DEBUG} -std=c99")
SET(CMAKE_C_FLAGS_RELEASE        "${CMAKE_C_FLAGS_RELEASE} -std=c99")
SET(CMAKE_C_FLAGS_RELWITHDEBINFO "${CMAKE_C_FLAGS_RELWITHDEBINFO} -std=c99")
SET(CMAKE_C_FLAGS_MINSIZEREL     "${CMAKE_C_FLAGS_MINSIZEREL} -std=c99")

message("CMAKE_C_FLAGS_DEBUG: ${CMAKE_C_FLAGS_DEBUG}")
message("CMAKE_C_FLAGS_RELEASE: ${CMAKE_C_FLAGS_RELEASE}")

project (Examples)

add_executable (Jacobian
  ../../../headers/fmi3Functions.h
  ../../../headers/fmi3FunctionTypes.h
  ../../../headers/fmi3PlatformTypes.h
  jacobian.c
  ../../../headers/fmi3PlatformTypes.h
  VanDerPol/modelDescription.xml
)

target_include_directories(Jacobian PRIVATE
  ../../../headers
)
