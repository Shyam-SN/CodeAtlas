#pragma once

#include <codeatlas/graph/knowledge_graph.hpp>

namespace codeatlas {

class ForceDirectedLayout {
public:
    static void apply_layout(KnowledgeGraph& graph, size_t iterations = 100, double width = 800.0, double height = 600.0);
};

} // namespace codeatlas
