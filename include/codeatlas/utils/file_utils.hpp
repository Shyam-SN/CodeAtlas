#pragma once

#include <string>
#include <vector>
#include <filesystem>

namespace codeatlas {

class FileUtils {
public:
    static std::vector<std::string> list_repository_files(const std::string& root_dir);
    static std::string read_file_content(const std::string& file_path);
    static std::string get_file_extension(const std::string& file_path);
};

} // namespace codeatlas
