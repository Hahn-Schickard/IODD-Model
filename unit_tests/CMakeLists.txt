#@+ =================== User TEST SUIT TARGET NAME configuration ========================
set(THIS Unit_Tests_Runner)
#@- =========================== END OF USER CONFIGURATION ===============================

#@+ ======================== User TEST_SUITES configuration =============================
file(GLOB TestSuite "${CMAKE_CURRENT_LIST_DIR}/TestSuite/*")

list(APPEND TEST_SUITES
    ${TestSuite}
)
#@- =========================== END OF USER CONFIGURATION ===============================
add_executable(${THIS})

target_sources(${THIS}
    PRIVATE
        "testRunner.cpp"
        ${TEST_SUITES}
)
#@+ ======================= User TEST_DECENCIES configuration ===========================
list(APPEND TEST_DECENCIES
)
#@- =========================== END OF USER CONFIGURATION ===============================
target_link_libraries(${THIS}
    PRIVATE
        GTest::gtest
        GTest::gmock
        ${TEST_DECENCIES}
)

add_test(
    NAME ${THIS}
    COMMAND ${THIS}
)

set_target_properties(${THIS}
    PROPERTIES
        CXX_STANDARD 17
)

PRINT_TARGET_PROPERTIES(${THIS})

IMPORT_TARGET_DLLS(${THIS})
