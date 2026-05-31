#include <gtest/gtest.h>
#include <codeatlas/ai/ai_assistant.hpp>

using namespace codeatlas;

TEST(AIAssistantTest, AnswerQuestionAndHighlightNodes) {
    RepositoryModel model;
    Entity e;
    e.id = "func:parseLiberty";
    e.name = "parseLiberty";
    e.kind = EntityKind::Function;
    e.file_path = "src/sta/liberty.cpp";
    e.docstring = "Parses Liberty timing files";

    model.add_entity(e);

    AIAssistant assistant(model);
    KnowledgeGraph kg(GraphType::MindMap);

    auto answer = assistant.answer_question("Liberty", kg);
    EXPECT_FALSE(answer.text.empty());
    EXPECT_FALSE(answer.highlighted_node_ids.empty());
    EXPECT_EQ(answer.highlighted_node_ids[0], "func:parseLiberty");
}
