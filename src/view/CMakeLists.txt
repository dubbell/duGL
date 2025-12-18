add_library(view STATIC camera.cpp)

target_include_directories(view PUBLIC ..)

target_link_libraries(view PUBLIC project_dependencies controllers_interface)