add_library(geometry STATIC
renderable.cpp 
vertex_manager.cpp 
entity.cpp
)

add_subdirectory(factory)

target_include_directories(geometry PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(geometry PUBLIC project_dependencies pipeline view)