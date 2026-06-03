#include <gtest/gtest.h>
#include <codeatlas/model/repository_model.hpp>

using namespace codeatlas;

TEST(RepositoryModelTest, AddAndQueryEntity) {
    RepositoryModel model;
    Entity e;
    e.id = "class:Foo";
    e.name = "Foo";
    e.kind = EntityKind::Class;
    e.file_path = "src/foo.cpp";

    model.add_entity(e);

    auto res = model.get_entity("class:Foo");
    ASSERT_TRUE(res.has_value());
    EXPECT_EQ(res->name, "Foo");
    EXPECT_EQ(res->kind, EntityKind::Class);
}

TEST(RepositoryModelTest, SearchEntities) {
    RepositoryModel model;
    Entity e1; e1.id = "1"; e1.name = "TimingCalculator"; e1.docstring = "Calculates arrival times";
    Entity e2; e2.id = "2"; e2.name = "LibertyParser"; e2.docstring = "Parses Liberty library format";

    model.add_entity(e1);
    model.add_entity(e2);

    auto search1 = model.search_entities("timing");
    EXPECT_EQ(search1.size(), 1);
    EXPECT_EQ(search1[0].name, "TimingCalculator");

    auto search2 = model.search_entities("parser");
    EXPECT_EQ(search2.size(), 1);
}
