add_library(controllers_interface INTERFACE)
target_include_directories(controllers_interface INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/interfaces)

add_library(controllers STATIC keyboard.cpp mouse.cpp)
target_include_directories(controllers PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(controllers PRIVATE 
    project_dependencies 
    view 
    controllers_interface
)