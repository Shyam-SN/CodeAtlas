#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

namespace codeatlas {

enum class EntityKind {
    Folder,
    File,
    Namespace,
    Class,
    Struct,
    Enum,
    Function,
    Method,
    Variable,
    BuildTarget,
    DocPage
};

inline std::string entity_kind_to_string(EntityKind kind) {
    switch (kind) {
        case EntityKind::Folder: return "Folder";
        case EntityKind::File: return "File";
        case EntityKind::Namespace: return "Namespace";
        case EntityKind::Class: return "Class";
        case EntityKind::Struct: return "Struct";
        case EntityKind::Enum: return "Enum";
        case EntityKind::Function: return "Function";
        case EntityKind::Method: return "Method";
        case EntityKind::Variable: return "Variable";
        case EntityKind::BuildTarget: return "BuildTarget";
        case EntityKind::DocPage: return "DocPage";
    }
    return "Unknown";
}

struct Relation {
    std::string type; // "inherits", "calls", "includes", "contains", "depends_on"
    std::string target_id;
};

struct Entity {
    std::string id;
    std::string name;
    EntityKind kind{EntityKind::File};
    std::string file_path;
    size_t start_line{1};
    size_t end_line{1};
    std::string docstring;
    std::string parent_id;
    std::vector<Relation> relations;
    std::vector<std::string> tags;
    std::unordered_map<std::string, std::string> metadata;
    int commit_count{0};
    int complexity{1};
};

} // namespace codeatlas
