#include <codeatlas/ai/repository_indexer.hpp>

namespace codeatlas {

void RepositoryIndexer::build_index(RepositoryModel& model) {
    (void)model;
    // Build inverted symbol index for fast retrieval
}

void RepositoryIndexer::calculate_git_churn(const std::string& repo_path, RepositoryModel& model) {
    std::string cmd = "git -C \"" + repo_path + "\" log --name-only --format=\"\" | sort | uniq -c";
    
    std::unordered_map<std::string, int> file_churn;
    
    FILE* pipe = popen(cmd.c_str(), "r");
    if (!pipe) return;
    
    char buffer[1024];
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        std::string line(buffer);
        // Trim whitespace
        line.erase(0, line.find_first_not_of(" \t\n\r"));
        line.erase(line.find_last_not_of(" \t\n\r") + 1);
        if (line.empty()) continue;
        
        // Parse "  123 filename.ext"
        size_t space_pos = line.find_first_of(" \t");
        if (space_pos != std::string::npos) {
            std::string count_str = line.substr(0, space_pos);
            std::string filename = line.substr(space_pos + 1);
            filename.erase(0, filename.find_first_not_of(" \t"));
            
            try {
                int count = std::stoi(count_str);
                // The filename from git log is relative to the repo root.
                // We need to match it against entity.file_path, which might be absolute.
                // For simplicity, we can store it in the map and when updating entities,
                // check if the entity.file_path ends with this filename.
                // Or better, construct the absolute path here assuming repo_path is absolute.
                
                std::string absolute_path = repo_path;
                if (!absolute_path.empty() && absolute_path.back() != '/') {
                    absolute_path += '/';
                }
                absolute_path += filename;
                
                file_churn[absolute_path] = count;
            } catch (...) {
                // Ignore parsing errors
            }
        }
    }
    pclose(pipe);
    
    model.update_entity_commit_counts(file_churn);
}

} // namespace codeatlas
