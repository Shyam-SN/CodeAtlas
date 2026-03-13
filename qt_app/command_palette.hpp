#pragma once

#include <QDialog>
#include <QLineEdit>
#include <QListWidget>
#include <QVBoxLayout>
#include "codeatlas/graph/knowledge_graph.hpp"

namespace codeatlas {

class CommandPalette : public QDialog {
    Q_OBJECT
public:
    explicit CommandPalette(const KnowledgeGraph* graph, QWidget* parent = nullptr);

signals:
    void node_selected(const std::string& node_id);

protected:
    void keyPressEvent(QKeyEvent* event) override;

private slots:
    void on_text_changed(const QString& text);
    void on_item_activated(QListWidgetItem* item);

private:
    const KnowledgeGraph* graph_;
    QLineEdit* search_box_;
    QListWidget* results_list_;
    
    struct ResultItem {
        std::string id;
        std::string name;
        std::string kind;
        std::string file;
    };
    std::vector<ResultItem> all_items_;
};

} // namespace codeatlas
