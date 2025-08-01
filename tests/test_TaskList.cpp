// tests/test_TaskList.cpp

#include "../src/TaskList.h"
#include <gtest/gtest.h>

TEST(TaskList_AddAndSize, IncreasesSize) {
    TaskList tl;
    EXPECT_EQ(tl.Size(), 0u);

    tl.AddTask("Test Task");
    EXPECT_EQ(tl.Size(), 1u);
}

TEST(TaskList_RemoveTask, ValidIndex) {
    TaskList tl;
    tl.AddTask("Test nr 1");
    tl.AddTask("Test nr 2");

    // entferne ersten (Index 0)
    EXPECT_TRUE(tl.RemoveTask(0));
    EXPECT_EQ(tl.Size(), 1u);
    EXPECT_EQ(tl.GetByStatus(Task::Status::TODO).size(), 0u);
}

TEST(TaskList_RemoveTask, InvalidIndex) {
    TaskList tl;
    EXPECT_FALSE(tl.RemoveTask(0));
    tl.AddTask("Test nr 1");
    EXPECT_FALSE(tl.RemoveTask(1));  // größer als letzter Index
}

TEST(TaskList_GetByStatus, FiltersCorrectly) {
    TaskList tl;
    tl.AddTask("Test nr 1");
    tl.AddTask("Test nr 2");
    tl.AddTask("Test nr 3");

    auto todos = tl.GetByStatus(Task::Status::TODO);
    EXPECT_EQ(todos.size(), 2u);
    for (auto const& t : todos)
        EXPECT_EQ(t.GetStatus(), Task::Status::TODO);

    auto dones = tl.GetByStatus(Task::Status::DONE);
    EXPECT_EQ(dones.size(), 1u);
    for (auto const& t : dones)
        EXPECT_EQ(t.GetStatus(), Task::Status::DONE);
}
