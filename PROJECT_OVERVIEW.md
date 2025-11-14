# CodeAtlas: AI-Assisted Software Repository Knowledge Graph

## 1. Introduction & Vision
**CodeAtlas** is a flagship, high-performance desktop application built in modern C++20 and Qt 6. It is designed to solve the problem of navigating, understanding, and visualizing large codebases. Instead of reading code linearly, CodeAtlas indexes an entire repository, builds a multi-relational Knowledge Graph of its architecture, and renders it beautifully on an interactive, hardware-accelerated 2D canvas.

Beyond visualization, CodeAtlas integrates a deeply context-aware **Local AI Agent** (powered by Ollama) that acts as an expert pair-programmer. By clicking on any visual node in the graph (a class, function, or file), the application automatically injects the node's metadata and raw source code into a local LLM to answer complex architectural queries securely and completely offline.

---

## 2. Technology Stack
- **Core Language**: C++20 (utilizing advanced features, concepts, smart pointers, standard library algorithms).
- **GUI Framework**: Qt 6 (Widgets module for the main layout, `QPainter` for custom canvas rendering, `QNetworkAccessManager` for asynchronous REST API calls).
- **Build System**: CMake (with FetchContent for dependency management).
- **Testing & Benchmarking**: GoogleTest (unit testing) and Google Benchmark (performance profiling).
- **AI Backend**: Ollama Local REST API (`http://localhost:11434`) running open-weights models like `llama3.1` or `gemma2`.

---

## 3. Core Subsystems & Architecture

### 3.1. Parsing Engine (`src/parser/`)
The Parsing Engine is responsible for walking the directory tree, reading source files, and extracting Abstract Syntax Tree (AST) representations of the code.
- **Multi-Language Support**: It currently supports **C++** (`cpp_parser.cpp`), **Python** (`python_parser.cpp`), and **CMake** (`cmake_parser.cpp`).
- **Entity Extraction**: It extracts logical entities such as `Files`, `Classes`, `Structs`, and `Functions`.
- **Relationship Extraction**: It infers dependencies, function calls, and ownership models (e.g., extracting function calls inside Python/C++ to build a Call Graph).

### 3.2. Knowledge Graph & Model (`src/graph/`, `src/model/`)
Once entities are extracted, they are mapped into a standardized `RepositoryModel`. 
- **GraphBuilder**: Translates the raw `RepositoryModel` into a `KnowledgeGraph` consisting of `GraphNode` and `GraphEdge` structs.
- **Graph Types**: 
  - *Repository Mind Map* (Hierarchical folder-file-symbol layout)
  - *File Dependency Graph* (Includes relationships between files)
  - *Class Diagram* (Inheritance and composition)
  - *Function Call Graph* (Execution flow tracking)
  - *Inferred Architecture Graph* (High-level subsystem boundaries)

### 3.3. Mathematical Layout Engines (`src/layout/`)
To render the graph nicely without overlap, custom math-based layout engines calculate `(x, y)` coordinates for every node.
- **Hierarchical Layout**: Generates radial, symmetric tree layouts (perfect for Mind Maps).
- **Force Directed Layout**: A physics simulation where nodes repel each other (like magnets) and edges pull them together (like springs), settling into a naturally organized cluster.

### 3.4. Qt Desktop Application (`qt_app/`)
The frontend is a fully custom desktop interface.
- **`AtlasCanvas`**: A custom `QWidget` that overrides `paintEvent`. It uses `QPainter` to draw antialiased nodes, glowing highlight rings, and curved Bezier paths for edges.
- **Interactivity**: Supports infinite panning, mouse-wheel zooming, and dynamic re-rendering. 
- **Expand/Collapse Logic**: Double-clicking a folder/file node expands or collapses its children dynamically to prevent the Mind Map from becoming congested.
- **`PropertyInspector`**: A right-docked panel that displays live metadata (Line numbers, Docstrings, File paths) for whatever node the user currently has selected.

### 3.5. Local AI Integration (`src/ai/`, `qt_app/llm_client.cpp`)
The flagship feature of the application.
- **`AIPanel`**: A chat interface built with a `QTextEdit` that supports rich-text Markdown rendering (`setMarkdown()`).
- **Context-Aware Injection**: When a user selects a node (e.g., `cpp_parser.cpp`) and asks a question, CodeAtlas opens the physical file on the hard drive, reads the raw source code, grabs the relations, and injects it all into a hidden system prompt.
- **`LocalLLMClient`**: Uses Qt's asynchronous Network classes to stream the query to a local Ollama server, piping the chunks directly back to the UI seamlessly without freezing the main thread.

---

## 4. Strengths & Differentiators
1. **100% Local & Private**: No code is sent to OpenAI or cloud servers. The entire AST parsing, layout math, and LLM inference run strictly on the user's hardware.
2. **Visual-First AI**: Traditional IDEs require users to know *what file* they are looking for. CodeAtlas lets users visually explore unknown codebases top-down and interrogate subsystems via AI.
3. **C++ Performance**: Generating force-directed layouts for thousands of nodes occurs in milliseconds, validated by built-in Google Benchmarks.

## 5. Potential Future Ideas for AI Analysis
*Use these prompts with an AI to get ideas for the project:*
- "Based on the CodeAtlas architecture, what would be the best way to implement a fuzzy-search 'Command Palette' to jump to specific nodes?"
- "How could I upgrade the `cpp_parser.cpp` to use `libclang` for perfect AST extraction instead of regex?"
- "What are some advanced AI RAG (Retrieval-Augmented Generation) techniques to allow the AI to answer questions about the *entire* graph at once, rather than just the selected node?"
- "How can we implement semantic chunking in the AI Panel if a selected file exceeds the 128k token context window limit?"
