#include "ai_panel.hpp"
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QFile>
#include <QTextStream>
#include <QScrollBar>
#include <QFileDialog>

namespace codeatlas {

AIPanel::AIPanel(QWidget* parent) : QWidget(parent) {
    auto* main_layout = new QVBoxLayout(this);

    auto* group = new QGroupBox("AI Repository Assistant", this);
    auto* layout = new QVBoxLayout(group);

    auto* input_layout = new QHBoxLayout();
    edit_query_ = new QLineEdit(group);
    edit_query_->setPlaceholderText("Ask a question (e.g. 'Explain parser architecture')...");

    btn_ask_ = new QPushButton("Ask AI", group);
    btn_clear_ = new QPushButton("Clear Chat", group);
    btn_podcast_ = new QPushButton("🎙️ Generate Podcast", group);
    btn_study_guide_ = new QPushButton("📚 Generate Study Guide", group);
    
    connect(btn_ask_, &QPushButton::clicked, this, &AIPanel::ask_question);
    connect(btn_clear_, &QPushButton::clicked, this, &AIPanel::clear_chat_history);
    connect(btn_podcast_, &QPushButton::clicked, this, &AIPanel::generate_podcast);
    connect(btn_study_guide_, &QPushButton::clicked, this, &AIPanel::generate_study_guide);
    connect(edit_query_, &QLineEdit::returnPressed, this, &AIPanel::ask_question);

    input_layout->addWidget(edit_query_);
    input_layout->addWidget(btn_ask_);
    input_layout->addWidget(btn_clear_);
    input_layout->addWidget(btn_podcast_);
    input_layout->addWidget(btn_study_guide_);

    text_answer_ = new QTextBrowser(group);
    text_answer_->setReadOnly(true);
    text_answer_->setOpenExternalLinks(false);
    text_answer_->setOpenLinks(false);
    connect(text_answer_, &QTextBrowser::anchorClicked, this, &AIPanel::on_link_clicked);

    layout->addLayout(input_layout);
    layout->addWidget(new QLabel("AI Response & Node Highlights:"));
    layout->addWidget(text_answer_);

    main_layout->addWidget(group);

    llm_client_ = new LocalLLMClient(this);
    connect(llm_client_, &LocalLLMClient::response_chunk_received, this, &AIPanel::on_response_chunk);
    connect(llm_client_, &LocalLLMClient::error_occurred, this, &AIPanel::on_response_error);
    connect(llm_client_, &LocalLLMClient::response_finished, this, &AIPanel::on_response_finished);

    embedder_ = new EmbeddingClient(this);
    connect(embedder_, &EmbeddingClient::embeddings_ready, this, &AIPanel::on_query_embedding_ready);
}

void AIPanel::clear_chat_history() {
    if (llm_client_) {
        llm_client_->clear_history();
    }
    current_response_.clear();
    text_answer_->clear();
    text_answer_->setMarkdown("_Chat history cleared._");
}

void AIPanel::ask_question() {
    QString q = edit_query_->text().trimmed();
    if (!q.isEmpty()) {
        submit_query(q);
    }
}

void AIPanel::submit_query(const QString& query) {
    if (query.isEmpty() || !assistant_ || !current_graph_) return;

    is_podcast_mode_ = false;
    is_study_guide_mode_ = false;

    // First do local ast lookup to highlight nodes
    AIAnswer answer = assistant_->answer_question(query.toStdString(), *current_graph_);
    emit highlight_nodes_requested(answer.highlighted_node_ids);
    
    btn_ask_->setEnabled(false);
    text_answer_->clear();
    current_response_.clear();
    
    QString prompt = "You are CodeAtlas AI. The user is asking a question about the repository.\n";
    prompt += "When referencing specific files, classes, or functions, you MUST provide an inline citation in the EXACT format: `[→ filename:L#](entity:ID)`. Use the component IDs provided in the context below.\n";
    
    // Whole-Graph RAG Mode for all Ask AI queries
    current_response_ = "_Analyzing repository architecture with Vector Search..._\n\n";
    text_answer_->setMarkdown(current_response_);
    
    edit_query_->setProperty("current_query", query);
    embedder_->generate_embeddings({query});
    // The rest of the prompt generation is handled in on_query_embedding_ready
}

void AIPanel::on_query_embedding_ready(const std::vector<std::vector<float>>& embeddings) {
    QString query = edit_query_->property("current_query").toString();
    
    if (embeddings.empty() || !vector_db_) {
        on_response_error("Failed to generate embedding for the query. Cannot perform Vector Search.");
        return;
    }

    auto results = vector_db_->search(embeddings[0], 3); // top 3
    
    QString prompt = "You are CodeAtlas AI. The user is asking a codebase-wide question.\n";
    
    if (results.empty()) {
        prompt += "The vector search returned NO relevant components. This might mean the repository is not indexed yet, or the query doesn't match anything. Tell the user that you don't have access to the codebase context to answer this question accurately, and avoid making up generic answers.\n\n";
    } else {
        prompt += "I have performed a semantic vector search and found the following Top 3 most relevant architectural components:\n\n";
        for (size_t i = 0; i < results.size(); ++i) {
            prompt += QString("--- COMPONENT %1 (ID: %2) ---\n").arg(i + 1).arg(QString::fromStdString(results[i].first));
            prompt += QString::fromStdString(results[i].second) + "\n\n";
        }
    }
    
    prompt += "User Query: " + query + "\n";
    prompt += "Synthesize a comprehensive answer using the provided components. If they do not seem relevant or if no components were provided, state that you cannot answer accurately without codebase context.\n";
    prompt += "IMPORTANT: When mentioning components, you MUST explicitly state their exact file paths and names so the user knows exactly where the code is located. Use the `[→ filename](entity:ID)` format for citations if possible.";

    current_response_.clear();
    text_answer_->clear();
    llm_client_->generate_response(prompt, "llama3.1");
}

void AIPanel::on_response_chunk(const QString& chunk) {
    current_response_ += chunk;
    text_answer_->setMarkdown(current_response_);
    
    QScrollBar *scrollbar = text_answer_->verticalScrollBar();
    scrollbar->setValue(scrollbar->maximum());
}

void AIPanel::on_response_error(const QString& error_msg) {
    if (error_msg.contains("Connection refused")) {
        current_response_ += "\n[INFO: Ollama not detected. Falling back to Mock AI System.]\n";
        
        QString node_name = "the repository";
        QString node_kind = "architecture";
        
        if (inspector_ && inspector_->current_entity()) {
            node_name = QString::fromStdString(inspector_->current_entity()->name);
            node_kind = QString::fromStdString(entity_kind_to_string(inspector_->current_entity()->kind));
        }

        QString mock_response = QString("\n### AI Analysis of %1 (%2)\n"
                                        "This is an intelligent mock response because the local Ollama API is not currently running.\n\n"
                                        "**Component Overview:**\n"
                                        "The `%3` %4 is a critical architectural component responsible for handling internal logic and bridging subsystems within CodeAtlas.\n\n"
                                        "**Key Responsibilities:**\n"
                                        "- Manages state and lifecycle of its dependent components.\n"
                                        "- Interfaces with the broader system to ensure smooth operation.\n\n"
                                        "*To use the real Llama 3.1 model, please install Ollama (https://ollama.com) and run `ollama run llama3.1` in your terminal.*")
                                        .arg(node_name)
                                        .arg(node_kind)
                                        .arg(node_name)
                                        .arg(node_kind);
                                        
        current_response_ += mock_response;
    } else {
        current_response_ += "\n[Error: " + error_msg + "]";
    }
    
    text_answer_->setMarkdown(current_response_);
    btn_ask_->setEnabled(true);
}

void AIPanel::on_response_finished() {
    if (is_study_guide_mode_) {
        current_response_ += "\n\n--- Study Guide Generated ---";
        text_answer_->setMarkdown(current_response_);
        
        QScrollBar *scrollbar = text_answer_->verticalScrollBar();
        scrollbar->setValue(scrollbar->maximum());
        btn_ask_->setEnabled(true);
        btn_podcast_->setEnabled(true);
        btn_study_guide_->setEnabled(true);

        QString file_path = QFileDialog::getSaveFileName(this, "Save Study Guide", "CodeAtlas_Study_Guide.md", "Markdown Files (*.md);;All Files (*)");
        if (!file_path.isEmpty()) {
            QFile file(file_path);
            if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
                QTextStream out(&file);
                // Strip the internal loading message
                QString content = current_response_;
                if (content.startsWith("_Generating Study Guide..._\n\n")) {
                    content = content.mid(29);
                }
                out << content;
                file.close();
            }
        }
        return;
    }

    if (is_podcast_mode_) {
        current_response_ += "\n\n--- Podcast Script Generated ---";
        text_answer_->setMarkdown(current_response_);
        
        QScrollBar *scrollbar = text_answer_->verticalScrollBar();
        scrollbar->setValue(scrollbar->maximum());
        btn_ask_->setEnabled(true);
        btn_podcast_->setEnabled(true);
        btn_study_guide_->setEnabled(true);

        // Parse script
        podcast_script_.clear();
        QStringList lines = current_response_.split('\n');
        for (const QString& line : lines) {
            QString trimmed = line.trimmed();
            if (trimmed.startsWith("Shyam:") || trimmed.startsWith("**Shyam:**")) {
                QString text = trimmed.mid(trimmed.indexOf(":") + 1).trimmed();
                podcast_script_.push_back({"Alex", text}); // Shyam voice
            } else if (trimmed.startsWith("Janaki:") || trimmed.startsWith("**Janaki:**")) {
                QString text = trimmed.mid(trimmed.indexOf(":") + 1).trimmed();
                podcast_script_.push_back({"Samantha", text}); // Janaki voice
            }
        }
        
        if (!podcast_script_.empty()) {
            current_podcast_line_ = 0;
            if (!tts_process_) {
                tts_process_ = new QProcess(this);
                connect(tts_process_, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, &AIPanel::on_tts_finished);
            }
            play_next_podcast_line();
        }
        return;
    }

    current_response_ += "\n\n--- AI response complete ---";
    text_answer_->setMarkdown(current_response_);
    
    QScrollBar *scrollbar = text_answer_->verticalScrollBar();
    scrollbar->setValue(scrollbar->maximum());
    
    btn_ask_->setEnabled(true);
}

void AIPanel::generate_podcast() {
    is_podcast_mode_ = true;
    is_study_guide_mode_ = false;
    btn_ask_->setEnabled(false);
    btn_podcast_->setEnabled(false);
    btn_study_guide_->setEnabled(false);
    text_answer_->clear();
    current_response_ = "_Generating Audio Podcast Script..._\n\n";
    text_answer_->setMarkdown(current_response_);
    
    QString prompt = "Generate a highly technical, engaging 2-person podcast script discussing the architecture of the selected component. ";
    prompt += "Use 'Shyam:' and 'Janaki:' prefixes for each line. Keep it to 4-6 lines total, focusing on specific code mechanics and design decisions.\n\n";
    
    if (inspector_ && inspector_->current_entity()) {
        const Entity* e = inspector_->current_entity();
        QString file_path = QString::fromStdString(e->file_path);
        prompt += QString("Selected Component: %1 (%2) in %3.\n").arg(QString::fromStdString(e->name)).arg(QString::fromStdString(entity_kind_to_string(e->kind))).arg(file_path);
        prompt += QString("Docstring: %1\n\n").arg(QString::fromStdString(e->docstring));
        
        QFile file(file_path);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&file);
            QString file_content;
            size_t current_line = 1;
            while (!in.atEnd() && current_line <= e->end_line) {
                QString line = in.readLine();
                if (current_line >= e->start_line) {
                    file_content += line + "\n";
                }
                current_line++;
            }
            if (!file_content.isEmpty()) {
                prompt += "--- SOURCE CODE ---\n" + file_content + "\n--- END ---\n\n";
            }
        }
    } else {
        prompt += "Focus generally on the software architecture as no specific node is selected.\n";
    }
    
    llm_client_->generate_response(prompt, "llama3.1");
}

void AIPanel::generate_study_guide() {
    is_podcast_mode_ = false;
    is_study_guide_mode_ = true;
    btn_ask_->setEnabled(false);
    btn_podcast_->setEnabled(false);
    btn_study_guide_->setEnabled(false);
    text_answer_->clear();
    current_response_ = "_Generating Study Guide..._\n\n";
    text_answer_->setMarkdown(current_response_);
    
    QString prompt = "Create a detailed technical 'Study Guide' exclusively focused on the selected component. ";
    prompt += "Provide an Architecture Overview, Core Mechanics, and Usage Examples. Keep it concise to ensure fast generation.\n\n";
    
    if (inspector_ && inspector_->current_entity()) {
        const Entity* e = inspector_->current_entity();
        QString file_path = QString::fromStdString(e->file_path);
        prompt += QString("Selected Component: %1 (%2) in %3.\n").arg(QString::fromStdString(e->name)).arg(QString::fromStdString(entity_kind_to_string(e->kind))).arg(file_path);
        prompt += QString("Docstring: %1\n\n").arg(QString::fromStdString(e->docstring));
        
        QFile file(file_path);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&file);
            QString file_content;
            size_t current_line = 1;
            while (!in.atEnd() && current_line <= e->end_line) {
                QString line = in.readLine();
                if (current_line >= e->start_line) {
                    file_content += line + "\n";
                }
                current_line++;
            }
            if (!file_content.isEmpty()) {
                prompt += "--- SOURCE CODE ---\n" + file_content + "\n--- END ---\n\n";
            }
        }
    } else {
        prompt += "Focus generally on the software architecture as no specific node is selected.\n";
    }
    llm_client_->generate_response(prompt, "llama3.1");
}

void AIPanel::play_next_podcast_line() {
    if (current_podcast_line_ >= podcast_script_.size()) {
        return; // done
    }
    const auto& line = podcast_script_[current_podcast_line_];
    tts_process_->start("say", QStringList() << "-v" << line.voice << line.text);
}

void AIPanel::on_tts_finished(int exitCode, int exitStatus) {
    (void)exitCode; (void)exitStatus;
    current_podcast_line_++;
    play_next_podcast_line();
}

void AIPanel::on_link_clicked(const QUrl& url) {
    if (url.scheme() == "entity") {
        std::string entity_id = url.path().toStdString();
        emit highlight_nodes_requested({entity_id});
    }
}

} // namespace codeatlas
