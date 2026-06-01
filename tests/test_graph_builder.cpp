#include <gtest/gtest.h>
#include <codeatlas/graph/graph_builder.hpp>
#include <codeatlas/layout/force_directed.hpp>

using namespace codeatlas;

TEST(GraphBuilderTest, BuildMindMapAndFileDependency) {
    RepositoryModel model;
    Entity f1; f1.id = "file:a.cpp"; f1.name = "a.cpp"; f1.kind = EntityKind::File;
    Entity f2; f2.id = "file:b.hpp"; f2.name = "b.hpp"; f2.kind = EntityKind::File;
    f1.relations.push_back({"includes", "file:b.hpp"});

    model.add_entity(f1);
    model.add_entity(f2);

    auto mindmap = GraphBuilder::build_graph(model, GraphType::MindMap);
    EXPECT_GE(mindmap.nodes().size(), 2);

    auto file_dep = GraphBuilder::build_graph(model, GraphType::FileDependency);
    EXPECT_GE(file_dep.nodes().size(), 2);
    EXPECT_EQ(file_dep.edges().size(), 1);

    ForceDirectedLayout::apply_layout(file_dep, 10, 500.0, 500.0);
    EXPECT_NE(file_dep.nodes()[0].x, 0.0);
}
