#pragma once

#include <string>
#include <codeatlas/model/repository_model.hpp>

namespace codeatlas {

class JavascriptParser {
public:
    static void parse(const std::string& file_path, const std::string& content, RepositoryModel& model);
};

} // namespace codeatlas
