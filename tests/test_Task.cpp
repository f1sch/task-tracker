#include <gtest/gtest.h>
#include "../src/Task.h"

TEST(TaskTest, BasicConstruction) {
    Task task(0, "Test Task");
    EXPECT_EQ(task.GetDescription(), "Test Task");
    EXPECT_EQ(task.GetStatus(), Task::Status::TODO);
}

TEST(TaskTest, DefaultConstruction) {
    Task task(0, "Test Task");
    EXPECT_EQ(task.GetDescription(), "Test Task");
    EXPECT_EQ(task.GetStatus(), Task::Status::TODO);
}
