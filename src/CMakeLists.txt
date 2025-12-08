add_library(application STATIC app.cpp)

add_subdirectory(controllers)
add_subdirectory(pipeline)
add_subdirectory(view)
add_subdirectory(scene)

target_include_directories(application PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(application 
    PUBLIC controllers pipeline view scene
    PRIVATE project_dependencies)