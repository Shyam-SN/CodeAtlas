#pragma once

#include <codeatlas/graph/knowledge_graph.hpp>

namespace codeatlas {

class HierarchicalLayout {
public:
    static void apply_layout(KnowledgeGraph& graph, double width = 1000.0, double height = 800.0);
    static void apply_radial_mindmap(KnowledgeGraph& graph, double width = 1200.0, double height = 900.0);
};

} // namespace codeatlas
