#include <codeatlas/graph/graph_builder.hpp>
#include <codeatlas/layout/hierarchical_layout.hpp>
#include <codeatlas/layout/force_directed.hpp>
#include <filesystem>
#include <unordered_set>

namespace codeatlas {

KnowledgeGraph GraphBuilder::build_graph(const RepositoryModel& model, GraphType type) {
    KnowledgeGraph g(type);
    switch (type) {
        case GraphType::MindMap:
            g = build_mind_map(model);
            break;
        case GraphType::FileDependency:
            g = build_file_dependency(model);
            break;
        case GraphType::ClassDiagram:
            g = build_class_diagram(model);
            break;
        case GraphType::CallGraph:
            g = build_call_graph(model);
            break;
        case GraphType::ArchitectureGraph:
        default:
            g = build_architecture_graph(model);
            break;
    }

    for (auto& node : const_cast<std::vector<GraphNode>&>(g.nodes())) {
        if (!node.entity_id.empty()) {
            if (auto entity = model.get_entity(node.entity_id)) {
                node.commit_count = entity->commit_count;
                node.complexity = entity->complexity;
            }
        }
    }

    audit_graph(g);

    if (type == GraphType::MindMap || type == GraphType::ArchitectureGraph) {
        HierarchicalLayout::apply_radial_mindmap(g, 1400.0, 1000.0);
    } else {
        ForceDirectedLayout::apply_layout(g, 60, 1000.0, 800.0);
    }

    return g;
}

KnowledgeGraph GraphBuilder::build_mind_map(const RepositoryModel& model) {
    KnowledgeGraph g(GraphType::MindMap);
    g.add_node({"root", "Repository Root", "Root", 0.0, 0.0, false, true, true, ""});

    std::unordered_set<std::string> folder_nodes;

    for (const auto& [id, entity] : model.all_entities()) {
        if (entity.kind == EntityKind::File) {
            std::filesystem::path p(entity.file_path);
            std::string folder_name = p.has_parent_path() ? p.parent_path().filename().string() : "root";
            if (folder_name.empty()) folder_name = "root";

            std::string folder_id = "folder:" + folder_name;
            if (folder_nodes.find(folder_id) == folder_nodes.end()) {
                folder_nodes.insert(folder_id);
                g.add_node({folder_id, folder_name, "Folder", 0.0, 0.0, false, true, true, folder_id});
                g.add_edge({"root", folder_id, "contains", 1.0, false});
            }

            g.add_node({entity.id, entity.name, "File", 0.0, 0.0, false, true, true, entity.id});
            g.add_edge({folder_id, entity.id, "contains", 1.0, false});
        } else if (entity.kind == EntityKind::Class || entity.kind == EntityKind::Function) {
            if (!entity.parent_id.empty()) {
                g.add_node({entity.id, entity.name, entity_kind_to_string(entity.kind), 0.0, 0.0, false, true, true, entity.id});
                g.add_edge({entity.parent_id, entity.id, "defines", 1.0, false});
            }
        }
    }
    return g;
}

KnowledgeGraph GraphBuilder::build_file_dependency(const RepositoryModel& model) {
    KnowledgeGraph g(GraphType::FileDependency);
    auto files = model.get_entities_by_kind(EntityKind::File);

    for (const auto& file : files) {
        g.add_node({file.id, file.name, "File", 0.0, 0.0, false, true, true, file.id});
        for (const auto& rel : file.relations) {
            if (rel.type == "includes" || rel.type == "imports") {
                std::string target_name = std::filesystem::path(rel.target_id).filename().string();
                g.add_node({rel.target_id, target_name, "File", 0.0, 0.0, false, true, true, rel.target_id});
                g.add_edge({file.id, rel.target_id, rel.type, 1.0, false});
            }
        }
    }
    return g;
}

KnowledgeGraph GraphBuilder::build_class_diagram(const RepositoryModel& model) {
    KnowledgeGraph g(GraphType::ClassDiagram);
    auto classes = model.get_entities_by_kind(EntityKind::Class);
    auto structs = model.get_entities_by_kind(EntityKind::Struct);

    for (const auto& cls : classes) {
        g.add_node({cls.id, cls.name, "Class", 0.0, 0.0, false, true, true, cls.id});
    }
    for (const auto& st : structs) {
        g.add_node({st.id, st.name, "Struct", 0.0, 0.0, false, true, true, st.id});
    }
    return g;
}

KnowledgeGraph GraphBuilder::build_call_graph(const RepositoryModel& model) {
    KnowledgeGraph g(GraphType::CallGraph);
    auto funcs = model.get_entities_by_kind(EntityKind::Function);
    
    std::unordered_set<std::string> id_set;
    std::unordered_map<std::string, std::string> name_to_id;
    for (const auto& func : funcs) {
        id_set.insert(func.id);
        name_to_id[func.name] = func.id;
        g.add_node({func.id, func.name, "Function", 0.0, 0.0, false, true, true, func.id});
    }

    for (const auto& func : funcs) {
        for (const auto& rel : func.relations) {
            if (rel.type == "calls") {
                std::string target_id = rel.target_id;
                // If it's just a function name, resolve to entity ID
                if (name_to_id.count(target_id)) {
                    target_id = name_to_id[target_id];
                }
                
                if (id_set.count(target_id)) {
                    g.add_edge({func.id, target_id, "calls", 1.0, false});
                }
            }
        }
    }

    return g;
}

KnowledgeGraph GraphBuilder::build_architecture_graph(const RepositoryModel& model) {
    return build_mind_map(model);
}

void GraphBuilder::audit_graph(KnowledgeGraph& g) {
    if (g.type() == GraphType::MindMap) return;

    std::unordered_map<std::string, int> in_degree;
    std::unordered_map<std::string, std::vector<std::string>> adj;
    
    for (const auto& node : g.nodes()) {
        in_degree[node.id] = 0;
    }
    
    for (const auto& edge : g.edges()) {
        in_degree[edge.target_id]++;
        adj[edge.source_id].push_back(edge.target_id);
    }

    // 1. Dead Code Detection
    for (auto& node : const_cast<std::vector<GraphNode>&>(g.nodes())) {
        if (in_degree[node.id] == 0 && node.category != "Folder" && node.category != "Root" && node.label != "main") {
            node.is_dead_code = true;
        }
    }

    // 2. Circular Dependency Detection (Tarjan's SCC)
    int time = 0;
    std::unordered_map<std::string, int> disc;
    std::unordered_map<std::string, int> low;
    std::unordered_map<std::string, bool> in_stack;
    std::vector<std::string> st;
    std::vector<std::unordered_set<std::string>> sccs;

    auto tarjan = [&](auto& self, const std::string& u) -> void {
        disc[u] = low[u] = ++time;
        st.push_back(u);
        in_stack[u] = true;

        for (const auto& v : adj[u]) {
            if (!disc[v]) {
                self(self, v);
                low[u] = std::min(low[u], low[v]);
            } else if (in_stack[v]) {
                low[u] = std::min(low[u], disc[v]);
            }
        }

        if (low[u] == disc[u]) {
            std::unordered_set<std::string> scc;
            while (true) {
                std::string v = st.back();
                st.pop_back();
                in_stack[v] = false;
                scc.insert(v);
                if (u == v) break;
            }
            if (scc.size() > 1) {
                sccs.push_back(scc);
            }
        }
    };

    for (const auto& node : g.nodes()) {
        if (!disc[node.id]) {
            tarjan(tarjan, node.id);
        }
    }

    // Mark nodes and edges as circular
    for (const auto& scc : sccs) {
        for (auto& node : const_cast<std::vector<GraphNode>&>(g.nodes())) {
            if (scc.count(node.id)) {
                node.is_circular = true;
            }
        }
        for (auto& edge : const_cast<std::vector<GraphEdge>&>(g.edges())) {
            if (scc.count(edge.source_id) && scc.count(edge.target_id)) {
                edge.is_circular = true;
            }
        }
    }
}

} // namespace codeatlas
