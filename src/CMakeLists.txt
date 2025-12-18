add_library(application STATIC app.cpp)

add_subdirectory(controllers)
add_subdirectory(pipeline)
add_subdirectory(view)
add_subdirectory(model)

target_include_directories(application PUBLIC ..)

target_link_libraries(application PUBLIC controllers pipeline view models)
