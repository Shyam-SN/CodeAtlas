#pragma once

#include <codeatlas/model/repository_model.hpp>

namespace codeatlas {

class CppParser {
public:
    static void parse(const std::string& file_path, const std::string& content, RepositoryModel& model);
};

} // namespace codeatlas
