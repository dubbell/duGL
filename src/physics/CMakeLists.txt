add_library(physics STATIC common.cpp)
target_include_directories(physics PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(physics PUBLIC Jolt)