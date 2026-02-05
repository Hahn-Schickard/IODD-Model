#@+ ======================== User TARGET NAME configuration ============================
set(THIS Repo)
#@- =========================== END OF USER CONFIGURATION ===============================

set(TARGET ${PROJECT_NAME}_${THIS})

#@+ =========================== User TARGET configuration ===============================
file(GLOB THIS_INCLUDES ${INCLUDES_DIR}/*.hpp)
file(GLOB SOURCEFILES "${CMAKE_CURRENT_LIST_DIR}/*.?pp"
                       "${CMAKE_CURRENT_LIST_DIR}/Decoders/*.?pp"
)

add_library(${TARGET})
target_sources(${TARGET}
    PUBLIC          
        ${THIS_INCLUDES}
    PRIVATE
        ${SOURCEFILES}
)

target_link_libraries(${TARGET}
    PUBLIC
        ${PROJECT_NAME}_Model
    PRIVATE
        pugixml::pugixml
        Variant_Visitor::Variant_Visitor
)

target_include_directories(${TARGET}
    PUBLIC
        $<BUILD_INTERFACE:${INCLUDES_DIR}>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}>
)
#@- =========================== END OF USER CONFIGURATION ===============================

target_compile_features(${TARGET} PUBLIC cxx_std_17)

PRINT_TARGET_PROPERTIES(${TARGET})

INSTALL_TARGET(${TARGET})
INSTALL_HEADERS(HEADERS "${THIS_INCLUDES}")
