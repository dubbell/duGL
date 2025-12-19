add_library(utils STATIC stb_image.cpp)

target_include_directories(utils PUBLIC ..)

target_link_libraries(utils PUBLIC project_dependencies)