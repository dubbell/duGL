add_library(pipeline STATIC shader.cpp stb_image.cpp)

target_include_directories(pipeline PUBLIC ..)

target_link_libraries(pipeline PUBLIC project_dependencies)