cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

enable_language(CXX)

set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)					# don't let OPTION erase non-cache variables
set(CMAKE_CXX_EXTENSIONS OFF CACHE INTERNAL "" FORCE)	# disable compiler extensions
set(CMAKE_POSITION_INDEPENDENT_CODE ON)					# enable PIC
set(CMAKE_DEBUG_POSTFIX "-d")
if("${CMAKE_BUILD_TYPE}" STREQUAL "")
  set(CMAKE_BUILD_TYPE Debug)			# ensure CMAKE_BUILD_TYPE is not empty
endif()

# ktl
include(FetchContent)
set(FETCHCONTENT_QUIET OFF)
FetchContent_Declare(
  ktl
  GIT_REPOSITORY https://github.com/karnkaul/ktl
  GIT_TAG 303f844
)
FetchContent_MakeAvailable(ktl)

# project parameters
set(project_name dibs)
set(${project_name}_version_file dibs_version.hpp)
set(${project_name}_version "1.0.0")
set(${project_name}_soversion 1)
set(cxx_standard cxx_std_20)

project(${project_name} VERSION "${${project_name}_version}")
set(is_root_project OFF)	# indicate if this is the top-level project
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
  set(is_root_project ON)
  message(STATUS "${PROJECT_NAME} is top level")
endif()

# options
option(DIBS_BUILD_EXAMPLES "Build dibs examples" ${is_root_project})
option(DIBS_UBSAN "Enable UBSan" OFF)

# includes
include(CheckPIESupported)
check_pie_supported()
include(cmake/platform.cmake)	# set up helper cache variables

# git operations
find_package(Git QUIET)
if(GIT_FOUND AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/.git")
  git_get_commit_hash(${PROJECT_NAME}_GIT_COMMIT_HASH)	# store commit hash in ${PROJECT_NAME}_GIT_COMIT_HASH
  if(is_root_project)
    option(GIT_UPDATE_SUBMODULES "Init/Update submodules during generation" ${is_root_project})
    if(GIT_UPDATE_SUBMODULES)
      git_update_submodules(WARNING)	# update git submodules and warn on failure
    endif()
  endif()
endif()

# external
add_subdirectory(ext)
add_subdirectory(cmake/interface)	# import interface library

# header text
if(is_root_project)
  set(build_config ${CMAKE_BUILD_TYPE})
  if(CMAKE_CONFIGURATION_TYPES)
    set(build_config "Multi-config")
  endif()
  message(STATUS "[${CMAKE_SYSTEM_PROCESSOR}] [${PLATFORM}] [${CMAKE_GENERATOR}] [${CMAKE_CXX_COMPILER_ID}] [${build_config}]")
endif()

# compile commands
if(is_root_project AND "${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
  set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
  message(STATUS "Exporting compile commands")
endif()

# appveyor
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/.appveyor.yml.in")
  message(STATUS "Configuring ${CMAKE_CURRENT_SOURCE_DIR}/.appveyor.yml.in")
  configure_file("${CMAKE_CURRENT_SOURCE_DIR}/.appveyor.yml.in" "${CMAKE_CURRENT_SOURCE_DIR}/.appveyor.yml")
endif()

# library
add_library(${PROJECT_NAME})
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})
set_property(TARGET ${PROJECT_NAME} PROPERTY VERSION ${${PROJECT_NAME}_version})
set_property(TARGET ${PROJECT_NAME} PROPERTY SOVERSION ${${PROJECT_NAME}_soversion})
set_property(TARGET ${PROJECT_NAME} PROPERTY INTERFACE_${PROJECT_NAME}_MAJOR_VERSION ${${PROJECT_NAME}_soversion})
set_property(TARGET ${PROJECT_NAME} APPEND PROPERTY COMPATIBLE_INTERFACE_STRING ${PROJECT_NAME}_MAJOR_VERSION)

target_include_directories(${PROJECT_NAME} SYSTEM INTERFACE
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>"
  "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)
target_include_directories(${PROJECT_NAME} PRIVATE 
  include
  src
  "${CMAKE_CURRENT_BINARY_DIR}/include"
)
target_link_libraries(${PROJECT_NAME} 
  PUBLIC
    glfw
    ktl::ktl
    dyvk::dyvk
    dear-imgui::dear-imgui
    ${PROJECT_NAME}::interface
  PRIVATE
    vk-bootstrap::vk-bootstrap
    ${PROJECT_NAME}::options
)

include(dibs_headers.cmake)
add_subdirectory(src)
target_source_group(TARGET ${PROJECT_NAME})

# version
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/src/${${project_name}_version_file}.in")
  configure_src_bin(
    IN src/${${project_name}_version_file}.in 
    OUT ${project_name}/${${project_name}_version_file}
    TARGET ${project_name}
  )
endif()

install(TARGETS ${PROJECT_NAME} interface options EXPORT ${PROJECT_NAME}-targets)
install(EXPORT ${PROJECT_NAME}-targets
  FILE ${PROJECT_NAME}-targets.cmake
  NAMESPACE ${PROJECT_NAME}::
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)
install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/include/" "${CMAKE_CURRENT_BINARY_DIR}/include/"
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
  FILES_MATCHING PATTERN "*.hpp"
)
install_package_config(PACKAGE ${PROJECT_NAME})
install_package_version(PACKAGE ${PROJECT_NAME} VERSION ${${PROJECT_NAME}_version})
export_package_to_build_tree(PACKAGE ${PROJECT_NAME})

# examples
if(DIBS_BUILD_EXAMPLES)
  message(STATUS "Adding dibs examples to build tree")
  add_subdirectory(examples)
endif()
