add_executable(codeatlas_tests
    test_repository_model.cpp
    test_parsers.cpp
    test_graph_builder.cpp
    test_ai_assistant.cpp
)

target_link_libraries(codeatlas_tests PRIVATE
    codeatlas
    GTest::gtest_main
)

add_test(NAME codeatlas_tests COMMAND codeatlas_tests)
