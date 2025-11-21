#include <benchmark/benchmark.h>
#include <codeatlas/codeatlas.hpp>

using namespace codeatlas;

static void BM_CppParserExtraction(benchmark::State& state) {
    std::string cpp_code = R"cpp(
#include <iostream>
class Engine { void update(); };
void run() {}
)cpp";

    for (auto _ : state) {
        RepositoryModel model;
        CppParser::parse("engine.cpp", cpp_code, model);
        benchmark::DoNotOptimize(model);
    }
}
BENCHMARK(BM_CppParserExtraction);

static void BM_ForceDirectedLayout(benchmark::State& state) {
    RepositoryModel model;
    for (int i = 0; i < 100; ++i) {
        Entity e;
        e.id = "node_" + std::to_string(i);
        e.name = "Node" + std::to_string(i);
        e.kind = EntityKind::File;
        model.add_entity(e);
    }
    auto graph = GraphBuilder::build_graph(model, GraphType::MindMap);

    for (auto _ : state) {
        auto g_copy = graph;
        ForceDirectedLayout::apply_layout(g_copy, 20, 800.0, 600.0);
        benchmark::DoNotOptimize(g_copy);
    }
}
BENCHMARK(BM_ForceDirectedLayout);

BENCHMARK_MAIN();
