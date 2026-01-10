#pragma once

#include <string>
#include <vector>
#include <codeatlas/model/repository_model.hpp>

namespace codeatlas {

enum class Severity {
    Low,
    Medium,
    High,
    Critical
};

inline std::string severity_to_string(Severity s) {
    switch (s) {
        case Severity::Low: return "Low";
        case Severity::Medium: return "Medium";
        case Severity::High: return "High";
        case Severity::Critical: return "Critical";
    }
    return "Unknown";
}

struct Vulnerability {
    std::string entity_id;
    std::string entity_name;
    std::string file_path;
    size_t line_number;
    Severity severity;
    std::string type;
    std::string description;
};

class SecurityScanner {
public:
    static std::vector<Vulnerability> scan(const RepositoryModel& model);
};

} // namespace codeatlas
