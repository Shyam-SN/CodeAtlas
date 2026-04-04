#include "security_panel.hpp"
#include <QHeaderView>
#include <QFileInfo>
#include <QColor>

namespace codeatlas {

SecurityPanel::SecurityPanel(QWidget* parent) : QDockWidget("Security Scanner", parent) {
    QWidget* content = new QWidget(this);
    auto* layout = new QVBoxLayout(content);

    btn_scan_ = new QPushButton("Run Security Scan", this);
    connect(btn_scan_, &QPushButton::clicked, this, &SecurityPanel::run_scan);

    table_ = new QTableWidget(this);
    table_->setColumnCount(4);
    table_->setHorizontalHeaderLabels({"Severity", "Type", "Component", "Description"});
    table_->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    table_->setSelectionBehavior(QAbstractItemView::SelectRows);
    table_->setEditTriggers(QAbstractItemView::NoEditTriggers);

    connect(table_, &QTableWidget::cellDoubleClicked, this, &SecurityPanel::on_item_double_clicked);

    layout->addWidget(btn_scan_);
    layout->addWidget(table_);

    setWidget(content);
}

void SecurityPanel::set_model(const RepositoryModel* model) {
    model_ = model;
}

void SecurityPanel::run_scan() {
    if (!model_) return;

    current_vulnerabilities_ = SecurityScanner::scan(*model_);

    table_->setRowCount(current_vulnerabilities_.size());
    
    for (size_t i = 0; i < current_vulnerabilities_.size(); ++i) {
        const auto& vuln = current_vulnerabilities_[i];
        
        QTableWidgetItem* sev_item = new QTableWidgetItem(QString::fromStdString(severity_to_string(vuln.severity)));
        if (vuln.severity == Severity::Critical) sev_item->setForeground(Qt::red);
        else if (vuln.severity == Severity::High) sev_item->setForeground(QColor(255, 165, 0)); // Orange
        else if (vuln.severity == Severity::Medium) sev_item->setForeground(Qt::yellow);
        
        QTableWidgetItem* type_item = new QTableWidgetItem(QString::fromStdString(vuln.type));
        
        QString comp_text = QString::fromStdString(vuln.entity_name) + " (" + QFileInfo(QString::fromStdString(vuln.file_path)).fileName() + ":" + QString::number(vuln.line_number) + ")";
        QTableWidgetItem* comp_item = new QTableWidgetItem(comp_text);
        
        QTableWidgetItem* desc_item = new QTableWidgetItem(QString::fromStdString(vuln.description));
        
        table_->setItem(i, 0, sev_item);
        table_->setItem(i, 1, type_item);
        table_->setItem(i, 2, comp_item);
        table_->setItem(i, 3, desc_item);
    }
}

void SecurityPanel::on_item_double_clicked(int row, int column) {
    (void)column;
    if (row >= 0 && row < static_cast<int>(current_vulnerabilities_.size())) {
        emit highlight_node_requested(current_vulnerabilities_[row].entity_id);
    }
}

} // namespace codeatlas
