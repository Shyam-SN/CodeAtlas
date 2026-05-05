#include <codeatlas/graph/knowledge_graph.hpp>

namespace codeatlas {

void KnowledgeGraph::add_node(GraphNode node) {
    if (node_index_.find(node.id) == node_index_.end()) {
        node_index_[node.id] = nodes_.size();
        nodes_.push_back(std::move(node));
    }
}

void KnowledgeGraph::add_edge(GraphEdge edge) {
    edges_.push_back(std::move(edge));
}

GraphNode* KnowledgeGraph::get_node_mut(const std::string& id) {
    auto it = node_index_.find(id);
    if (it != node_index_.end()) {
        return &nodes_[it->second];
    }
    return nullptr;
}

bool KnowledgeGraph::toggle_node_collapse(const std::string& id) {
    if (GraphNode* node = get_node_mut(id)) {
        node->collapsed = !node->collapsed;
        return true;
    }
    return false;
}

void KnowledgeGraph::highlight_path(const std::vector<std::string>& node_ids) {
    std::unordered_set<std::string> set(node_ids.begin(), node_ids.end());
    for (auto& node : nodes_) {
        node.highlighted = set.count(node.id) > 0;
    }
    for (auto& edge : edges_) {
        edge.highlighted = set.count(edge.source_id) > 0 && set.count(edge.target_id) > 0;
    }
}

} // namespace codeatlas
