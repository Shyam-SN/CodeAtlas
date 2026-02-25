#include "atlas_canvas.hpp"
#include <codeatlas/layout/hierarchical_layout.hpp>
#include <QPainter>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QPainterPath>
#include <cmath>
#include <algorithm>

namespace codeatlas {

AtlasCanvas::AtlasCanvas(QWidget* parent) : QWidget(parent) {
    setMinimumSize(600, 400);
    setMouseTracking(true);
}

void AtlasCanvas::set_graph(KnowledgeGraph graph) {
    graph_ = std::move(graph);
    reset_view();
}

void AtlasCanvas::highlight_nodes(const std::vector<std::string>& node_ids) {
    graph_.highlight_path(node_ids);
    update();
}

void AtlasCanvas::zoom_in() {
    zoom_factor_ = std::min(5.0, zoom_factor_ * 1.25);
    update();
}

void AtlasCanvas::zoom_out() {
    zoom_factor_ = std::max(0.2, zoom_factor_ * 0.80);
    update();
}

void AtlasCanvas::reset_view() {
    zoom_factor_ = 1.0;
    pan_offset_ = QPointF(0.0, 0.0);
    update();
}

void AtlasCanvas::draw_graph(QPainter& painter) {
    QColor bg_color = is_dark_mode_ ? QColor(22, 26, 32) : QColor(245, 245, 245);
    QColor grid_color = is_dark_mode_ ? QColor(35, 42, 54) : QColor(220, 220, 220);
    QColor edge_color = is_dark_mode_ ? QColor(100, 115, 140, 180) : QColor(150, 150, 150, 180);
    QColor text_color = is_dark_mode_ ? QColor(235, 242, 250) : QColor(30, 30, 30);

    // 0. Draw Background & Grid
    painter.fillRect(painter.window(), bg_color);
    painter.setPen(QPen(grid_color, 1, Qt::DotLine));
    int grid_step = 40;
    for (int x = 0; x < painter.window().width(); x += grid_step) painter.drawLine(x, 0, x, painter.window().height());
    for (int y = 0; y < painter.window().height(); y += grid_step) painter.drawLine(0, y, painter.window().width(), y);

    const auto& nodes = graph_.nodes();
    const auto& edges = graph_.edges();
    size_t N = nodes.size();

    std::unordered_map<std::string, QPointF> pos_map;
    for (const auto& node : nodes) {
        pos_map[node.id] = QPointF(node.x, node.y);
    }

    double base_radius = std::max(7.0, std::min(22.0, 320.0 / std::sqrt(N + 1)));

    // 1. Draw Mind Map Curved Bezier Branches
    for (const auto& edge : edges) {
        if (pos_map.count(edge.source_id) && pos_map.count(edge.target_id)) {
            // Find nodes to check visibility
            bool source_visible = false;
            bool target_visible = false;
            for(const auto& n : nodes) {
                if(n.id == edge.source_id) source_visible = n.visible;
                if(n.id == edge.target_id) target_visible = n.visible;
            }
            if(!source_visible || !target_visible) continue;

            QPointF p1 = pos_map[edge.source_id];
            QPointF p2 = pos_map[edge.target_id];

            QPainterPath path(p1);
            QPointF ctrl1 = p1 + (p2 - p1) * 0.5 + QPointF(0, (p2.y() - p1.y()) * 0.2);
            path.cubicTo(ctrl1, ctrl1, p2);

            if (edge.is_circular) {
                painter.setPen(QPen(QColor(255, 20, 20), 4.0, Qt::SolidLine)); // Bright Red
            } else if (edge.highlighted) {
                painter.setPen(QPen(QColor(255, 64, 129), 3.0, Qt::SolidLine));
            } else {
                painter.setPen(QPen(edge_color, 1.5));
            }
            painter.drawPath(path);
        }
    }

    // 2. Find max commit count for heatmap
    int max_commits = 1;
    for (const auto& node : nodes) {
        if (node.commit_count > max_commits) max_commits = node.commit_count;
    }

    // 3. Draw Nodes
    for (const auto& node : nodes) {
        if (!node.visible) continue;

        QPointF p(node.x, node.y);
        double r = base_radius;
        
        int alpha = node.is_dead_code ? 76 : 255;
        QColor fill_color(67, 160, 71, alpha); // Green
        QColor border_color(255, 255, 255, node.is_dead_code ? 100 : 200);

        if (node.category == "Root") {
            r = base_radius * 1.8;
            fill_color = QColor(63, 81, 181, alpha); // Indigo
            border_color = QColor(255, 215, 0, alpha); // Gold
        } else if (node.category == "Folder") {
            r = base_radius * 1.4;
            fill_color = QColor(92, 107, 192, alpha); // Slate
        } else {
            // Scale radius based on cyclomatic complexity
            if (node.category == "Function" || node.category == "Method") {
                // logarithmic scaling, base radius for complexity=1, increasing visibly.
                r = base_radius + std::log2(static_cast<double>(std::max(1, node.complexity))) * 5.0;
            }
            
            // Heatmap coloring for Files, Classes, Functions based on churn
            double ratio = std::min(1.0, static_cast<double>(node.commit_count) / max_commits);
            if (node.commit_count == 0) {
                // Cold (Blue)
                fill_color = QColor(2, 136, 209, alpha);
            } else if (ratio < 0.5) {
                // Warm (Yellow/Orange)
                int r_col = 200 + 55 * (ratio * 2.0);
                int g_col = 200 - 50 * (ratio * 2.0);
                fill_color = QColor(r_col, g_col, 50, alpha);
            } else {
                // Hot (Red)
                int r_col = 255;
                int g_col = 150 - 150 * ((ratio - 0.5) * 2.0);
                fill_color = QColor(r_col, g_col, 50, alpha);
            }
        }

        if (node.is_circular) {
            border_color = QColor(255, 20, 20, 255); // Bright Red
        } else if (node.is_dead_code) {
            border_color = QColor(100, 100, 100, 150); // Dark Gray
        }

        if (node.highlighted) {
            fill_color = QColor(225, 24, 60); // Crimson
            r = base_radius * 1.5;

            painter.setBrush(Qt::NoBrush);
            painter.setPen(QPen(QColor(255, 64, 129, 140), 4));
            painter.drawEllipse(p, r + 5, r + 5);
        }

        if (node.id == selected_node_id_) {
            border_color = QColor(255, 255, 0);
            r *= 1.3;
        }

        painter.setBrush(fill_color);
        painter.setPen(QPen(border_color, 2.0));
        painter.drawEllipse(p, r, r);

        // Draw +/- indicator if it can be expanded/collapsed (e.g. Folder or File or Root)
        if (node.category == "Root" || node.category == "Folder" || node.category == "File") {
            painter.setPen(QColor(255, 255, 255));
            QFont ind_font = painter.font();
            ind_font.setPointSize(std::max(6, static_cast<int>(r / 2)));
            ind_font.setBold(true);
            painter.setFont(ind_font);
            QString indicator = node.collapsed ? "+" : "-";
            painter.drawText(QRectF(p.x() - r, p.y() - r, r*2, r*2), Qt::AlignCenter, indicator);
        }

        // Label
        QColor node_text_color = text_color;
        node_text_color.setAlpha(alpha);
        painter.setPen(node_text_color);
        QFont font = painter.font();
        font.setPointSize(std::max(8, std::min(10, static_cast<int>(base_radius))));
        font.setBold(false);
        painter.setFont(font);

        QString label_text = QString::fromStdString(node.label);
        if (N > 50 && label_text.length() > 14 && !node.highlighted && node.id != selected_node_id_) {
            label_text = label_text.left(12) + "..";
        }

        painter.drawText(QRectF(p.x() - 65, p.y() + r + 3, 130, 20), Qt::AlignCenter, label_text);
    }
}

void AtlasCanvas::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    painter.save();
    painter.translate(width() / 2.0 + pan_offset_.x(), height() / 2.0 + pan_offset_.y());
    painter.scale(zoom_factor_, zoom_factor_);
    painter.translate(-width() / 2.0, -height() / 2.0);

    draw_graph(painter);

    painter.restore();

    // 3. Draw On-Screen Zoom & View HUD
    QColor hud_color = is_dark_mode_ ? QColor(180, 195, 215) : QColor(60, 60, 60);
    painter.setPen(hud_color);
    painter.setFont(QFont("Helvetica", 10, QFont::Bold));
    
    int visible_count = 0;
    const auto& nodes = graph_.nodes();
    for(const auto& n : nodes) if(n.visible) visible_count++;

    QString hud = QString("Zoom: %1% | Visible Nodes: %2 / %3").arg(static_cast<int>(zoom_factor_ * 100)).arg(visible_count).arg(nodes.size());
    painter.drawText(15, 25, hud);

    // 4. Draw Minimap (HUD in bottom right)
    if (nodes.size() > 0) {
        double minimap_w = 200.0;
        double minimap_h = 150.0;
        double margin = 20.0;
        QRectF minimap_rect(width() - minimap_w - margin, height() - minimap_h - margin, minimap_w, minimap_h);
        
        QColor minimap_bg = is_dark_mode_ ? QColor(0, 0, 0, 150) : QColor(255, 255, 255, 180);
        painter.fillRect(minimap_rect, minimap_bg);
        painter.setPen(QPen(is_dark_mode_ ? QColor(100, 100, 100) : QColor(200, 200, 200), 1));
        painter.drawRect(minimap_rect);

        double base_radius = std::max(7.0, std::min(22.0, 320.0 / std::sqrt(nodes.size() + 1)));

        // Calculate graph bounds
        double min_x = std::numeric_limits<double>::max();
        double max_x = std::numeric_limits<double>::lowest();
        double min_y = std::numeric_limits<double>::max();
        double max_y = std::numeric_limits<double>::lowest();
        
        for (const auto& n : nodes) {
            if (n.x < min_x) min_x = n.x;
            if (n.x > max_x) max_x = n.x;
            if (n.y < min_y) min_y = n.y;
            if (n.y > max_y) max_y = n.y;
        }
        
        double graph_w = std::max(1.0, max_x - min_x + base_radius * 4);
        double graph_h = std::max(1.0, max_y - min_y + base_radius * 4);
        
        double scale_x = minimap_w / graph_w;
        double scale_y = minimap_h / graph_h;
        double scale = std::min(scale_x, scale_y) * 0.9;
        
        double center_x = (min_x + max_x) / 2.0;
        double center_y = (min_y + max_y) / 2.0;

        // Draw nodes in minimap
        painter.setPen(Qt::NoPen);
        for (const auto& n : nodes) {
            if (!n.visible) continue;
            double mx = minimap_rect.x() + minimap_w / 2.0 + (n.x - center_x) * scale;
            double my = minimap_rect.y() + minimap_h / 2.0 + (n.y - center_y) * scale;
            
            QColor dot_color = is_dark_mode_ ? QColor(200, 200, 200) : QColor(100, 100, 100);
            if (n.highlighted) dot_color = QColor(255, 64, 129);
            else if (n.category == "Root") dot_color = QColor(63, 81, 181);
            else if (n.category == "File") dot_color = QColor(2, 136, 209);
            
            painter.setBrush(dot_color);
            painter.drawEllipse(QPointF(mx, my), 1.5, 1.5);
        }

        // Draw Viewport Bounding Box
        double view_w = width() / zoom_factor_;
        double view_h = height() / zoom_factor_;
        double view_center_x = -pan_offset_.x() / zoom_factor_;
        double view_center_y = -pan_offset_.y() / zoom_factor_;
        
        double vx = minimap_rect.x() + minimap_w / 2.0 + (view_center_x - view_w/2.0 - center_x) * scale;
        double vy = minimap_rect.y() + minimap_h / 2.0 + (view_center_y - view_h/2.0 - center_y) * scale;
        double vw = view_w * scale;
        double vh = view_h * scale;
        
        painter.setBrush(QColor(100, 150, 255, 40));
        painter.setPen(QPen(QColor(100, 150, 255, 200), 1));
        painter.drawRect(QRectF(vx, vy, vw, vh).intersected(minimap_rect));
    }
}

void AtlasCanvas::export_to_image(const QString& file_path) {
    const auto& nodes = graph_.nodes();
    if (nodes.empty()) return;

    double min_x = std::numeric_limits<double>::max();
    double max_x = std::numeric_limits<double>::lowest();
    double min_y = std::numeric_limits<double>::max();
    double max_y = std::numeric_limits<double>::lowest();

    double base_radius = std::max(7.0, std::min(22.0, 320.0 / std::sqrt(nodes.size() + 1)));
    for (const auto& n : nodes) {
        if (!n.visible) continue;
        if (n.x < min_x) min_x = n.x;
        if (n.x > max_x) max_x = n.x;
        if (n.y < min_y) min_y = n.y;
        if (n.y > max_y) max_y = n.y;
    }

    double margin = base_radius * 10.0;
    double graph_w = max_x - min_x + margin * 2;
    double graph_h = max_y - min_y + margin * 2;

    int img_w = std::min(8192, static_cast<int>(graph_w));
    int img_h = std::min(8192, static_cast<int>(graph_h));

    double scale = std::min(static_cast<double>(img_w) / graph_w, static_cast<double>(img_h) / graph_h);

    QImage img(img_w, img_h, QImage::Format_ARGB32);
    img.fill(is_dark_mode_ ? QColor(22, 26, 32) : QColor(245, 245, 245));

    QPainter painter(&img);
    painter.setRenderHint(QPainter::Antialiasing);

    // Provide a window rect for grid drawing
    painter.setWindow(0, 0, img_w, img_h);

    painter.translate(img_w / 2.0, img_h / 2.0);
    painter.scale(scale, scale);
    painter.translate(-(min_x + max_x) / 2.0, -(min_y + max_y) / 2.0);
    
    // Adjust window rect so grid draws properly over the bounds
    painter.setWindow(min_x - margin, min_y - margin, graph_w, graph_h);

    draw_graph(painter);

    img.save(file_path);
}

void AtlasCanvas::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::RightButton || event->button() == Qt::MiddleButton) {
        is_panning_ = true;
        last_mouse_pos_ = event->position();
    } else if (event->button() == Qt::LeftButton) {
        QPointF scene_pos = (event->position() - QPointF(width() / 2.0, height() / 2.0) - pan_offset_) / zoom_factor_ + QPointF(width() / 2.0, height() / 2.0);

        const auto& nodes = graph_.nodes();
        double base_radius = std::max(7.0, std::min(22.0, 320.0 / std::sqrt(nodes.size() + 1)));

        for (const auto& node : nodes) {
            if (!node.visible) continue;
            double dx = scene_pos.x() - node.x;
            double dy = scene_pos.y() - node.y;
            if (std::sqrt(dx * dx + dy * dy) <= base_radius * 1.5) {
                selected_node_id_ = node.id;
                emit node_selected(node.entity_id);
                update();
                break;
            }
        }
    }
}

void AtlasCanvas::mouseDoubleClickEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        QPointF scene_pos = (event->position() - QPointF(width() / 2.0, height() / 2.0) - pan_offset_) / zoom_factor_ + QPointF(width() / 2.0, height() / 2.0);

        const auto& nodes = graph_.nodes();
        double base_radius = std::max(7.0, std::min(22.0, 320.0 / std::sqrt(nodes.size() + 1)));

        for (const auto& node : nodes) {
            if (!node.visible) continue;
            double dx = scene_pos.x() - node.x;
            double dy = scene_pos.y() - node.y;
            if (std::sqrt(dx * dx + dy * dy) <= base_radius * 1.5) {
                if (graph_.toggle_node_collapse(node.id)) {
                    // Recalculate layout
                    if (graph_.type() == GraphType::MindMap || graph_.type() == GraphType::ArchitectureGraph) {
                        HierarchicalLayout::apply_radial_mindmap(graph_, 1400.0, 1000.0);
                    }
                    
                    // Center pan on the node
                    auto updated_nodes = graph_.nodes();
                    for(const auto& un : updated_nodes) {
                        if(un.id == node.id) {
                            pan_offset_ = QPointF(width() / 2.0 - un.x, height() / 2.0 - un.y) * zoom_factor_;
                            break;
                        }
                    }
                    update();
                }
                break;
            }
        }
    }
}

void AtlasCanvas::mouseMoveEvent(QMouseEvent* event) {
    if (is_panning_) {
        QPointF delta = event->position() - last_mouse_pos_;
        pan_offset_ += delta;
        last_mouse_pos_ = event->position();
        update();
    }
}

void AtlasCanvas::mouseReleaseEvent(QMouseEvent* event) {
    if (event->button() == Qt::RightButton || event->button() == Qt::MiddleButton) {
        is_panning_ = false;
    }
}

void AtlasCanvas::wheelEvent(QWheelEvent* event) {
    if (event->angleDelta().y() > 0) {
        zoom_in();
    } else {
        zoom_out();
    }
}

void AtlasCanvas::set_dark_mode(bool dark) {
    is_dark_mode_ = dark;
    update();
}

} // namespace codeatlas
