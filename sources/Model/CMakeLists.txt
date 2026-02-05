set(TARGET ${PROJECT_NAME})

#@+ =========================== User TARGET configuration ===============================
list(APPEND THIS_INCLUDES_DIRS
            ${INCLUDES_DIR}/Model
            ${INCLUDES_DIR}/Model/Datatypes
            ${INCLUDES_DIR}/Model/Datatypes/Primitives
)

file(GLOB SOURCEFILES "${CMAKE_CURRENT_LIST_DIR}/*.cpp"
                       "${CMAKE_CURRENT_LIST_DIR}/Datatypes/*.cpp"
                       "${CMAKE_CURRENT_LIST_DIR}/Datatypes/Primitives/*.cpp"
)

INCLUDE_DIRS_AND_FILES("${THIS_INCLUDES_DIRS}" THIS_INCLUDES)

add_library(${TARGET} OBJECT)
target_sources(${TARGET}
    PUBLIC          
        ${THIS_INCLUDES}
    PRIVATE
        ${SOURCEFILES}
)

target_link_libraries(${TARGET}
    PRIVATE
        date::date
        Variant_Visitor::Variant_Visitor
)

target_include_directories(${TARGET} 
    INTERFACE
        $<BUILD_INTERFACE:${INCLUDES_DIR}/Model>
        $<BUILD_INTERFACE:${INCLUDES_DIR}/Model/Datatypes>
        $<BUILD_INTERFACE:${INCLUDES_DIR}/Model/Datatypes/Primitives>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}>
)
#@- =========================== END OF USER CONFIGURATION ===============================

target_compile_features(${TARGET} PUBLIC cxx_std_17)

PRINT_TARGET_PROPERTIES(${TARGET})

INSTALL_TARGET(${TARGET})

install(
    DIRECTORY ${INCLUDES_DIR}/Model
              ${INCLUDES_DIR}/Model/Datatypes
              ${INCLUDES_DIR}/Model/Datatypes/Primitives
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}
)
