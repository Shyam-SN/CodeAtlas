#pragma once

#include <string>
#include <vector>
#include <utility>

namespace codeatlas {

struct EmbeddingRecord {
    std::string entity_id;
    std::string text_chunk;
    std::vector<float> vector;
};

class VectorDatabase {
public:
    void add_record(const std::string& entity_id, const std::string& text_chunk, const std::vector<float>& vector);
    
    // Returns pair of (entity_id, text_chunk)
    std::vector<std::pair<std::string, std::string>> search(const std::vector<float>& query_vec, int top_k = 3) const;
    
    void clear();
    size_t size() const { return records_.size(); }

    bool save_to_disk(const std::string& cache_dir) const;
    bool load_from_disk(const std::string& cache_dir);

private:
    std::vector<EmbeddingRecord> records_;
    
    static float cosine_similarity(const std::vector<float>& v1, const std::vector<float>& v2);
};

} // namespace codeatlas
