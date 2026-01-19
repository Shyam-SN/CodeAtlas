#pragma once

#include <string>

namespace codeatlas {

enum class GraphType {
    MindMap,
    FolderDependency,
    FileDependency,
    ClassDiagram,
    CallGraph,
    ModuleDependency,
    BuildGraph,
    KnowledgeGraph,
    GitGraph,
    ArchitectureGraph
};

inline std::string graph_type_to_string(GraphType type) {
    switch (type) {
        case GraphType::MindMap: return "Repository Mind Map";
        case GraphType::FolderDependency: return "Folder Dependency Graph";
        case GraphType::FileDependency: return "File Dependency Graph";
        case GraphType::ClassDiagram: return "Class Diagram";
        case GraphType::CallGraph: return "Function Call Graph";
        case GraphType::ModuleDependency: return "Module Dependency Graph";
        case GraphType::BuildGraph: return "Build Target Graph";
        case GraphType::KnowledgeGraph: return "Knowledge Graph";
        case GraphType::GitGraph: return "Git Graph";
        case GraphType::ArchitectureGraph: return "Inferred Architecture Graph";
    }
    return "Custom Graph";
}

} // namespace codeatlas
