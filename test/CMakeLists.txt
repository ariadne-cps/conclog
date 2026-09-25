include(CTest)

set(UNIT_TESTS
    test_logging
    test_concurrency
)

foreach(TEST ${UNIT_TESTS})
    add_executable(${TEST} ${TEST}.cpp)
    target_link_libraries(${TEST} logging)
    add_test(${TEST} ${TEST})
endforeach()

add_custom_target(tests)
add_dependencies(tests ${UNIT_TESTS})
