add_library(geometry STATIC
    renderable.cpp 
    vertex_manager.cpp 
    entity.cpp
)

target_include_directories(geometry PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(geometry PRIVATE project_dependencies pipeline view)

add_library(geometry_factory STATIC
    factory/cube_builder.cpp
    factory/renderable_factory.cpp
)