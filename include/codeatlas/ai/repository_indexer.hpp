#pragma once

#include <codeatlas/model/repository_model.hpp>

namespace codeatlas {

class RepositoryIndexer {
public:
    static void build_index(RepositoryModel& model);
    static void calculate_git_churn(const std::string& repo_path, RepositoryModel& model);
};

} // namespace codeatlas
