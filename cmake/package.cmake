cmake_minimum_required(VERSION 3.20 FATAL_ERROR)

if("${FILENAME}" STREQUAL "")
  set(FILENAME dibs-${CMAKE_HOST_SYSTEM_NAME}-x64)
endif()

execute_process(
  COMMAND ${CMAKE_COMMAND} -DBUILD_DIR=build/package -DINSTALL_DIR=install -DBUILD_CONFIG=Debug -P cmake/build.cmake
  COMMAND_ERROR_IS_FATAL ANY
)

execute_process(
  COMMAND ${CMAKE_COMMAND} -DBUILD_DIR=build/package -DINSTALL_DIR=install -DBUILD_CONFIG=Release -P cmake/build.cmake
  COMMAND_ERROR_IS_FATAL ANY
)

execute_process(
  COMMAND ${CMAKE_COMMAND} -E tar -c ../${FILENAME}.zip --format=zip .
  WORKING_DIRECTORY build/package/install
  COMMAND_ERROR_IS_FATAL ANY
)
