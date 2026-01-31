#pragma once

#include <codeatlas/model/entity.hpp>
#include <unordered_map>
#include <vector>
#include <string>
#include <mutex>
#include <optional>

namespace codeatlas {

class RepositoryModel {
public:
    RepositoryModel() = default;

    void set_repository_path(std::string path) { repo_path_ = std::move(path); }
    [[nodiscard]] const std::string& repository_path() const noexcept { return repo_path_; }

    void add_entity(Entity entity);
    [[nodiscard]] std::optional<Entity> get_entity(const std::string& id) const;
    [[nodiscard]] std::vector<Entity> get_entities_by_kind(EntityKind kind) const;
    [[nodiscard]] std::vector<Entity> search_entities(const std::string& query) const;
    [[nodiscard]] const std::unordered_map<std::string, Entity>& all_entities() const noexcept { return entities_; }

    void add_relation(const std::string& source_id, const std::string& target_id, std::string relation_type);
    void update_entity_commit_counts(const std::unordered_map<std::string, int>& file_churn);

    void clear();
    [[nodiscard]] size_t entity_count() const noexcept { return entities_.size(); }

    bool save_to_disk(const std::string& cache_dir) const;
    bool load_from_disk(const std::string& cache_dir);

private:
    std::string repo_path_;
    std::unordered_map<std::string, Entity> entities_;
    mutable std::mutex mutex_;
};

} // namespace codeatlas
