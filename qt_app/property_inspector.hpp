#pragma once

#include <QWidget>
#include <QLabel>
#include <QTextEdit>
#include <QVBoxLayout>
#include <codeatlas/model/entity.hpp>

namespace codeatlas {

class AIPanel;

class PropertyInspector : public QWidget {
    Q_OBJECT
public:
    explicit PropertyInspector(QWidget* parent = nullptr);

    void set_entity(const Entity& entity);
    void clear();

    const Entity* current_entity() const { return current_entity_valid_ ? &current_entity_ : nullptr; }
    void set_ai_panel(AIPanel* panel) { ai_panel_ = panel; }

private slots:
    void generate_ai_docstring();

private:
    AIPanel* ai_panel_{nullptr};
    Entity current_entity_;
    bool current_entity_valid_{false};
    QLabel* label_name_{nullptr};
    QLabel* label_kind_{nullptr};
    QLabel* label_file_{nullptr};
    QLabel* label_lines_{nullptr};
    QTextEdit* text_details_{nullptr};
};

} // namespace codeatlas
