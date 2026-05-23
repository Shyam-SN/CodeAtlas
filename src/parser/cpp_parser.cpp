#include <codeatlas/parser/cpp_parser.hpp>
#include <clang-c/Index.h>
#include <filesystem>
#include <string>
#include <unordered_set>
#include <iostream>

namespace codeatlas {

struct ParserState {
    RepositoryModel* model;
    std::string file_id;
    std::string file_path;
    std::string current_parent_id;
};

static std::string to_string(CXString cx_str) {
    std::string str;
    if (const char* s = clang_getCString(cx_str)) {
        str = s;
    }
    clang_disposeString(cx_str);
    return str;
}

static CXChildVisitResult complexity_visitor(CXCursor cursor, CXCursor parent, CXClientData client_data) {
    (void)parent;
    int* complexity = static_cast<int*>(client_data);
    CXCursorKind kind = clang_getCursorKind(cursor);
    
    if (kind == CXCursor_IfStmt || 
        kind == CXCursor_ForStmt || 
        kind == CXCursor_WhileStmt || 
        kind == CXCursor_DoStmt || 
        kind == CXCursor_CaseStmt || 
        kind == CXCursor_CXXCatchStmt || 
        kind == CXCursor_ConditionalOperator) {
        (*complexity)++;
    }
    return CXChildVisit_Recurse;
}

static CXChildVisitResult visitor(CXCursor cursor, CXCursor parent, CXClientData client_data) {
    (void)parent;
    ParserState* state = static_cast<ParserState*>(client_data);
    CXCursorKind kind = clang_getCursorKind(cursor);

    CXSourceLocation loc = clang_getCursorLocation(cursor);
    if (clang_Location_isInSystemHeader(loc)) {
        return CXChildVisit_Continue;
    }

    CXFile file;
    unsigned line, column, offset;
    clang_getSpellingLocation(loc, &file, &line, &column, &offset);
    
    std::string cursor_file = to_string(clang_getFileName(file));
    if (cursor_file.empty() || cursor_file != state->file_path) {
        // Only process cursors in the target file, but continue to visit children
        return CXChildVisit_Recurse; 
    }

    std::string name = to_string(clang_getCursorSpelling(cursor));

    std::string docstring = to_string(clang_Cursor_getRawCommentText(cursor));

    CXSourceRange range = clang_getCursorExtent(cursor);
    CXSourceLocation start = clang_getRangeStart(range);
    CXSourceLocation end = clang_getRangeEnd(range);
    unsigned start_line, end_line;
    clang_getSpellingLocation(start, nullptr, &start_line, nullptr, nullptr);
    clang_getSpellingLocation(end, nullptr, &end_line, nullptr, nullptr);

    if (kind == CXCursor_ClassDecl || kind == CXCursor_StructDecl || kind == CXCursor_CXXMethod || kind == CXCursor_FunctionDecl) {
        if (name.empty()) {
            return CXChildVisit_Recurse;
        }
        
        Entity entity;
        entity.name = name;
        entity.file_path = state->file_path;
        entity.start_line = start_line;
        entity.end_line = end_line;
        entity.docstring = docstring;
        entity.parent_id = state->current_parent_id;

        if (kind == CXCursor_ClassDecl) {
            entity.kind = EntityKind::Class;
            entity.id = "class:" + state->file_path + ":" + name;
        } else if (kind == CXCursor_StructDecl) {
            entity.kind = EntityKind::Struct;
            entity.id = "struct:" + state->file_path + ":" + name;
        } else {
            entity.kind = EntityKind::Function;
            entity.id = "func:" + state->file_path + ":" + name;
            
            // Calculate Cyclomatic Complexity
            int func_complexity = 1;
            clang_visitChildren(cursor, complexity_visitor, &func_complexity);
            entity.complexity = func_complexity;
        }

        state->model->add_entity(entity);
        state->model->add_relation(state->current_parent_id, entity.id, "contains");

        // Traverse children to maintain nested scope
        std::string previous_parent = state->current_parent_id;
        state->current_parent_id = entity.id;
        clang_visitChildren(cursor, visitor, state);
        state->current_parent_id = previous_parent;

        return CXChildVisit_Continue; // We manually recursed
    } 
    else if (kind == CXCursor_CallExpr) {
        if (!state->current_parent_id.empty() && !name.empty()) { 
            static const std::unordered_set<std::string> keywords = {
                "if", "while", "for", "switch", "catch", "return", "sizeof", "typeid", "static_cast",
                "dynamic_cast", "reinterpret_cast", "const_cast", "decltype", "alignof"
            };
            if (!keywords.count(name)) {
                state->model->add_relation(state->current_parent_id, name, "calls");
            }
        }
        return CXChildVisit_Recurse;
    } 
    else if (kind == CXCursor_InclusionDirective) {
        if (!name.empty()) {
            state->model->add_relation(state->file_id, "header:" + name, "includes");
        }
        return CXChildVisit_Continue;
    }

    return CXChildVisit_Recurse;
}

void CppParser::parse(const std::string& file_path, const std::string& content, RepositoryModel& model) {
    std::string file_id = "file:" + file_path;
    std::string file_name = std::filesystem::path(file_path).filename().string();

    Entity file_entity;
    file_entity.id = file_id;
    file_entity.name = file_name;
    file_entity.kind = EntityKind::File;
    file_entity.file_path = file_path;
    model.add_entity(file_entity);

    CXIndex index = clang_createIndex(0, 0);

    struct CXUnsavedFile unsaved_file;
    unsaved_file.Filename = file_path.c_str();
    unsaved_file.Contents = content.c_str();
    unsaved_file.Length = content.length();

    // Use C++20 standard for parsing
    const char* args[] = {"-std=c++20", "-Iinclude", "-xc++"};

    CXTranslationUnit tu = clang_parseTranslationUnit(
        index, 
        file_path.c_str(), 
        args, 3, 
        &unsaved_file, 1, 
        CXTranslationUnit_DetailedPreprocessingRecord | CXTranslationUnit_KeepGoing
    );

    if (tu) {
        CXCursor root_cursor = clang_getTranslationUnitCursor(tu);
        ParserState state{&model, file_id, file_path, file_id};
        clang_visitChildren(root_cursor, visitor, &state);
        clang_disposeTranslationUnit(tu);
    } else {
        std::cerr << "[Warning] libclang failed to parse: " << file_path << std::endl;
    }

    clang_disposeIndex(index);
}

} // namespace codeatlas
