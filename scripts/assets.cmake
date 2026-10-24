set(SHADER_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/assets")
set(SHADER_DEST_DIR "$<TARGET_FILE_DIR:duGL>/assets")

add_custom_target(copy_assets
    COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different ${SHADER_SOURCE_DIR} ${SHADER_DEST_DIR}
    COMMENT "Copying assets directory to output folder.")

add_dependencies(duGL copy_assets)
