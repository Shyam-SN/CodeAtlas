# CodeAtlas 🌍

**AI-Assisted Software Repository Knowledge Graph & Visualizer**

CodeAtlas is a high-performance desktop application designed to help developers instantly understand, navigate, and document complex codebases. Written in modern C++20 and powered by Qt, CodeAtlas transforms static source code into interactive, AI-aware architectural graphs.

By combining deterministic AST (Abstract Syntax Tree) parsing with local Large Language Models (LLMs), CodeAtlas acts as a pair programmer that can not only show you how your code connects but can also explain it to you mathematically and semantically.

---

## 🚀 Key Features

### 1. Interactive Architectural Graphing
CodeAtlas parses your repository and visualizes it in multiple dimensions:
- **Repository Mind Map**: A bird's-eye view of your folders, files, and classes.
- **File Dependency Graph**: See exactly how headers and modules include one another.
- **Class Diagram**: Visualize inheritance (`Extends`) and composition (`Owns`).
- **Function Call Graph**: Track execution paths and see who calls whom.
- **Inferred Architecture Graph**: A condensed, high-level map of the major subsystems.

### 2. Deep Static Analysis
- **`libclang` AST Parsing**: CodeAtlas doesn't just use regex; it uses LLVM's `libclang` to parse C++ code exactly as a compiler would, perfectly resolving classes, methods, relations, and docstrings.
- **Cyclomatic Complexity**: Functions and methods are scored based on their branching paths (if/for/while). High-complexity nodes are drawn larger in the graph to highlight potential refactoring targets.
- **Circular Dependency Detection**: Implements Tarjan's Strongly Connected Components (SCC) algorithm to flag architectural loops.

### 3. Git Integration (Churn Analysis)
- **Heatmaps**: CodeAtlas integrates with Git to analyze file modification history. Nodes in the graph turn **Crimson Red** based on their "churn" (how frequently they are modified), immediately highlighting the most active and volatile parts of the codebase.

### 4. Advanced AI & Retrieval-Augmented Generation (RAG)
Powered entirely by local, privacy-first LLMs (via Ollama):
- **Semantic Code Chunking**: When you click a function, the AST parser extracts its exact start and end lines. The AI receives pristine, noise-free code chunks rather than truncated files.
- **Auto-Generated Docstrings**: Select an undocumented function and click `[ 📝 ] Generate AI Docstring`. The AI will read the implementation and stream back a professional Doxygen-style comment.
- **Whole-Graph RAG (Vector Database)**: Ask a codebase-wide question without selecting a node (e.g., *"How is the parser engineered?"*). CodeAtlas features a custom C++ **Vector Database** that generates high-dimensional embeddings for your entire repository, performs a cosine-similarity search, and injects the top results into the LLM context to answer global architectural questions instantly.

### 5. Professional UI/UX
- **Qt6 Desktop Interface**: Smooth, native performance.
- **Command Palette**: Press `Ctrl+P` (or `Cmd+P`) to fuzzy-search and jump to any node in the graph.
- **Minimap HUD**: A dynamic minimap in the bottom-right corner helps you navigate massive graphs.
- **High-Resolution Export**: One-click 8K PNG export of your architectural diagrams.
- **Dark & Light Themes**: Aesthetic, modern styling.

---

## 🏗️ System Architecture

The CodeAtlas architecture is heavily decoupled and purely data-driven:

1. **`ParserEngine`**: Ingests files and uses language-specific parsers (like `CppParser` using `libclang`) to extract entities.
2. **`RepositoryModel`**: The Source of Truth. It stores a normalized representation of all extracted `Entity` objects (classes, functions) and their relationships.
3. **`GraphBuilder`**: Transforms the raw `RepositoryModel` into a specialized `KnowledgeGraph` based on the requested view (e.g., Call Graph vs. Class Diagram).
4. **`AtlasCanvas`**: A custom Qt Widget that calculates physics-based layouts (Force-Directed) and renders the `KnowledgeGraph` using high-performance `QPainter`.
5. **`AIPanel` & `EmbeddingClient`**: Interfaces asynchronously via HTTP with a local Ollama server for LLM inference and Vector Embeddings.

---

## 🛠️ How to Use

### Prerequisites
- macOS (Linux/Windows supported with CMake adjustments)
- **CMake** (v3.16+)
- **LLVM / Clang** (Installed via Homebrew: `brew install llvm`)
- **Qt5 or Qt6** (Installed via Homebrew: `brew install qt`)
- **Ollama** (For AI features. Download from [ollama.com](https://ollama.com) and run `ollama run llama3.1` and `ollama pull nomic-embed-text`)

### Compiling
CodeAtlas uses CMake. A helper script `test.sh` automates the build and runs benchmarks.
```bash
# Ensure LLVM is in your path (macOS Homebrew example)
export CMAKE_PREFIX_PATH="/opt/homebrew/opt/llvm"
export PATH="/opt/homebrew/bin:$PATH"

# Build the project
./test.sh
```

### Running the Application
```bash
./build/qt_app/codeatlas_qt_app
```

### Workflow
1. **Index a Repository**: Enter the path to your codebase in the top bar and click `Index & Parse Repository`.
2. **Explore**: Use the dropdown to switch between Graph Views. Use the mouse wheel to zoom, and click-and-drag to pan.
3. **Inspect**: Click on any node. The right panel will display its physical file location, line numbers, relationships, and complexity.
4. **Ask the AI**: 
   - **Local Context**: With a node selected, type in the AI Chat Panel (e.g., *"Refactor this to be thread-safe"*).
   - **Global Context**: Deselect nodes (or start with `@repo`) and ask *"Where is the vector math handled?"* to trigger the Vector RAG search.

---

## 🔬 Technologies Used
- **C++20**: Core language.
- **Qt6**: GUI Framework, 2D Rendering, Networking.
- **libclang**: C/C++ AST Parsing.
- **GoogleTest & Google Benchmark**: Unit testing and micro-benchmarking.
- **Ollama API**: Local LLM and Embedding inference.
