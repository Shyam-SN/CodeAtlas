#include <codeatlas/model/repository_model.hpp>
#include <algorithm>
#include <fstream>

namespace codeatlas {

namespace {
    void write_str(std::ofstream& out, const std::string& str) {
        size_t len = str.size();
        out.write(reinterpret_cast<const char*>(&len), sizeof(len));
        if (len > 0) out.write(str.data(), len);
    }
    
    std::string read_str(std::ifstream& in) {
        size_t len = 0;
        in.read(reinterpret_cast<char*>(&len), sizeof(len));
        std::string str(len, '\0');
        if (len > 0) in.read(str.data(), len);
        return str;
    }
}

void RepositoryModel::add_entity(Entity entity) {
    std::lock_guard<std::mutex> lock(mutex_);
    entities_[entity.id] = std::move(entity);
}

std::optional<Entity> RepositoryModel::get_entity(const std::string& id) const {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = entities_.find(id);
    if (it != entities_.end()) return it->second;
    return std::nullopt;
}

std::vector<Entity> RepositoryModel::get_entities_by_kind(EntityKind kind) const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<Entity> result;
    for (const auto& [id, entity] : entities_) {
        if (entity.kind == kind) {
            result.push_back(entity);
        }
    }
    return result;
}

std::vector<Entity> RepositoryModel::search_entities(const std::string& query) const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<Entity> result;
    std::string lower_query = query;
    std::transform(lower_query.begin(), lower_query.end(), lower_query.begin(), ::tolower);

    for (const auto& [id, entity] : entities_) {
        std::string name = entity.name;
        std::transform(name.begin(), name.end(), name.begin(), ::tolower);

        std::string doc = entity.docstring;
        std::transform(doc.begin(), doc.end(), doc.begin(), ::tolower);

        if (name.find(lower_query) != std::string::npos || doc.find(lower_query) != std::string::npos) {
            result.push_back(entity);
        }
    }
    return result;
}

void RepositoryModel::add_relation(const std::string& source_id, const std::string& target_id, std::string relation_type) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = entities_.find(source_id);
    if (it != entities_.end()) {
        it->second.relations.push_back({std::move(relation_type), target_id});
    }
}

void RepositoryModel::update_entity_commit_counts(const std::unordered_map<std::string, int>& file_churn) {
    std::lock_guard<std::mutex> lock(mutex_);
    for (auto& [id, entity] : entities_) {
        auto it = file_churn.find(entity.file_path);
        if (it != file_churn.end()) {
            entity.commit_count = it->second;
        } else {
            entity.commit_count = 0;
        }
    }
}

void RepositoryModel::clear() {
    std::lock_guard<std::mutex> lock(mutex_);
    entities_.clear();
    repo_path_.clear();
}

bool RepositoryModel::save_to_disk(const std::string& cache_dir) const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::ofstream out(cache_dir + "/index.bin", std::ios::binary);
    if (!out) return false;

    write_str(out, repo_path_);
    size_t count = entities_.size();
    out.write(reinterpret_cast<const char*>(&count), sizeof(count));

    for (const auto& [id, e] : entities_) {
        write_str(out, e.id);
        write_str(out, e.name);
        out.write(reinterpret_cast<const char*>(&e.kind), sizeof(e.kind));
        write_str(out, e.file_path);
        out.write(reinterpret_cast<const char*>(&e.start_line), sizeof(e.start_line));
        out.write(reinterpret_cast<const char*>(&e.end_line), sizeof(e.end_line));
        write_str(out, e.docstring);
        write_str(out, e.parent_id);

        size_t rel_count = e.relations.size();
        out.write(reinterpret_cast<const char*>(&rel_count), sizeof(rel_count));
        for (const auto& rel : e.relations) {
            write_str(out, rel.type);
            write_str(out, rel.target_id);
        }

        size_t tag_count = e.tags.size();
        out.write(reinterpret_cast<const char*>(&tag_count), sizeof(tag_count));
        for (const auto& t : e.tags) {
            write_str(out, t);
        }

        size_t meta_count = e.metadata.size();
        out.write(reinterpret_cast<const char*>(&meta_count), sizeof(meta_count));
        for (const auto& [k, v] : e.metadata) {
            write_str(out, k);
            write_str(out, v);
        }

        out.write(reinterpret_cast<const char*>(&e.commit_count), sizeof(e.commit_count));
        out.write(reinterpret_cast<const char*>(&e.complexity), sizeof(e.complexity));
    }
    return out.good();
}

bool RepositoryModel::load_from_disk(const std::string& cache_dir) {
    std::lock_guard<std::mutex> lock(mutex_);
    std::ifstream in(cache_dir + "/index.bin", std::ios::binary);
    if (!in) return false;

    entities_.clear();
    repo_path_ = read_str(in);

    size_t count = 0;
    if (!in.read(reinterpret_cast<char*>(&count), sizeof(count))) return false;

    for (size_t i = 0; i < count; ++i) {
        Entity e;
        e.id = read_str(in);
        e.name = read_str(in);
        in.read(reinterpret_cast<char*>(&e.kind), sizeof(e.kind));
        e.file_path = read_str(in);
        in.read(reinterpret_cast<char*>(&e.start_line), sizeof(e.start_line));
        in.read(reinterpret_cast<char*>(&e.end_line), sizeof(e.end_line));
        e.docstring = read_str(in);
        e.parent_id = read_str(in);

        size_t rel_count = 0;
        in.read(reinterpret_cast<char*>(&rel_count), sizeof(rel_count));
        for (size_t j = 0; j < rel_count; ++j) {
            Relation rel;
            rel.type = read_str(in);
            rel.target_id = read_str(in);
            e.relations.push_back(std::move(rel));
        }

        size_t tag_count = 0;
        in.read(reinterpret_cast<char*>(&tag_count), sizeof(tag_count));
        for (size_t j = 0; j < tag_count; ++j) {
            e.tags.push_back(read_str(in));
        }

        size_t meta_count = 0;
        in.read(reinterpret_cast<char*>(&meta_count), sizeof(meta_count));
        for (size_t j = 0; j < meta_count; ++j) {
            std::string k = read_str(in);
            std::string v = read_str(in);
            e.metadata[k] = v;
        }

        in.read(reinterpret_cast<char*>(&e.commit_count), sizeof(e.commit_count));
        in.read(reinterpret_cast<char*>(&e.complexity), sizeof(e.complexity));

        entities_[e.id] = std::move(e);
    }
    return true;
}

} // namespace codeatlas
