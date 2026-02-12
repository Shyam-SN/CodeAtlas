#pragma once

#include <codeatlas/model/repository_model.hpp>

namespace codeatlas {

class ParserEngine {
public:
    static void parse_repository(const std::string& repo_path, RepositoryModel& model);
    static void parse_file(const std::string& file_path, RepositoryModel& model);
};

} // namespace codeatlas
