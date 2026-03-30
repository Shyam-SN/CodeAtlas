#pragma once

#include <QMainWindow>
#include <QTabWidget>
#include <QComboBox>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QSplitter>
#include <codeatlas/codeatlas.hpp>
#include "atlas_canvas.hpp"
#include "property_inspector.hpp"
#include "ai_panel.hpp"
#include "security_panel.hpp"
#include "command_palette.hpp"
#include "embedding_client.hpp"
#include <codeatlas/ai/vector_database.hpp>
#include <QShortcut>
#include <QProcess>
#include <QFileSystemWatcher>
#include <QTimer>

namespace codeatlas {

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);

private slots:
    void open_repository_folder(bool force_reindex = false);
    void change_graph_view(int index);
    void handle_node_selection(const std::string& entity_id);
    void show_command_palette();
    void toggle_theme();
    void export_graph_image();
    void on_git_clone_finished(int exitCode, int exitStatus);
    void on_file_changed(const QString& path);
    void trigger_reindex();

private:
    RepositoryModel model_;
    AIAssistant assistant_{model_};
    KnowledgeGraph current_graph_{GraphType::MindMap};

    AtlasCanvas* canvas_{nullptr};
    PropertyInspector* inspector_{nullptr};
    AIPanel* ai_panel_{nullptr};
    QDockWidget* security_dock_{nullptr};
    SecurityPanel* security_panel_{nullptr};
    EmbeddingClient* embedder_{nullptr};
    VectorDatabase vector_db_;

    QComboBox* combo_graph_type_{nullptr};
    QLineEdit* edit_repo_path_{nullptr};
    QPushButton* btn_index_{nullptr};
    QPushButton* btn_zoom_in_{nullptr};
    QPushButton* btn_zoom_out_{nullptr};
    QPushButton* btn_reset_view_{nullptr};
    QLabel* status_label_{nullptr};
    QProcess* git_process_{nullptr};
    QFileSystemWatcher* file_watcher_{nullptr};
    QTimer* debounce_timer_{nullptr};

    void setup_ui();
    void reload_graph();
    void generate_repository_embeddings(const std::string& cache_dir);
    
    bool is_indexing_{false};
    class QProgressBar* index_progress_bar_{nullptr};
};

} // namespace codeatlas
