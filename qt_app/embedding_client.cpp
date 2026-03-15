#include "embedding_client.hpp"
#include <QNetworkRequest>
#include <QUrl>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QByteArray>

namespace codeatlas {

EmbeddingClient::EmbeddingClient(QObject* parent) : QObject(parent) {
    network_manager_ = new QNetworkAccessManager(this);
}

EmbeddingClient::~EmbeddingClient() {
    cancel_request();
}

void EmbeddingClient::cancel_request() {
    for (auto* reply : active_replies_) {
        reply->abort();
        reply->deleteLater();
    }
    active_replies_.clear();
}

void EmbeddingClient::generate_embeddings(const QStringList& texts, const QString& model) {
    cancel_request();
    pending_texts_ = texts;
    current_model_ = model;
    total_texts_ = texts.size();
    processed_texts_ = 0;
    accumulated_embeddings_.clear();
    accumulated_embeddings_.resize(total_texts_);

    if (pending_texts_.isEmpty()) {
        emit progress_updated(0, 0);
        emit embeddings_ready({});
        return;
    }

    emit progress_updated(0, total_texts_);
    try_start_batches();
}

void EmbeddingClient::try_start_batches() {
    while (active_replies_.size() < static_cast<size_t>(max_concurrent_requests_) && !pending_texts_.isEmpty()) {
        int current_batch_size = std::min(batch_size_, static_cast<int>(pending_texts_.size()));
        QStringList batch;
        for (int i = 0; i < current_batch_size; ++i) {
            batch.append(pending_texts_.takeFirst());
        }

        QUrl url("http://localhost:11434/api/embed");
        QNetworkRequest request(url);
        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
        request.setTransferTimeout(300000); // 5 minutes timeout for local LLMs

        QJsonObject json;
        json["model"] = current_model_;
        
        QJsonArray input_array;
        for (const QString& text : batch) {
            input_array.append(text);
        }
        json["input"] = input_array;

        QByteArray data = QJsonDocument(json).toJson();
        
        // Calculate the starting index of this batch in the final array
        int start_index = total_texts_ - pending_texts_.size() - current_batch_size;

        QNetworkReply* reply = network_manager_->post(request, data);
        active_replies_.push_back(reply);
        
        connect(reply, &QNetworkReply::finished, this, [this, reply, start_index, current_batch_size]() {
            auto it = std::find(active_replies_.begin(), active_replies_.end(), reply);
            if (it != active_replies_.end()) {
                active_replies_.erase(it);
            }

            if (reply->error() != QNetworkReply::NoError) {
                if (reply->error() != QNetworkReply::OperationCanceledError) {
                    QByteArray err_data = reply->readAll();
                    QString err_msg = reply->errorString();
                    if (!err_data.isEmpty()) {
                        QJsonParseError parseError;
                        QJsonDocument err_doc = QJsonDocument::fromJson(err_data, &parseError);
                        if (parseError.error == QJsonParseError::NoError && err_doc.isObject()) {
                            QJsonObject err_obj = err_doc.object();
                            if (err_obj.contains("error")) {
                                err_msg = err_obj["error"].toString();
                            }
                        }
                    }
                    emit error_occurred("Ollama API Error: " + err_msg);
                }
            } else {
                QByteArray data = reply->readAll();
                QJsonParseError parseError;
                QJsonDocument doc = QJsonDocument::fromJson(data, &parseError);
                
                if (parseError.error != QJsonParseError::NoError || !doc.isObject()) {
                    emit error_occurred("Failed to parse Ollama response");
                } else {
                    QJsonObject obj = doc.object();
                    if (!obj.contains("embeddings") || !obj["embeddings"].isArray()) {
                        emit error_occurred("Invalid response format from Ollama (missing embeddings array)");
                    } else {
                        QJsonArray embeddings_arr = obj["embeddings"].toArray();
                        int offset = 0;
                        for (const QJsonValue& embed_val : embeddings_arr) {
                            if (!embed_val.isArray()) continue;
                            QJsonArray vec_arr = embed_val.toArray();
                            std::vector<float> vec;
                            vec.reserve(vec_arr.size());
                            for (const QJsonValue& v : vec_arr) {
                                vec.push_back(static_cast<float>(v.toDouble()));
                            }
                            if (start_index + offset < static_cast<int>(accumulated_embeddings_.size())) {
                                accumulated_embeddings_[start_index + offset] = std::move(vec);
                            }
                            offset++;
                        }
                    }
                }
            }

            reply->deleteLater();

            processed_texts_ += current_batch_size;
            emit progress_updated(processed_texts_, total_texts_);

            if (pending_texts_.isEmpty() && active_replies_.empty()) {
                emit embeddings_ready(accumulated_embeddings_);
            } else {
                try_start_batches();
            }
        });
    }
}

void EmbeddingClient::on_finished() {
    // Unused now due to lambda, but left for header compatibility
}

} // namespace codeatlas
