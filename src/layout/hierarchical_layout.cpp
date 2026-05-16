#include <codeatlas/layout/hierarchical_layout.hpp>
#include <cmath>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <queue>

namespace codeatlas {

void HierarchicalLayout::apply_layout(KnowledgeGraph& graph, double width, double height) {
    apply_radial_mindmap(graph, width, height);
}

void HierarchicalLayout::apply_radial_mindmap(KnowledgeGraph& graph, double width, double height) {
    auto& nodes = const_cast<std::vector<GraphNode>&>(graph.nodes());
    const auto& edges = graph.edges();
    size_t N = nodes.size();
    if (N == 0) return;

    double center_x = width / 2.0;
    double center_y = height / 2.0;

    // Reset visibility to false, we will only set true for expanded branches
    for (auto& node : nodes) {
        node.visible = false;
    }

    std::unordered_map<std::string, size_t> id_to_idx;
    for (size_t i = 0; i < N; ++i) {
        id_to_idx[nodes[i].id] = i;
    }

    std::unordered_map<std::string, std::vector<std::string>> children_map;
    std::unordered_set<std::string> child_set;

    for (const auto& edge : edges) {
        children_map[edge.source_id].push_back(edge.target_id);
        child_set.insert(edge.target_id);
    }

    // Identify Root
    std::string root_id = "root";
    if (id_to_idx.find(root_id) == id_to_idx.end()) {
        for (const auto& node : nodes) {
            if (child_set.find(node.id) == child_set.end()) {
                root_id = node.id;
                break;
            }
        }
    }

    if (id_to_idx.find(root_id) != id_to_idx.end()) {
        size_t root_idx = id_to_idx[root_id];
        nodes[root_idx].x = center_x;
        nodes[root_idx].y = center_y;
        nodes[root_idx].visible = true;
        // Root is always expanded initially if we want to see Level 1
    }

    const auto& folders = children_map[root_id];
    size_t num_folders = folders.empty() ? 1 : folders.size();
    double folder_angle_step = (2.0 * M_PI) / static_cast<double>(num_folders);
    double R1 = 220.0;

    for (size_t i = 0; i < folders.size(); ++i) {
        const std::string& folder_id = folders[i];
        if (id_to_idx.find(folder_id) == id_to_idx.end()) continue;

        size_t f_idx = id_to_idx[folder_id];
        nodes[f_idx].visible = true; // Folders always visible

        double angle_mid = i * folder_angle_step;
        nodes[f_idx].x = center_x + R1 * std::cos(angle_mid);
        nodes[f_idx].y = center_y + R1 * std::sin(angle_mid);

        // If folder is collapsed, do not process children
        if (nodes[f_idx].collapsed) continue;

        const auto& files = children_map[folder_id];
        if (files.empty()) continue;

        double R2 = R1 + 180.0;
        double file_angle_span = folder_angle_step * 0.85;
        double file_angle_start = angle_mid - file_angle_span / 2.0;
        double file_angle_step = file_angle_span / static_cast<double>(std::max<size_t>(1, files.size()));

        for (size_t j = 0; j < files.size(); ++j) {
            const std::string& file_id = files[j];
            if (id_to_idx.find(file_id) == id_to_idx.end()) continue;

            size_t file_idx = id_to_idx[file_id];
            nodes[file_idx].visible = true;

            double file_angle = file_angle_start + (j + 0.5) * file_angle_step;
            nodes[file_idx].x = center_x + R2 * std::cos(file_angle);
            nodes[file_idx].y = center_y + R2 * std::sin(file_angle);

            // If file is collapsed, skip children
            if (nodes[file_idx].collapsed) continue;

            const auto& symbols = children_map[file_id];
            if (symbols.empty()) continue;

            double R3 = R2 + 160.0;
            double sym_angle_span = file_angle_step * 0.9;
            double sym_angle_start = file_angle - sym_angle_span / 2.0;
            double sym_angle_step = sym_angle_span / static_cast<double>(std::max<size_t>(1, symbols.size()));

            for (size_t k = 0; k < symbols.size(); ++k) {
                const std::string& sym_id = symbols[k];
                if (id_to_idx.find(sym_id) == id_to_idx.end()) continue;

                size_t sym_idx = id_to_idx[sym_id];
                nodes[sym_idx].visible = true;

                double sym_angle = sym_angle_start + (k + 0.5) * sym_angle_step;
                nodes[sym_idx].x = center_x + R3 * std::cos(sym_angle);
                nodes[sym_idx].y = center_y + R3 * std::sin(sym_angle);
            }
        }
    }
}

} // namespace codeatlas
