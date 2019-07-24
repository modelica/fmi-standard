cmake_minimum_required(VERSION 2.8)

project (Examples)

add_executable (Jacobian
  ../../../headers/fmi3Functions.h
  ../../../headers/fmi3FunctionTypes.h
  ../../../headers/fmi3PlatformTypes.h
  jacobian.c
  ../../../headers/fmi3PlatformTypes.h
  VanDerPol/modelDescription.xml
)

set_property(TARGET Jacobian PROPERTY C_STANDARD 99)

target_include_directories(Jacobian PRIVATE
  ../../../headers
)
