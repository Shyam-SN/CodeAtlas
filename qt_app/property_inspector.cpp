#include "property_inspector.hpp"
#include "ai_panel.hpp"
#include <QGroupBox>
#include <QPushButton>

namespace codeatlas {

PropertyInspector::PropertyInspector(QWidget* parent) : QWidget(parent) {
    auto* main_layout = new QVBoxLayout(this);

    auto* group = new QGroupBox("Entity Inspector", this);
    auto* layout = new QVBoxLayout(group);

    label_name_ = new QLabel("<b>Name:</b> -", group);
    label_kind_ = new QLabel("<b>Kind:</b> -", group);
    label_file_ = new QLabel("<b>File:</b> -", group);
    label_lines_ = new QLabel("<b>Lines:</b> -", group);
    text_details_ = new QTextEdit(group);
    text_details_->setReadOnly(true);

    layout->addWidget(label_name_);
    layout->addWidget(label_kind_);
    layout->addWidget(label_file_);
    layout->addWidget(label_lines_);
    layout->addWidget(new QLabel("Relationships & Docstrings:"));
    layout->addWidget(text_details_);

    auto* btn_generate_doc = new QPushButton(" [ 📝 ] Generate AI Docstring", group);
    connect(btn_generate_doc, &QPushButton::clicked, this, &PropertyInspector::generate_ai_docstring);
    layout->addWidget(btn_generate_doc);

    main_layout->addWidget(group);
}

void PropertyInspector::generate_ai_docstring() {
    if (ai_panel_ && current_entity_valid_) {
        QString prompt = "Please write a professional Doxygen-style docstring comment for this component. Explain its purpose, arguments, and return values based on its implementation. Only output the docstring comment itself.";
        ai_panel_->submit_query(prompt);
    }
}

void PropertyInspector::set_entity(const Entity& entity) {
    current_entity_ = entity;
    current_entity_valid_ = true;
    
    label_name_->setText(QString("<b>Name:</b> %1").arg(QString::fromStdString(entity.name)));
    label_kind_->setText(QString("<b>Kind:</b> %1").arg(QString::fromStdString(entity_kind_to_string(entity.kind))));
    label_file_->setText(QString("<b>File:</b> %1").arg(QString::fromStdString(entity.file_path)));
    label_lines_->setText(QString("<b>Lines:</b> %1 - %2").arg(entity.start_line).arg(entity.end_line));

    QString text = QString("Docstring:\n%1\n\nRelations (%2):\n").arg(QString::fromStdString(entity.docstring)).arg(entity.relations.size());
    for (const auto& rel : entity.relations) {
        text += QString("• %1 -> %2\n").arg(QString::fromStdString(rel.type)).arg(QString::fromStdString(rel.target_id));
    }
    text_details_->setText(text);
}

void PropertyInspector::clear() {
    current_entity_valid_ = false;
    
    label_name_->setText("<b>Name:</b> -");
    label_kind_->setText("<b>Kind:</b> -");
    label_file_->setText("<b>File:</b> -");
    label_lines_->setText("<b>Lines:</b> -");
    text_details_->clear();
}

} // namespace codeatlas
