#include "../src/Task.h"
#include <gtest/gtest.h>
#include <chrono>

TEST(TaskTest, BasicConstruction) 
{
    Task task(0, "Test Task");
    EXPECT_EQ(task.GetDescription(), "Test Task");
    EXPECT_EQ(task.GetStatus(), Task::Status::TODO);
}

TEST(TaskTest, AdvancedConstruction)
{
    int id = 0; 
    std::string desc = "Test Task§§$$";
    auto status = Task::Status::IN_PROGRESS;
    auto createdAt = std::chrono::system_clock::now();
    Task task(id, desc, status, createdAt, std::nullopt);
    EXPECT_EQ(task.GetId(), id);
    EXPECT_EQ(task.GetDescription(), desc);
    EXPECT_EQ(task.GetStatus(), status);
    EXPECT_EQ(task.GetCreatedAt(), createdAt);
    EXPECT_EQ(task.GetUpdatedAt(), std::nullopt);
}

TEST(TaskTest, UpdateTask) 
{
    Task task(0, "Test Task");
    std::string newDesc = "New Description";
    task.UpdateTask(newDesc);
    EXPECT_EQ(task.GetDescription(), newDesc);
    EXPECT_NE(task.GetUpdatedAt(), std::nullopt);
}

TEST(TaskTest, MarkTest)
{
    Task task(0, "Test Task");
    auto status = Task::Status::IN_PROGRESS;
    task.MarkTask(status);
    EXPECT_EQ(task.GetStatus(), status);
}
