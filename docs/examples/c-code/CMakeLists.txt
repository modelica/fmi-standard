cmake_minimum_required(VERSION 2.8)

SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99")

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
