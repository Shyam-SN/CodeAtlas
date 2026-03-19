#pragma once

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QString>
#include <QStringList>
#include <vector>
#include <functional>

namespace codeatlas {

class EmbeddingClient : public QObject {
    Q_OBJECT
public:
    explicit EmbeddingClient(QObject* parent = nullptr);
    ~EmbeddingClient() override;

    // Generates embeddings for a list of texts using Ollama's /api/embed
    void generate_embeddings(const QStringList& texts, const QString& model = "nomic-embed-text");

    void cancel_request();

signals:
    // Emitted when the embeddings are ready
    void embeddings_ready(const std::vector<std::vector<float>>& embeddings);
    void progress_updated(int current, int total);
    void error_occurred(const QString& error_msg);

private slots:
    void on_finished();

private:
    void process_next_batch();
    void try_start_batches();

    QNetworkAccessManager* network_manager_{nullptr};
    std::vector<QNetworkReply*> active_replies_;
    
    QStringList pending_texts_;
    QString current_model_;
    std::vector<std::vector<float>> accumulated_embeddings_;
    int total_texts_{0};
    int processed_texts_{0};
    const int batch_size_{10};
    const int max_concurrent_requests_{3};
};

} // namespace codeatlas
