add_subdirectory(detail)

target_sources(${PROJECT_NAME} PRIVATE
  bridge.cpp
  dibs.cpp
  instance_impl.hpp
)
