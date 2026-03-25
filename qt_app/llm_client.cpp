#include "llm_client.hpp"
#include <QNetworkRequest>
#include <QUrl>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonArray>
#include <QByteArray>

namespace codeatlas {

LocalLLMClient::LocalLLMClient(QObject* parent) : QObject(parent) {
    network_manager_ = new QNetworkAccessManager(this);
}

LocalLLMClient::~LocalLLMClient() {
    cancel_request();
}

void LocalLLMClient::cancel_request() {
    if (current_reply_) {
        current_reply_->abort();
        current_reply_->deleteLater();
        current_reply_ = nullptr;
    }
}

void LocalLLMClient::clear_history() {
    message_history_.clear();
}

void LocalLLMClient::generate_response(const QString& prompt, const QString& model) {
    cancel_request();

    message_history_.push_back({"user", prompt});
    current_assistant_message_.clear();

    QUrl url("http://localhost:11434/api/chat");
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonArray messages_array;
    for (const auto& msg : message_history_) {
        QJsonObject msg_obj;
        msg_obj["role"] = msg.role;
        msg_obj["content"] = msg.content;
        messages_array.append(msg_obj);
    }

    QJsonObject json;
    json["model"] = model;
    json["messages"] = messages_array;
    json["stream"] = true;

    QByteArray data = QJsonDocument(json).toJson();

    current_reply_ = network_manager_->post(request, data);

    connect(current_reply_, &QNetworkReply::readyRead, this, &LocalLLMClient::on_ready_read);
    connect(current_reply_, &QNetworkReply::finished, this, &LocalLLMClient::on_finished);
}

void LocalLLMClient::on_ready_read() {
    if (!current_reply_) return;

    QByteArray data = current_reply_->readAll();
    
    // Data might contain multiple JSON lines if streamed quickly
    QList<QByteArray> lines = data.split('\n');
    for (const auto& line : lines) {
        if (line.trimmed().isEmpty()) continue;
        
        QJsonParseError parseError;
        QJsonDocument doc = QJsonDocument::fromJson(line, &parseError);
        
        if (parseError.error == QJsonParseError::NoError && doc.isObject()) {
            QJsonObject obj = doc.object();
            if (obj.contains("message")) {
                QJsonObject msg = obj["message"].toObject();
                if (msg.contains("content")) {
                    QString chunk = msg["content"].toString();
                    current_assistant_message_ += chunk;
                    emit response_chunk_received(chunk);
                }
            }
        }
    }
}

void LocalLLMClient::on_finished() {
    if (current_reply_) {
        if (current_reply_->error() != QNetworkReply::NoError && current_reply_->error() != QNetworkReply::OperationCanceledError) {
            emit error_occurred("Ollama API Error: " + current_reply_->errorString() + "\nPlease ensure Ollama is running (http://localhost:11434).");
        }
        current_reply_->deleteLater();
        current_reply_ = nullptr;
    }
    
    if (!current_assistant_message_.isEmpty()) {
        message_history_.push_back({"assistant", current_assistant_message_});
    }

    emit response_finished();
}

} // namespace codeatlas
