add_library(geometry_factory STATIC
    cube_builder.cpp
    renderable_factory.cpp
)

target_include_directories(geometry_factory PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(geometry_factory PRIVATE project_dependencies geometry)