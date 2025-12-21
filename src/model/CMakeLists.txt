add_library(models STATIC mesh.cpp renderable.cpp renderable_builder.cpp entity.cpp skybox.cpp)

target_include_directories(models PUBLIC ..)

target_link_libraries(models PUBLIC project_dependencies shading)