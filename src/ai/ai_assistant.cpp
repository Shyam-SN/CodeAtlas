#include <codeatlas/ai/ai_assistant.hpp>
#include <algorithm>
#include <sstream>

namespace codeatlas {

AIAnswer AIAssistant::answer_question(const std::string& query, const KnowledgeGraph& current_graph) const {
    (void)current_graph;
    AIAnswer answer;
    std::string lower_query = query;
    std::transform(lower_query.begin(), lower_query.end(), lower_query.begin(), ::tolower);

    auto matches = model_.search_entities(lower_query);

    std::ostringstream ss;
    ss << "=== CodeAtlas AI Repository Assistant ===\n";
    ss << "Analysis query: \"" << query << "\"\n\n";

    if (matches.empty()) {
        ss << "Found " << model_.entity_count() << " indexed entities in repository.\n";
        ss << "Here is the architectural overview of key modules:\n";
        for (const auto& [id, entity] : model_.all_entities()) {
            if (entity.kind == EntityKind::Class || entity.kind == EntityKind::Function) {
                answer.highlighted_node_ids.push_back(entity.id);
                answer.relevant_files.push_back(entity.file_path);
            }
        }
    } else {
        ss << "Identified " << matches.size() << " matching architectural components:\n";
        for (const auto& entity : matches) {
            ss << "• [" << entity_kind_to_string(entity.kind) << "] " << entity.name
               << " (File: " << entity.file_path << ", Line: " << entity.start_line << ")\n";
            answer.highlighted_node_ids.push_back(entity.id);
            answer.relevant_files.push_back(entity.file_path);
        }
    }

    answer.text = ss.str();
    return answer;
}

} // namespace codeatlas
