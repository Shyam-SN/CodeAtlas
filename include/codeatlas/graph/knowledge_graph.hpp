#pragma once

#include <codeatlas/graph/graph_types.hpp>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>

namespace codeatlas {

struct GraphNode {
    std::string id;
    std::string label;
    std::string category;
    double x{0.0};
    double y{0.0};
    bool highlighted{false};
    bool collapsed{true}; // Default to collapsed for clean Mind Map view
    bool visible{true};
    std::string entity_id;
    bool is_dead_code{false};
    bool is_circular{false};
    int commit_count{0};
    int complexity{1};
};

struct GraphEdge {
    std::string source_id;
    std::string target_id;
    std::string label;
    double weight{1.0};
    bool highlighted{false};
    bool is_circular{false};
};

class KnowledgeGraph {
public:
    explicit KnowledgeGraph(GraphType type = GraphType::MindMap) : type_(type) {}

    [[nodiscard]] GraphType type() const noexcept { return type_; }
    void set_type(GraphType type) { type_ = type; }

    void add_node(GraphNode node);
    void add_edge(GraphEdge edge);

    [[nodiscard]] const std::vector<GraphNode>& nodes() const noexcept { return nodes_; }
    [[nodiscard]] const std::vector<GraphEdge>& edges() const noexcept { return edges_; }

    GraphNode* get_node_mut(const std::string& id);
    bool toggle_node_collapse(const std::string& id);

    void clear() { nodes_.clear(); edges_.clear(); node_index_.clear(); }

    void highlight_path(const std::vector<std::string>& node_ids);

private:
    GraphType type_{GraphType::MindMap};
    std::vector<GraphNode> nodes_;
    std::vector<GraphEdge> edges_;
    std::unordered_map<std::string, size_t> node_index_;
};

} // namespace codeatlas
