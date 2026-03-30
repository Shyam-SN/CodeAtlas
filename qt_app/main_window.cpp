#include <QProgressBar>
#include <fstream>
#include "main_window.hpp"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QGroupBox>
#include <QMenuBar>
#include <QShortcut>
#include <QStatusBar>
#include <QDirIterator>

namespace codeatlas {

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    setWindowTitle("CodeAtlas: AI-Assisted Software Repository Knowledge Graph & Visualizer");
    resize(1250, 850);
    setup_ui();

    // Default load current directory
    edit_repo_path_->setText("/Users/prathima/Desktop/Shyam/CodeAtlas");
    open_repository_folder();
}

void MainWindow::setup_ui() {
    auto* central_widget = new QWidget(this);
    setCentralWidget(central_widget);

    auto* main_layout = new QVBoxLayout(central_widget);

    // Top Toolbar Panel
    auto* top_panel = new QHBoxLayout();
    edit_repo_path_ = new QLineEdit(central_widget);
    auto* btn_browse = new QPushButton("Browse Repository...", central_widget);
    btn_index_ = new QPushButton("Index & Parse Repository", central_widget);

    combo_graph_type_ = new QComboBox(central_widget);
    combo_graph_type_->addItem("1. Repository Mind Map", static_cast<int>(GraphType::MindMap));
    combo_graph_type_->addItem("2. File Dependency Graph", static_cast<int>(GraphType::FileDependency));
    combo_graph_type_->addItem("3. Class Diagram", static_cast<int>(GraphType::ClassDiagram));
    combo_graph_type_->addItem("4. Function Call Graph", static_cast<int>(GraphType::CallGraph));
    combo_graph_type_->addItem("5. Inferred Architecture Graph", static_cast<int>(GraphType::ArchitectureGraph));

    // Zoom Controls
    btn_zoom_in_ = new QPushButton(" [ + ] Zoom In ", central_widget);
    btn_zoom_out_ = new QPushButton(" [ - ] Zoom Out ", central_widget);
    btn_reset_view_ = new QPushButton(" [ ⟲ ] Reset View ", central_widget);
    
    // Theme & Export
    auto* btn_theme = new QPushButton(" [ 🌗 ] Toggle Theme ", central_widget);
    connect(btn_theme, &QPushButton::clicked, this, &MainWindow::toggle_theme);

    auto* btn_export = new QPushButton(" [ 📸 ] Export Image ", central_widget);
    connect(btn_export, &QPushButton::clicked, this, &MainWindow::export_graph_image);

    connect(btn_browse, &QPushButton::clicked, [this]() {
        QString dir = QFileDialog::getExistingDirectory(this, "Select Repository Directory");
        if (!dir.isEmpty()) {
            edit_repo_path_->setText(dir);
            open_repository_folder();
        }
    });

    connect(btn_index_, &QPushButton::clicked, [this]() {
        open_repository_folder(true);
    });
    connect(combo_graph_type_, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::change_graph_view);

    top_panel->addWidget(new QLabel("Repository Path:", central_widget));
    top_panel->addWidget(edit_repo_path_);
    top_panel->addWidget(btn_browse);
    top_panel->addWidget(btn_index_);
    top_panel->addSpacing(15);
    top_panel->addWidget(new QLabel("Graph View:", central_widget));
    top_panel->addWidget(combo_graph_type_);
    top_panel->addSpacing(15);
    top_panel->addWidget(btn_zoom_in_);
    top_panel->addWidget(btn_zoom_out_);
    top_panel->addWidget(btn_reset_view_);
    top_panel->addWidget(btn_theme);
    top_panel->addWidget(btn_export);

    main_layout->addLayout(top_panel);

    // Main Splitter Layout
    auto* splitter = new QSplitter(Qt::Horizontal, central_widget);

    // Canvas Widget
    canvas_ = new AtlasCanvas(central_widget);
    connect(canvas_, &AtlasCanvas::node_selected, this, &MainWindow::handle_node_selection);
    connect(btn_zoom_in_, &QPushButton::clicked, canvas_, &AtlasCanvas::zoom_in);
    connect(btn_zoom_out_, &QPushButton::clicked, canvas_, &AtlasCanvas::zoom_out);
    connect(btn_reset_view_, &QPushButton::clicked, canvas_, &AtlasCanvas::reset_view);

    // Right Dock Panel (Property Inspector + AI Panel)
    auto* right_panel = new QWidget(central_widget);
    auto* right_layout = new QVBoxLayout(right_panel);

    inspector_ = new PropertyInspector(right_panel);
    ai_panel_ = new AIPanel(right_panel);
    ai_panel_->set_assistant(&assistant_);
    ai_panel_->set_inspector(inspector_);
    inspector_->set_ai_panel(ai_panel_);

    connect(ai_panel_, &AIPanel::highlight_nodes_requested, canvas_, &AtlasCanvas::highlight_nodes);

    right_layout->addWidget(inspector_);
    right_layout->addWidget(ai_panel_);

    splitter->addWidget(canvas_);
    splitter->addWidget(right_panel);
    splitter->setStretchFactor(0, 3);
    splitter->setStretchFactor(1, 2);

    main_layout->addWidget(splitter);

    auto* shortcut = new QShortcut(QKeySequence("Ctrl+P"), this);
    connect(shortcut, &QShortcut::activated, this, &MainWindow::show_command_palette);

    security_panel_ = new SecurityPanel(this);
    security_panel_->set_model(&model_);
    addDockWidget(Qt::BottomDockWidgetArea, security_panel_);
    connect(security_panel_, &SecurityPanel::highlight_node_requested, this, [this](const std::string& entity_id) {
        if (canvas_) {
            canvas_->highlight_nodes({entity_id});
            if (inspector_) {
                auto entity_opt = model_.get_entity(entity_id);
                if (entity_opt) {
                    inspector_->set_entity(*entity_opt);
                }
            }
        }
    });

    auto* view_menu = menuBar()->addMenu("&View");
    QAction* toggle_security = security_panel_->toggleViewAction();
    view_menu->addAction(toggle_security);


    embedder_ = new EmbeddingClient(this);
    ai_panel_->set_vector_db(&vector_db_);
    
    connect(embedder_, &EmbeddingClient::embeddings_ready, [this](const std::vector<std::vector<float>>& embeddings) {
        if (embeddings.empty()) {
            statusBar()->showMessage("Embedding generation failed or empty.");
            if (index_progress_bar_) {
                index_progress_bar_->hide();
            }
            is_indexing_ = false;
            return;
        }

        auto entities_map = model_.all_entities();
        int count = 0;
        auto it = entities_map.begin();
        for (size_t i = 0; i < embeddings.size() && it != entities_map.end(); ++i, ++it) {
            const Entity& entity = it->second;
            QString chunk = QString("Name: %1\nKind: %2\nFile: %3\nDocstring: %4")
                .arg(QString::fromStdString(entity.name))
                .arg(QString::fromStdString(entity_kind_to_string(entity.kind)))
                .arg(QString::fromStdString(entity.file_path))
                .arg(QString::fromStdString(entity.docstring));
            vector_db_.add_record(entity.id, chunk.toStdString(), embeddings[i]);
            count++;
        }
        
        ai_panel_->set_vector_db(&vector_db_);
        security_panel_->set_model(&model_);
        
        std::string cache_dir = model_.repository_path() + "/.codeatlas";
        vector_db_.save_to_disk(cache_dir);
        model_.save_to_disk(cache_dir);
        
        statusBar()->showMessage(QString("Successfully generated AI Vector Index for %1 entities (Saved to Cache).").arg(count));
        if (index_progress_bar_) {
            index_progress_bar_->hide();
        }
        is_indexing_ = false;
    });

    index_progress_bar_ = new QProgressBar(this);
    index_progress_bar_->setMinimum(0);
    index_progress_bar_->setMaximum(100);
    index_progress_bar_->hide();
    statusBar()->addPermanentWidget(index_progress_bar_);

    connect(embedder_, &EmbeddingClient::progress_updated, [this](int current, int total) {
        if (total > 0) {
            if (index_progress_bar_->isHidden()) {
                index_progress_bar_->show();
            }
            index_progress_bar_->setMaximum(total);
            index_progress_bar_->setValue(current);
            statusBar()->showMessage(QString("Generating AI Vector Index: %1 / %2 chunks...").arg(current).arg(total));
        }
    });

    connect(embedder_, &EmbeddingClient::error_occurred, [](const QString& err) {
        qDebug() << "EmbeddingClient Error:" << err;
    });

    file_watcher_ = new QFileSystemWatcher(this);
    debounce_timer_ = new QTimer(this);
    debounce_timer_->setSingleShot(true);
    debounce_timer_->setInterval(1000); // 1 second debounce
    
    connect(file_watcher_, &QFileSystemWatcher::directoryChanged, this, &MainWindow::on_file_changed);
    connect(file_watcher_, &QFileSystemWatcher::fileChanged, this, &MainWindow::on_file_changed);
    connect(debounce_timer_, &QTimer::timeout, this, &MainWindow::trigger_reindex);

    statusBar()->showMessage("CodeAtlas Ready. Use [ + ] Zoom In and [ - ] Zoom Out buttons or mouse scroll wheel to zoom.");
}

void MainWindow::generate_repository_embeddings(const std::string& cache_dir) {
    (void)cache_dir; // Handled in lambda
    vector_db_.clear();
    statusBar()->showMessage("Generating AI Vector Index for repository... Please wait.");
    
    std::unordered_map<std::string, std::vector<std::string>> file_cache;
    auto get_lines = [&file_cache](const std::string& path) -> const std::vector<std::string>& {
        if (file_cache.find(path) == file_cache.end()) {
            std::vector<std::string> lines;
            std::ifstream file(path);
            if (file.is_open()) {
                std::string line;
                while (std::getline(file, line)) {
                    lines.push_back(line);
                }
            }
            file_cache[path] = std::move(lines);
        }
        return file_cache[path];
    };
    
    QStringList texts;
    for (const auto& pair : model_.all_entities()) {
        const auto& entity = pair.second;
        
        std::string source_code;
        if (!entity.file_path.empty() && entity.start_line > 0 && entity.end_line >= entity.start_line) {
            const auto& lines = get_lines(entity.file_path);
            // Lines are 1-indexed
            size_t start_idx = entity.start_line - 1;
            size_t end_idx = entity.end_line - 1;
            if (start_idx < lines.size() && end_idx < lines.size()) {
                for (size_t i = start_idx; i <= end_idx; ++i) {
                    source_code += lines[i] + "\n";
                }
            }
        }
        
        QString chunk = QString("Name: %1\nKind: %2\nFile: %3\nDocstring: %4\nSource Code:\n%5")
            .arg(QString::fromStdString(entity.name))
            .arg(QString::fromStdString(entity_kind_to_string(entity.kind)))
            .arg(QString::fromStdString(entity.file_path))
            .arg(QString::fromStdString(entity.docstring))
            .arg(QString::fromStdString(source_code));
            
        texts.append(chunk);
    }
    embedder_->generate_embeddings(texts);
}

void MainWindow::open_repository_folder(bool force_reindex) {
    std::string path = edit_repo_path_->text().toStdString();
    if (path.empty()) return;
    
    is_indexing_ = true;
    
    // Check if path is a Git URL
    if (path.find("http://") == 0 || path.find("https://") == 0 || path.find("git@") == 0) {
        std::string repo_name = "repo";
        size_t last_slash = path.find_last_of('/');
        if (last_slash != std::string::npos && last_slash < path.length() - 1) {
            repo_name = path.substr(last_slash + 1);
            if (repo_name.ends_with(".git")) {
                repo_name = repo_name.substr(0, repo_name.length() - 4);
            }
        }
        
        std::string target_dir = "/tmp/codeatlas/" + repo_name;
        
        if (!force_reindex && std::filesystem::exists(target_dir)) {
            // Already cloned, just switch to it
            edit_repo_path_->setText(QString::fromStdString(target_dir));
            // Recurse to load it locally
            open_repository_folder(false);
            return;
        }
        
        if (std::filesystem::exists(target_dir)) {
            std::filesystem::remove_all(target_dir);
        }
        
        statusBar()->showMessage("Cloning repository...");
        if (btn_index_) btn_index_->setEnabled(false);
        edit_repo_path_->setEnabled(false);
        
        if (!git_process_) {
            git_process_ = new QProcess(this);
            connect(git_process_, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, &MainWindow::on_git_clone_finished);
        }
        
        git_process_->setProperty("target_dir", QString::fromStdString(target_dir));
        git_process_->start("git", QStringList() << "clone" << "--depth" << "1" << QString::fromStdString(path) << QString::fromStdString(target_dir));
        return;
    }
    
    std::string cache_dir = path + "/.codeatlas";
    
    if (!force_reindex) {
        if (std::filesystem::exists(cache_dir + "/index.bin") && std::filesystem::exists(cache_dir + "/embeddings.bin")) {
            statusBar()->showMessage("Loading repository from cache...");
            if (model_.load_from_disk(cache_dir) && vector_db_.load_from_disk(cache_dir)) {
                statusBar()->showMessage(QString("Loaded %1 entities from cache.").arg(model_.entity_count()));
                reload_graph();
                is_indexing_ = false;
                return;
            }
        }
    }
    
    std::filesystem::create_directories(cache_dir);

    model_.clear();
    ParserEngine::parse_repository(path, model_);
    RepositoryIndexer::build_index(model_);
    RepositoryIndexer::calculate_git_churn(path, model_);

    statusBar()->showMessage(QString("Indexed %1 entities across repository.").arg(model_.entity_count()));
    reload_graph();
    
    generate_repository_embeddings(cache_dir);
    
    // Setup file watcher
    if (!file_watcher_->directories().isEmpty()) {
        file_watcher_->removePaths(file_watcher_->directories());
    }
    QStringList dirs;
    QDirIterator it(QString::fromStdString(path), QDir::Dirs | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);
    dirs << QString::fromStdString(path);
    while (it.hasNext()) {
        QString d = it.next();
        if (!d.contains(".git") && !d.contains("build") && !d.contains(".codeatlas")) {
            dirs << d;
        }
    }
    if (!dirs.isEmpty()) {
        file_watcher_->addPaths(dirs);
    }
}

void MainWindow::change_graph_view(int index) {
    (void)index;
    reload_graph();
}

void MainWindow::reload_graph() {
    GraphType type = static_cast<GraphType>(combo_graph_type_->currentData().toInt());
    current_graph_ = GraphBuilder::build_graph(model_, type);

    canvas_->set_graph(current_graph_);
    ai_panel_->set_current_graph(&current_graph_);
}

void MainWindow::handle_node_selection(const std::string& entity_id) {
    auto entity = model_.get_entity(entity_id);
    if (entity) {
        inspector_->set_entity(*entity);
    } else {
        inspector_->clear();
    }
}

void MainWindow::show_command_palette() {
    auto* palette = new CommandPalette(&current_graph_, this);
    connect(palette, &CommandPalette::node_selected, [this](const std::string& id) {
        handle_node_selection(id);
    });
    
    // Calculate center position
    QRect parent_rect = geometry();
    palette->move(parent_rect.center() - palette->rect().center());
    
    palette->exec();
}

void MainWindow::toggle_theme() {
    static bool is_dark_mode = true;
    is_dark_mode = !is_dark_mode;

    if (is_dark_mode) {
        setStyleSheet("");
    } else {
        setStyleSheet(
            "QMainWindow, QWidget, QDialog { background-color: #F5F5F5; color: #2D2D30; }"
            "QPushButton { background-color: #E0E0E0; border: 1px solid #BDBDBD; padding: 5px; border-radius: 3px; color: #2D2D30; }"
            "QPushButton:hover { background-color: #D6D6D6; }"
            "QLineEdit, QTextEdit, QListWidget, QComboBox { background-color: #FFFFFF; color: #2D2D30; border: 1px solid #BDBDBD; }"
            "QLabel { color: #2D2D30; }"
            "QGroupBox { border: 1px solid #BDBDBD; margin-top: 1ex; }"
            "QGroupBox::title { subcontrol-origin: margin; subcontrol-position: top center; padding: 0 3px; }"
        );
    }
    
    if (canvas_) {
        canvas_->set_dark_mode(is_dark_mode);
    }
}

void MainWindow::export_graph_image() {
    if (!canvas_) return;

    QString file_path = QFileDialog::getSaveFileName(
        this,
        "Export Graph as Image",
        "",
        "PNG Image (*.png);;All Files (*)"
    );

    if (!file_path.isEmpty()) {
        if (!file_path.endsWith(".png", Qt::CaseInsensitive)) {
            file_path += ".png";
        }
        canvas_->export_to_image(file_path);
        statusBar()->showMessage(QString("Graph exported successfully to %1").arg(file_path));
    }
}

void MainWindow::on_git_clone_finished(int exitCode, int exitStatus) {
    if (btn_index_) btn_index_->setEnabled(true);
    edit_repo_path_->setEnabled(true);
    
    if (exitStatus == QProcess::CrashExit || exitCode != 0) {
        statusBar()->showMessage("Failed to clone repository. Check URL and git installation.");
        return;
    }
    
    QString target_dir = git_process_->property("target_dir").toString();
    edit_repo_path_->setText(target_dir);
    statusBar()->showMessage("Repository cloned successfully. Indexing...");
    
    open_repository_folder(true);
}

void MainWindow::on_file_changed(const QString& path) {
    (void)path;
    if (is_indexing_) return;
    debounce_timer_->start();
    statusBar()->showMessage("File change detected... preparing to hot reload.");
}

void MainWindow::trigger_reindex() {
    statusBar()->showMessage("Hot reloading repository index...");
    open_repository_folder(true); // force reindex
}

} // namespace codeatlas
