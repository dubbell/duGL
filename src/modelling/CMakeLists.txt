add_library(modelling STATIC skybox.cpp)

add_subdirectory(renderable)
add_subdirectory(entity)

target_include_directories(modelling PUBLIC ..)

target_link_libraries(modelling PUBLIC project_dependencies shading model entity)