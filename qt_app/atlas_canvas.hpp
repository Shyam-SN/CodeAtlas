#pragma once

#include <QWidget>
#include <QPointF>
#include <codeatlas/graph/knowledge_graph.hpp>

namespace codeatlas {

class AtlasCanvas : public QWidget {
    Q_OBJECT
public:
    explicit AtlasCanvas(QWidget* parent = nullptr);

    void set_graph(KnowledgeGraph graph);
    const KnowledgeGraph& graph() const noexcept { return graph_; }

    void highlight_nodes(const std::vector<std::string>& node_ids);

public slots:
    void zoom_in();
    void zoom_out();
    void reset_view();
    void set_dark_mode(bool dark);
    void export_to_image(const QString& file_path);

signals:
    void node_selected(const std::string& entity_id);

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;

private:
    void draw_graph(QPainter& painter);

    KnowledgeGraph graph_{GraphType::MindMap};
    std::string selected_node_id_;

    double zoom_factor_{1.0};
    QPointF pan_offset_{0.0, 0.0};
    QPointF last_mouse_pos_;
    bool is_panning_{false};
    bool is_dark_mode_{true};
};

} // namespace codeatlas
