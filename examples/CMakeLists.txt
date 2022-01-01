if(NOT TARGET dibs)
  find_package(dibs REQUIRED)
endif()

add_executable(dibs-example)
target_link_libraries(dibs-example PRIVATE dibs::dibs dibs::options)
target_sources(dibs-example PRIVATE example.cpp)
