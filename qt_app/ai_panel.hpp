#pragma once

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QTextBrowser>
#include <QVBoxLayout>
#include <codeatlas/ai/ai_assistant.hpp>
#include "llm_client.hpp"
#include "property_inspector.hpp"
#include "embedding_client.hpp"
#include <codeatlas/ai/vector_database.hpp>
#include <QProcess>

namespace codeatlas {

struct PodcastLine {
    QString voice;
    QString text;
};

class AIPanel : public QWidget {
    Q_OBJECT
public:
    explicit AIPanel(QWidget* parent = nullptr);

    void set_assistant(const AIAssistant* assistant) { assistant_ = assistant; }
    void set_current_graph(const KnowledgeGraph* graph) { current_graph_ = graph; }
    void set_inspector(const PropertyInspector* inspector) { inspector_ = inspector; }
    void set_vector_db(const VectorDatabase* db) { vector_db_ = db; }

    void submit_query(const QString& query);
    void clear_chat_history();

signals:
    void highlight_nodes_requested(const std::vector<std::string>& node_ids);

private slots:
    void ask_question();
    void on_response_chunk(const QString& chunk);
    void on_response_error(const QString& error_msg);
    void on_response_finished();
    void on_query_embedding_ready(const std::vector<std::vector<float>>& embeddings);
    void on_link_clicked(const QUrl& url);
    void generate_podcast();
    void generate_study_guide();
    void play_next_podcast_line();
    void on_tts_finished(int exitCode, int exitStatus);

private:
    const AIAssistant* assistant_{nullptr};
    const KnowledgeGraph* current_graph_{nullptr};
    const PropertyInspector* inspector_{nullptr};
    const VectorDatabase* vector_db_{nullptr};
    LocalLLMClient* llm_client_{nullptr};
    EmbeddingClient* embedder_{nullptr};
    QString current_response_;

    QLineEdit* edit_query_{nullptr};
    QPushButton* btn_ask_{nullptr};
    QPushButton* btn_clear_{nullptr};
    QPushButton* btn_podcast_{nullptr};
    QPushButton* btn_study_guide_{nullptr};
    QTextBrowser* text_answer_{nullptr};

    bool is_podcast_mode_{false};
    bool is_study_guide_mode_{false};
    std::vector<PodcastLine> podcast_script_;
    size_t current_podcast_line_{0};
    QProcess* tts_process_{nullptr};
};

} // namespace codeatlas
