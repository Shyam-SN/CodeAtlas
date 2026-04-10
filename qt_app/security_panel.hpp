#pragma once

#include <QDockWidget>
#include <QTableWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <codeatlas/ai/security_scanner.hpp>
#include <codeatlas/model/repository_model.hpp>

namespace codeatlas {

class SecurityPanel : public QDockWidget {
    Q_OBJECT
public:
    explicit SecurityPanel(QWidget* parent = nullptr);

    void set_model(const RepositoryModel* model);

signals:
    void highlight_node_requested(const std::string& entity_id);

public slots:
    void run_scan();

private slots:
    void on_item_double_clicked(int row, int column);

private:
    const RepositoryModel* model_{nullptr};
    QTableWidget* table_;
    QPushButton* btn_scan_;
    std::vector<Vulnerability> current_vulnerabilities_;
};

} // namespace codeatlas
