#include <codeatlas/parser/cmake_parser.hpp>
#include <sstream>
#include <regex>

namespace codeatlas {

void CMakeParser::parse(const std::string& file_path, const std::string& content, RepositoryModel& model) {
    std::string file_id = "file:" + file_path;

    std::istringstream stream(content);
    std::string line;
    size_t line_num = 0;

    std::regex target_regex(R"#(^\s*(add_executable|add_library)\s*\(\s*([A-Za-z0-9_]+))#");
    std::regex link_regex(R"#(^\s*target_link_libraries\s*\(\s*([A-Za-z0-9_]+))#");

    while (std::getline(stream, line)) {
        line_num++;
        std::smatch match;

        if (std::regex_search(line, match, target_regex)) {
            std::string name = match[2].str();
            std::string target_id = "target:" + name;

            Entity target_entity;
            target_entity.id = target_id;
            target_entity.name = name;
            target_entity.kind = EntityKind::BuildTarget;
            target_entity.file_path = file_path;
            target_entity.start_line = line_num;

            model.add_entity(target_entity);
            model.add_relation(file_id, target_id, "defines");
        } else if (std::regex_search(line, match, link_regex)) {
            std::string target = match[1].str();
            model.add_relation("target:" + target, file_id, "configured_by");
        }
    }
}

} // namespace codeatlas
