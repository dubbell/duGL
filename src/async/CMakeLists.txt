add_library(async STATIC dispatcher.cpp)
target_include_directories(async PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(async PUBLIC Jolt)