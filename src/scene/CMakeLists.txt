add_library(scene STATIC
    renderable.cpp 
    vertex_manager.cpp 
    entity.cpp 
    vertex_factory.cpp)

target_include_directories(scene PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(scene PRIVATE project_dependencies pipeline view)