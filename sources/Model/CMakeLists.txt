cmake_minimum_required(VERSION 3.6)

#@+ ======================== User MODULE NAME configuration ============================
set(THIS Model)
#@- =========================== END OF USER CONFIGURATION ===============================

set(MODULE ${PROJECT_NAME}_${THIS})

list(APPEND THIS_INCLUDES_DIRS
            ${INCLUDES_DIR}/Standard
)

file(GLOB sources_list "${CMAKE_CURRENT_LIST_DIR}/*.cpp")

INCLUDE_DIRS_AND_FILES("${THIS_INCLUDES_DIRS}" THIS_INCLUDES)

add_library(${MODULE})

target_sources(${MODULE}
               PRIVATE
               ${THIS_INCLUDES}
               ${sources_list}
)

#@+ ======================== User DEPENDENCIES configuration ============================
target_link_libraries(${MODULE}
                      PRIVATE #Private dependencies
                      Variant_Visitor::Variant_Visitor
                      PUBLIC  #Public dependencies
)
#@- =========================== END OF USER CONFIGURATION ===============================

target_include_directories(${MODULE} INTERFACE
                           $<BUILD_INTERFACE:${INCLUDES_DIR}>
                           $<BUILD_INTERFACE:${INCLUDES_DIR}/Standard>
                           $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
                           $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}>
)

target_compile_features(${MODULE} PUBLIC cxx_std_17)

PRINT_TARGET_PROPERTIES(${MODULE})

install(
    TARGETS ${MODULE}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(
    FILES ${THIS_INCLUDES}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}
)
