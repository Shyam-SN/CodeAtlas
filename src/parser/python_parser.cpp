#include <codeatlas/parser/python_parser.hpp>
#include <sstream>
#include <regex>
#include <filesystem>
#include <unordered_set>

namespace codeatlas {

void PythonParser::parse(const std::string& file_path, const std::string& content, RepositoryModel& model) {
    std::string file_id = "file:" + file_path;
    std::string file_name = std::filesystem::path(file_path).filename().string();

    Entity file_entity;
    file_entity.id = file_id;
    file_entity.name = file_name;
    file_entity.kind = EntityKind::File;
    file_entity.file_path = file_path;
    model.add_entity(file_entity);

    std::istringstream stream(content);
    std::string line;
    size_t line_num = 0;

    std::regex import_regex(R"#(^\s*(?:import|from)\s+([a-zA-Z0-9_.]+))#");
    std::regex class_regex(R"#(^\s*class\s+([a-zA-Z0-9_]+))#");
    std::regex func_regex(R"#(^\s*def\s+([a-zA-Z0-9_]+)\s*\()#");
    std::regex call_regex(R"#((?:[a-zA-Z0-9_]+\.)?([a-zA-Z0-9_]{3,})\s*\()#");

    static const std::unordered_set<std::string> keywords = {
        "if", "elif", "while", "for", "with", "except", "return", "yield", "print", "len",
        "int", "str", "float", "list", "dict", "set", "tuple", "bool", "range", "zip", "enumerate"
    };

    std::string current_func_id;

    while (std::getline(stream, line)) {
        line_num++;
        std::smatch match;

        if (std::regex_search(line, match, import_regex)) {
            std::string module = match[1].str();
            model.add_relation(file_id, "module:" + module, "imports");
        } else if (std::regex_search(line, match, class_regex)) {
            std::string name = match[1].str();
            std::string class_id = "class:" + file_path + ":" + name;

            Entity class_entity;
            class_entity.id = class_id;
            class_entity.name = name;
            class_entity.kind = EntityKind::Class;
            class_entity.file_path = file_path;
            class_entity.start_line = line_num;
            class_entity.end_line = line_num + 20;
            class_entity.parent_id = file_id;

            model.add_entity(class_entity);
            model.add_relation(file_id, class_id, "contains");
        } else if (std::regex_search(line, match, func_regex)) {
            std::string name = match[1].str();
            if (name != "__init__" && name != "__main__") {
                std::string func_id = "func:" + file_path + ":" + name;
                current_func_id = func_id;

                Entity func_entity;
                func_entity.id = func_id;
                func_entity.name = name;
                func_entity.kind = EntityKind::Function;
                func_entity.file_path = file_path;
                func_entity.start_line = line_num;
                func_entity.end_line = line_num + 15;
                func_entity.parent_id = file_id;

                model.add_entity(func_entity);
                model.add_relation(file_id, func_id, "contains");
            }
        }

        // Function Calls
        if (!current_func_id.empty()) {
            std::string line_search = line;
            std::smatch call_match;
            while (std::regex_search(line_search, call_match, call_regex)) {
                std::string called_name = call_match[1].str();
                if (!keywords.count(called_name) && called_name != current_func_id) {
                    model.add_relation(current_func_id, called_name, "calls");
                }
                line_search = call_match.suffix().str();
            }
        }
    }
}

} // namespace codeatlas
