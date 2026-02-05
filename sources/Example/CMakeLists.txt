#@+ ======================== User TARGET NAME configuration ============================
set(THIS Example)
#@- =========================== END OF USER CONFIGURATION ===============================

set(TARGET ${PROJECT_NAME}_${THIS})
file(GLOB sources_list "${CMAKE_CURRENT_LIST_DIR}/*.cpp")
add_executable(${TARGET})
target_sources(${TARGET} 
    PRIVATE
        ${sources_list}
)

#@+ ======================== User DEPENDENCIES configuration ============================
target_link_libraries(${TARGET}
    PRIVATE
        ${PROJECT_NAME}_Repo
        Variant_Visitor::Variant_Visitor
)
#@- =========================== END OF USER CONFIGURATION ===============================
target_compile_features(${TARGET} PUBLIC cxx_std_17)

PRINT_TARGET_PROPERTIES(${TARGET})

add_custom_command(TARGET ${TARGET} POST_BUILD 
                    COMMAND ${CMAKE_COMMAND} -E copy_directory
                        ${PROJECT_SOURCE_DIR}/config
                        $<TARGET_FILE_DIR:${TARGET}>/config 
                    COMMENT "Copying configuration files."
                        VERBATIM
)

IMPORT_TARGET_DLLS(${TARGET})
