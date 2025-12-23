add_library(entity STATIC entity.cpp outlined_entity.cpp)

target_include_directories(entity PUBLIC ..)

target_link_libraries(entity PUBLIC project_dependencies shading)