#include <codeatlas/utils/file_utils.hpp>
#include <fstream>
#include <sstream>

namespace codeatlas {

namespace fs = std::filesystem;

std::vector<std::string> FileUtils::list_repository_files(const std::string& root_dir) {
    std::vector<std::string> files;
    if (!fs::exists(root_dir)) return files;

    for (const auto& entry : fs::recursive_directory_iterator(root_dir)) {
        if (entry.is_regular_file()) {
            std::string path = entry.path().string();
            // Exclude hidden folders like .git or build
            if (path.find("/.") == std::string::npos && path.find("/build/") == std::string::npos) {
                files.push_back(path);
            }
        }
    }
    return files;
}

std::string FileUtils::read_file_content(const std::string& file_path) {
    std::ifstream in(file_path, std::ios::in | std::ios::binary);
    if (!in) return "";
    std::ostringstream contents;
    contents << in.rdbuf();
    return contents.str();
}

std::string FileUtils::get_file_extension(const std::string& file_path) {
    fs::path p(file_path);
    return p.extension().string();
}

} // namespace codeatlas
