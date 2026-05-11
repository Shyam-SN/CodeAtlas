#include <codeatlas/layout/force_directed.hpp>
#include <random>
#include <cmath>
#include <algorithm>
#include <unordered_map>

namespace codeatlas {

void ForceDirectedLayout::apply_layout(KnowledgeGraph& graph, size_t iterations, double width, double height) {
    auto& nodes = const_cast<std::vector<GraphNode>&>(graph.nodes());
    const auto& edges = graph.edges();
    size_t N = nodes.size();
    if (N == 0) return;

    double center_x = width / 2.0;
    double center_y = height / 2.0;

    std::unordered_map<std::string, size_t> node_id_to_idx;
    for (size_t i = 0; i < N; ++i) {
        node_id_to_idx[nodes[i].id] = i;
    }

    // Initial Placement: Concentric Radial Tree Placement around Root
    std::mt19937 rng(42);
    std::uniform_real_distribution<double> dist_angle(0.0, 2.0 * M_PI);
    std::uniform_real_distribution<double> dist_radius(80.0, std::min(width, height) * 0.35);

    for (size_t i = 0; i < N; ++i) {
        if (nodes[i].category == "Root") {
            nodes[i].x = center_x;
            nodes[i].y = center_y;
        } else {
            double angle = dist_angle(rng);
            double radius = dist_radius(rng);
            nodes[i].x = center_x + radius * std::cos(angle);
            nodes[i].y = center_y + radius * std::sin(angle);
        }
    }

    double area = width * height;
    double k = std::sqrt(area / static_cast<double>(N)) * 1.5;
    double temp = width / 6.0;

    for (size_t iter = 0; iter < iterations; ++iter) {
        std::vector<std::pair<double, double>> disp(N, {0.0, 0.0});

        // 1. Repulsive forces between all node pairs
        for (size_t i = 0; i < N; ++i) {
            for (size_t j = i + 1; j < N; ++j) {
                double dx = nodes[i].x - nodes[j].x;
                double dy = nodes[i].y - nodes[j].y;
                double dist = std::sqrt(dx * dx + dy * dy);
                if (dist < 1e-3) dist = 1e-3;

                double fr = (k * k) / dist;
                disp[i].first += (dx / dist) * fr;
                disp[i].second += (dy / dist) * fr;

                disp[j].first -= (dx / dist) * fr;
                disp[j].second -= (dy / dist) * fr;
            }
        }

        // 2. Attractive forces along graph edges
        for (const auto& edge : edges) {
            auto it1 = node_id_to_idx.find(edge.source_id);
            auto it2 = node_id_to_idx.find(edge.target_id);
            if (it1 != node_id_to_idx.end() && it2 != node_id_to_idx.end()) {
                size_t u = it1->second;
                size_t v = it2->second;

                double dx = nodes[u].x - nodes[v].x;
                double dy = nodes[u].y - nodes[v].y;
                double dist = std::sqrt(dx * dx + dy * dy);
                if (dist < 1e-3) dist = 1e-3;

                double fa = (dist * dist) / k;
                disp[u].first -= (dx / dist) * fa;
                disp[u].second -= (dy / dist) * fa;

                disp[v].first += (dx / dist) * fa;
                disp[v].second += (dy / dist) * fa;
            }
        }

        // 3. Center Gravity Pull (keeps layout organic and centered)
        for (size_t i = 0; i < N; ++i) {
            double c_dx = center_x - nodes[i].x;
            double c_dy = center_y - nodes[i].y;
            disp[i].first += c_dx * 0.08;
            disp[i].second += c_dy * 0.08;
        }

        // 4. Displacement application bounded by temperature
        for (size_t i = 0; i < N; ++i) {
            if (nodes[i].category == "Root") continue; // Fix root at center

            double dlen = std::sqrt(disp[i].first * disp[i].first + disp[i].second * disp[i].second);
            if (dlen > 1e-3) {
                nodes[i].x += (disp[i].first / dlen) * std::min(dlen, temp);
                nodes[i].y += (disp[i].second / dlen) * std::min(dlen, temp);
            }
        }

        temp *= 0.95;
    }

    // Smooth Soft Rescaling to fit within canvas bounds
    double min_x = width, max_x = 0, min_y = height, max_y = 0;
    for (size_t i = 0; i < N; ++i) {
        min_x = std::min(min_x, nodes[i].x);
        max_x = std::max(max_x, nodes[i].x);
        min_y = std::min(min_y, nodes[i].y);
        max_y = std::max(max_y, nodes[i].y);
    }

    double margin = 60.0;
    double scale_x = (max_x > min_x) ? (width - 2 * margin) / (max_x - min_x) : 1.0;
    double scale_y = (max_y > min_y) ? (height - 2 * margin) / (max_y - min_y) : 1.0;
    double scale = std::min(scale_x, scale_y);

    for (size_t i = 0; i < N; ++i) {
        nodes[i].x = center_x + (nodes[i].x - (min_x + max_x) / 2.0) * scale;
        nodes[i].y = center_y + (nodes[i].y - (min_y + max_y) / 2.0) * scale;
    }
}

} // namespace codeatlas
