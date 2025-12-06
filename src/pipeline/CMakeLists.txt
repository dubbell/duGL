add_library(pipeline STATIC shader.cpp stb_image.cpp)

target_include_directories(pipeline PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(pipeline PRIVATE project_dependencies)