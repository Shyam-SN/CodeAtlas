#pragma once

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QString>

#include <vector>

namespace codeatlas {

struct ChatMessage {
    QString role;
    QString content;
};

class LocalLLMClient : public QObject {
    Q_OBJECT
public:
    explicit LocalLLMClient(QObject* parent = nullptr);
    ~LocalLLMClient() override;

    void generate_response(const QString& prompt, const QString& model = "llama3.1");
    void cancel_request();
    void clear_history();

signals:
    void response_chunk_received(const QString& chunk);
    void response_finished();
    void error_occurred(const QString& error_msg);

private slots:
    void on_ready_read();
    void on_finished();

private:
    QNetworkAccessManager* network_manager_{nullptr};
    QNetworkReply* current_reply_{nullptr};
    std::vector<ChatMessage> message_history_;
    QString current_assistant_message_;
};

} // namespace codeatlas
