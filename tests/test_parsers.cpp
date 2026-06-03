#include <gtest/gtest.h>
#include <codeatlas/parser/cpp_parser.hpp>
#include <codeatlas/parser/python_parser.hpp>
#include <codeatlas/parser/cmake_parser.hpp>

using namespace codeatlas;

TEST(ParserTest, CppParserExtraction) {
    RepositoryModel model;
    std::string cpp_code = R"cpp(
#include <iostream>
#include "graph.hpp"

class GraphEngine {
public:
    void compute_layout() {}
};

void run_analysis() {
}
)cpp";

    CppParser::parse("src/engine.cpp", cpp_code, model);

    auto classes = model.get_entities_by_kind(EntityKind::Class);
    EXPECT_EQ(classes.size(), 1);
    EXPECT_EQ(classes[0].name, "GraphEngine");

    auto funcs = model.get_entities_by_kind(EntityKind::Function);
    EXPECT_EQ(funcs.size(), 2);
}

TEST(ParserTest, PythonParserExtraction) {
    RepositoryModel model;
    std::string py_code = R"py(
import sys
from math import sqrt

class ModelTrainer:
    def fit(self):
        pass

def evaluate_metrics():
    pass
)py";

    PythonParser::parse("scripts/train.py", py_code, model);

    auto classes = model.get_entities_by_kind(EntityKind::Class);
    EXPECT_EQ(classes.size(), 1);
    EXPECT_EQ(classes[0].name, "ModelTrainer");
}

TEST(ParserTest, CMakeParserExtraction) {
    RepositoryModel model;
    std::string cmake_code = R"cmake(
add_executable(codeatlas_app main.cpp)
target_link_libraries(codeatlas_app PRIVATE codeatlas_lib)
)cmake";

    CMakeParser::parse("CMakeLists.txt", cmake_code, model);

    auto targets = model.get_entities_by_kind(EntityKind::BuildTarget);
    EXPECT_EQ(targets.size(), 1);
    EXPECT_EQ(targets[0].name, "codeatlas_app");
}
