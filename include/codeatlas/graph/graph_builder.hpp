#pragma once

#include <codeatlas/model/repository_model.hpp>
#include <codeatlas/graph/knowledge_graph.hpp>

namespace codeatlas {

class GraphBuilder {
public:
    static KnowledgeGraph build_graph(const RepositoryModel& model, GraphType type);

private:
    static KnowledgeGraph build_mind_map(const RepositoryModel& model);
    static KnowledgeGraph build_file_dependency(const RepositoryModel& model);
    static KnowledgeGraph build_class_diagram(const RepositoryModel& model);
    static KnowledgeGraph build_call_graph(const RepositoryModel& model);
    static KnowledgeGraph build_architecture_graph(const RepositoryModel& model);
    static void audit_graph(KnowledgeGraph& g);
};

} // namespace codeatlas
