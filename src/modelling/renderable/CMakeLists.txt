add_library(model STATIC mesh.cpp renderable.cpp model_builder.cpp)

target_include_directories(model PUBLIC ..)

target_link_libraries(model PUBLIC project_dependencies shading)