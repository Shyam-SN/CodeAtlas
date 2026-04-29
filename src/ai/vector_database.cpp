#include <codeatlas/ai/vector_database.hpp>
#include <cmath>
#include <algorithm>
#include <fstream>

namespace codeatlas {

void VectorDatabase::add_record(const std::string& entity_id, const std::string& text_chunk, const std::vector<float>& vector) {
    if (vector.empty()) return;
    records_.push_back({entity_id, text_chunk, vector});
}

void VectorDatabase::clear() {
    records_.clear();
}

float VectorDatabase::cosine_similarity(const std::vector<float>& v1, const std::vector<float>& v2) {
    if (v1.empty() || v2.empty() || v1.size() != v2.size()) return 0.0f;

    float dot_product = 0.0f;
    float norm_v1 = 0.0f;
    float norm_v2 = 0.0f;

    for (size_t i = 0; i < v1.size(); ++i) {
        dot_product += v1[i] * v2[i];
        norm_v1 += v1[i] * v1[i];
        norm_v2 += v2[i] * v2[i];
    }

    if (norm_v1 == 0.0f || norm_v2 == 0.0f) return 0.0f;
    return dot_product / (std::sqrt(norm_v1) * std::sqrt(norm_v2));
}

std::vector<std::pair<std::string, std::string>> VectorDatabase::search(const std::vector<float>& query_vec, int top_k) const {
    if (records_.empty() || query_vec.empty()) return {};

    struct ScoredRecord {
        float score;
        const EmbeddingRecord* record;
        
        bool operator<(const ScoredRecord& other) const {
            return score > other.score; // Descending order
        }
    };

    std::vector<ScoredRecord> scores;
    scores.reserve(records_.size());

    for (const auto& rec : records_) {
        float sim = cosine_similarity(query_vec, rec.vector);
        scores.push_back({sim, &rec});
    }

    std::sort(scores.begin(), scores.end());

    std::vector<std::pair<std::string, std::string>> results;
    int k = std::min(top_k, static_cast<int>(scores.size()));
    for (int i = 0; i < k; ++i) {
        results.push_back({scores[i].record->entity_id, scores[i].record->text_chunk});
    }

    return results;
}

bool VectorDatabase::save_to_disk(const std::string& cache_dir) const {
    std::ofstream out(cache_dir + "/embeddings.bin", std::ios::binary);
    if (!out) return false;

    size_t count = records_.size();
    out.write(reinterpret_cast<const char*>(&count), sizeof(count));

    for (const auto& rec : records_) {
        size_t id_len = rec.entity_id.size();
        out.write(reinterpret_cast<const char*>(&id_len), sizeof(id_len));
        out.write(rec.entity_id.data(), id_len);

        size_t text_len = rec.text_chunk.size();
        out.write(reinterpret_cast<const char*>(&text_len), sizeof(text_len));
        out.write(rec.text_chunk.data(), text_len);

        size_t vec_size = rec.vector.size();
        out.write(reinterpret_cast<const char*>(&vec_size), sizeof(vec_size));
        if (vec_size > 0) {
            out.write(reinterpret_cast<const char*>(rec.vector.data()), vec_size * sizeof(float));
        }
    }
    return out.good();
}

bool VectorDatabase::load_from_disk(const std::string& cache_dir) {
    std::ifstream in(cache_dir + "/embeddings.bin", std::ios::binary);
    if (!in) return false;

    clear();
    size_t count = 0;
    if (!in.read(reinterpret_cast<char*>(&count), sizeof(count))) return false;

    records_.resize(count);
    for (size_t i = 0; i < count; ++i) {
        size_t id_len = 0;
        in.read(reinterpret_cast<char*>(&id_len), sizeof(id_len));
        records_[i].entity_id.resize(id_len);
        in.read(records_[i].entity_id.data(), id_len);

        size_t text_len = 0;
        in.read(reinterpret_cast<char*>(&text_len), sizeof(text_len));
        records_[i].text_chunk.resize(text_len);
        in.read(records_[i].text_chunk.data(), text_len);

        size_t vec_size = 0;
        in.read(reinterpret_cast<char*>(&vec_size), sizeof(vec_size));
        records_[i].vector.resize(vec_size);
        if (vec_size > 0) {
            in.read(reinterpret_cast<char*>(records_[i].vector.data()), vec_size * sizeof(float));
        }
    }
    return true;
}

} // namespace codeatlas
