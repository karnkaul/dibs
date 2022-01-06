cmake_minimum_required(VERSION 3.20 FATAL_ERROR)

if("${FILENAME}" STREQUAL "")
  set(FILENAME dibs-${CMAKE_HOST_SYSTEM_NAME}-x64)
endif()
if("${BUILD_DIR}" STREQUAL "")
  set(BUILD_DIR build/package)
endif()

message(STATUS "Building Debug to ${BUILD_DIR}/Debug...")
execute_process(
  COMMAND ${CMAKE_COMMAND} -DBUILD_DIR=${BUILD_DIR} -DINSTALL_DIR=install -DBUILD_CONFIG=Debug -P cmake/build.cmake
  COMMAND_ERROR_IS_FATAL ANY
)

message(STATUS "Building Release to ${BUILD_DIR}/Release...")
execute_process(
  COMMAND ${CMAKE_COMMAND} -DBUILD_DIR=${BUILD_DIR} -DINSTALL_DIR=install -DBUILD_CONFIG=Release -P cmake/build.cmake
  COMMAND_ERROR_IS_FATAL ANY
)

message(STATUS "Building archive ${BUILD_DIR}/${FILENAME}.zip...")
execute_process(
  COMMAND ${CMAKE_COMMAND} -E tar -c ../${FILENAME}.zip --format=zip .
  WORKING_DIRECTORY ${BUILD_DIR}/install
  COMMAND_ERROR_IS_FATAL ANY
)

message(STATUS "${BUILD_DIR}/${FILENAME}.zip packaged successfully")
