#include <codeatlas/parser/parser_engine.hpp>
#include <codeatlas/parser/cpp_parser.hpp>
#include <codeatlas/parser/python_parser.hpp>
#include <codeatlas/parser/cmake_parser.hpp>
#include <codeatlas/parser/javascript_parser.hpp>
#include <codeatlas/utils/file_utils.hpp>

namespace codeatlas {

void ParserEngine::parse_repository(const std::string& repo_path, RepositoryModel& model) {
    model.set_repository_path(repo_path);
    auto files = FileUtils::list_repository_files(repo_path);

    for (const auto& file : files) {
        parse_file(file, model);
    }
}

void ParserEngine::parse_file(const std::string& file_path, RepositoryModel& model) {
    std::string ext = FileUtils::get_file_extension(file_path);
    std::string content = FileUtils::read_file_content(file_path);
    if (content.empty()) return;

    if (ext == ".cpp" || ext == ".hpp" || ext == ".c" || ext == ".h" || ext == ".cc" || ext == ".cxx") {
        CppParser::parse(file_path, content, model);
    } else if (ext == ".py") {
        PythonParser::parse(file_path, content, model);
    } else if (ext == ".js" || ext == ".ts" || ext == ".jsx" || ext == ".tsx") {
        JavascriptParser::parse(file_path, content, model);
    } else if (ext == ".txt" || file_path.find("CMakeLists") != std::string::npos) {
        CMakeParser::parse(file_path, content, model);
    } else {
        // Generic file entry
        Entity file_entity;
        file_entity.id = "file:" + file_path;
        file_entity.name = std::filesystem::path(file_path).filename().string();
        file_entity.kind = EntityKind::File;
        file_entity.file_path = file_path;
        model.add_entity(file_entity);
    }
}

} // namespace codeatlas
