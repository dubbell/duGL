add_library(shading STATIC shader.cpp uniform_buffer.cpp)

target_include_directories(shading PUBLIC ..)

target_link_libraries(shading PUBLIC project_dependencies)