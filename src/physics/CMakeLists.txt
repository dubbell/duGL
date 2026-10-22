add_library(physics STATIC common.cpp anchored_constraints.cpp physics_entity.cpp rigid_entity.cpp body_settings.cpp ragdoll_builder.cpp ragdoll_entity.cpp)
target_include_directories(physics PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(physics PUBLIC Jolt modelling glm::glm)
