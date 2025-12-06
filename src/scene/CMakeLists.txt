add_library(entity STATIC entity.cpp)

target_include_directories(entity PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(entity PRIVATE project_dependencies)