add_library(view STATIC camera.cpp)

target_include_directories(view INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(view PRIVATE project_dependencies controllers)