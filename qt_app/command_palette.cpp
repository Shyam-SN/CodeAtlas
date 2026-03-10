#include "command_palette.hpp"
#include <QKeyEvent>

namespace codeatlas {

CommandPalette::CommandPalette(const KnowledgeGraph* graph, QWidget* parent)
    : QDialog(parent), graph_(graph) {
    
    setWindowFlags(Qt::Popup | Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(10, 10, 10, 10);
    layout->setSpacing(5);
    
    search_box_ = new QLineEdit(this);
    search_box_->setPlaceholderText("Search files, classes, functions...");
    search_box_->setStyleSheet(
        "QLineEdit { "
        "  background-color: #2D2D30; "
        "  color: white; "
        "  border: 1px solid #3F3F46; "
        "  border-radius: 5px; "
        "  padding: 10px; "
        "  font-size: 16px; "
        "}"
    );
    
    results_list_ = new QListWidget(this);
    results_list_->setStyleSheet(
        "QListWidget { "
        "  background-color: #1E1E1E; "
        "  color: #CCCCCC; "
        "  border: 1px solid #3F3F46; "
        "  border-radius: 5px; "
        "  font-size: 14px; "
        "}"
        "QListWidget::item:selected { "
        "  background-color: #007ACC; "
        "  color: white; "
        "}"
    );
    
    layout->addWidget(search_box_);
    layout->addWidget(results_list_);
    
    // Populate all_items_ from graph
    if (graph_) {
        for (const auto& node : graph_->nodes()) {
            all_items_.push_back({
                node.id, 
                node.label, 
                node.category, 
                ""
            });
        }
    }
    
    connect(search_box_, &QLineEdit::textChanged, this, &CommandPalette::on_text_changed);
    connect(results_list_, &QListWidget::itemActivated, this, &CommandPalette::on_item_activated);
    
    setFixedSize(600, 400);
}

void CommandPalette::keyPressEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_Escape) {
        close();
    } else if (event->key() == Qt::Key_Down) {
        results_list_->setFocus();
        if (results_list_->count() > 0 && results_list_->currentRow() < 0) {
            results_list_->setCurrentRow(0);
        }
    } else if (event->key() == Qt::Key_Up && results_list_->currentRow() == 0) {
        search_box_->setFocus();
    } else if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {
        if (results_list_->currentItem()) {
            on_item_activated(results_list_->currentItem());
        }
    } else {
        QDialog::keyPressEvent(event);
    }
}

void CommandPalette::on_text_changed(const QString& text) {
    results_list_->clear();
    
    if (text.isEmpty()) return;
    
    std::string query = text.toLower().toStdString();
    
    for (const auto& item : all_items_) {
        std::string name_lower = item.name;
        std::transform(name_lower.begin(), name_lower.end(), name_lower.begin(), ::tolower);
        
        if (name_lower.find(query) != std::string::npos) {
            QString display_text = QString::fromStdString(item.name + " [" + item.kind + "]");
            auto* list_item = new QListWidgetItem(display_text, results_list_);
            list_item->setData(Qt::UserRole, QString::fromStdString(item.id));
        }
    }
    
    if (results_list_->count() > 0) {
        results_list_->setCurrentRow(0);
    }
}

void CommandPalette::on_item_activated(QListWidgetItem* item) {
    QString id = item->data(Qt::UserRole).toString();
    emit node_selected(id.toStdString());
    close();
}

} // namespace codeatlas
