add_library(physics STATIC common.cpp physics_entity.cpp)
target_include_directories(physics PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(physics PUBLIC Jolt modelling glm::glm)
