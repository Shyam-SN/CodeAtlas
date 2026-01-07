#pragma once

#include <codeatlas/model/repository_model.hpp>
#include <codeatlas/graph/knowledge_graph.hpp>
#include <string>
#include <vector>

namespace codeatlas {

struct AIAnswer {
    std::string text;
    std::vector<std::string> highlighted_node_ids;
    std::vector<std::string> relevant_files;
};

class AIAssistant {
public:
    explicit AIAssistant(const RepositoryModel& model) : model_(model) {}

    AIAnswer answer_question(const std::string& query, const KnowledgeGraph& current_graph) const;

private:
    const RepositoryModel& model_;
};

} // namespace codeatlas
