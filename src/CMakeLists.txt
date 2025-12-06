add_library(application STATIC app.cpp)

add_subdirectory(controllers)
add_subdirectory(pipeline)
add_subdirectory(utils)
add_subdirectory(view)

target_include_directories(application PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(application PUBLIC controllers pipeline view utils)

target_link_libraries(application PRIVATE project_dependencies)
