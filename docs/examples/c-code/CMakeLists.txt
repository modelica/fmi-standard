cmake_minimum_required(VERSION 3.2)

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
