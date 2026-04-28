#include <codeatlas/ai/security_scanner.hpp>
#include <fstream>
#include <regex>
#include <iostream>
#include <unordered_set>

namespace codeatlas {

struct Rule {
    std::string type;
    std::string description;
    Severity severity;
    std::regex pattern;
};

std::vector<Vulnerability> SecurityScanner::scan(const RepositoryModel& model) {
    std::vector<Vulnerability> results;

    std::vector<Rule> rules = {
        {"Hardcoded Secret", "Detected a potential hardcoded password or token.", Severity::Critical, std::regex(R"((?i)(password|passwd|pwd|api_key|secret|token)\s*=\s*["'][^"']+["'])")},
        {"Dangerous Function", "Usage of unsafe function strcpy(). Consider using strncpy().", Severity::High, std::regex(R"(\bstrcpy\s*\()")},
        {"Dangerous Function", "Usage of unsafe function sprintf(). Consider using snprintf().", Severity::High, std::regex(R"(\bsprintf\s*\()")},
        {"Command Injection", "Usage of system() can lead to arbitrary code execution.", Severity::Critical, std::regex(R"(\bsystem\s*\()")},
        {"SQL Injection", "Potential string concatenation in SQL query.", Severity::High, std::regex(R"((?i)(SELECT|UPDATE|INSERT|DELETE).*\+.*)")}
    };

    // Group entities by file
    std::unordered_map<std::string, std::vector<const Entity*>> file_to_entities;
    for (const auto& [id, entity] : model.all_entities()) {
        if (!entity.file_path.empty()) {
            file_to_entities[entity.file_path].push_back(&entity);
        }
    }

    // Process each file
    for (const auto& [file_path, entities] : file_to_entities) {
        std::ifstream file(file_path);
        if (!file.is_open()) continue;

        std::string line;
        size_t line_number = 1;
        while (std::getline(file, line)) {
            for (const auto& rule : rules) {
                if (std::regex_search(line, rule.pattern)) {
                    // Find most specific entity
                    const Entity* best_entity = nullptr;
                    size_t min_span = static_cast<size_t>(-1);

                    for (const Entity* e : entities) {
                        if (line_number >= e->start_line && line_number <= e->end_line) {
                            size_t span = e->end_line - e->start_line;
                            if (span < min_span) {
                                min_span = span;
                                best_entity = e;
                            }
                        }
                    }

                    if (best_entity) {
                        results.push_back({
                            best_entity->id,
                            best_entity->name,
                            file_path,
                            line_number,
                            rule.severity,
                            rule.type,
                            rule.description
                        });
                    }
                }
            }
            line_number++;
        }
    }

    return results;
}

} // namespace codeatlas
