#include <codeatlas/codeatlas.hpp>
#include <iostream>
#include <iomanip>

using namespace codeatlas;

int main() {
    std::cout << "=========================================================\n";
    std::cout << " CodeAtlas: AI-Assisted Software Repository Platform     \n";
    std::cout << "=========================================================\n\n";

    Timer timer;

    // 1. Parsing Repository
    std::cout << "=== 1. Multi-Language Repository AST Indexing ===\n";
    RepositoryModel model;
    ParserEngine::parse_repository("/Users/prathima/Desktop/Shyam/CodeAtlas", model);
    RepositoryIndexer::build_index(model);

    std::cout << "[INFO] Successfully indexed " << model.entity_count() << " entities.\n";
    auto files = model.get_entities_by_kind(EntityKind::File);
    auto classes = model.get_entities_by_kind(EntityKind::Class);
    auto funcs = model.get_entities_by_kind(EntityKind::Function);

    std::cout << "  • Files: " << files.size() << "\n";
    std::cout << "  • Classes/Structs: " << classes.size() << "\n";
    std::cout << "  • Functions: " << funcs.size() << "\n";

    // 2. Generating Knowledge Graph Views
    std::cout << "\n=== 2. Knowledge Graph Generation & Layout ===\n";
    Timer graph_timer;
    auto mind_map = GraphBuilder::build_graph(model, GraphType::MindMap);
    ForceDirectedLayout::apply_layout(mind_map, 50, 800.0, 600.0);
    std::cout << "[BENCHMARK] Mind Map generated with " << mind_map.nodes().size() << " nodes in " << graph_timer.elapsed_ms() << " ms.\n";

    Timer call_timer;
    auto call_graph = GraphBuilder::build_graph(model, GraphType::CallGraph);
    HierarchicalLayout::apply_layout(call_graph, 800.0, 600.0);
    std::cout << "[BENCHMARK] Function Call Graph generated in " << call_timer.elapsed_ms() << " ms.\n";

    // 3. AI Assistant Repository Navigation
    std::cout << "\n=== 3. AI Assistant Natural Language Navigation ===\n";
    AIAssistant assistant(model);

    std::vector<std::string> sample_queries = {
        "parser",
        "graph",
        "model"
    };

    for (const auto& q : sample_queries) {
        std::cout << "\n--- User Question: \"" << q << "\" ---\n";
        auto answer = assistant.answer_question(q, mind_map);
        std::cout << answer.text;
        std::cout << "[AI] Highlighted Node IDs: " << answer.highlighted_node_ids.size() << " nodes.\n";
    }

    std::cout << "\nDemo executed successfully in " << timer.elapsed_ms() << " ms.\n";
    std::cout << "=========================================================\n";

    return 0;
}
