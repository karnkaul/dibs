cmake_minimum_required(VERSION 3.20 FATAL_ERROR)

include(cmake/build_version.cmake)

if("${FILENAME}" STREQUAL "")
  set(FILENAME dibs-${build_version}-${CMAKE_HOST_SYSTEM_NAME}-x64)
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

if(WIN32)
  set(FILENAME ${FILENAME}.zip)
  set(tar_cmd ${CMAKE_COMMAND} -E tar -c ../${FILENAME} --format=zip .)
else()
  set(FILENAME ${FILENAME}.tar.gz)
  set(tar_cmd ${CMAKE_COMMAND} -E tar -cvfz ../${FILENAME} .)
endif()
message(STATUS "Building archive ${BUILD_DIR}/${FILENAME}...")
execute_process(
  COMMAND ${tar_cmd}
  WORKING_DIRECTORY ${BUILD_DIR}/install
  COMMAND_ERROR_IS_FATAL ANY
)

message(STATUS "${BUILD_DIR}/${FILENAME} packaged successfully")
