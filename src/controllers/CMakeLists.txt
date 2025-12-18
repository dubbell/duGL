add_library(controllers_interface INTERFACE)
target_include_directories(controllers_interface 
    INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/interfaces)

add_library(controllers STATIC keyboard.cpp mouse.cpp flight_controller.cpp)
target_include_directories(controllers PUBLIC ..)
target_link_libraries(controllers PUBLIC project_dependencies view)