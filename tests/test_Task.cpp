#include "../src/Task.h"
#include <gtest/gtest.h>
#include <chrono>
#include <sstream>

class TaskTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Common setup for all tests
    }
};

// Basic Construction Tests
TEST_F(TaskTest, BasicConstruction) 
{
    Task task(1, "Test Task");
    EXPECT_EQ(task.GetId(), 1);
    EXPECT_EQ(task.GetDescription(), "Test Task");
    EXPECT_EQ(task.GetStatus(), Task::Status::TODO);
    EXPECT_NE(task.GetCreatedAt(), std::chrono::system_clock::time_point{});
    EXPECT_EQ(task.GetUpdatedAt(), std::nullopt);
}

TEST_F(TaskTest, AdvancedConstruction)
{
    int id = 42; 
    std::string desc = "Test Task with special chars: §§$$";
    auto status = Task::Status::IN_PROGRESS;
    auto createdAt = std::chrono::system_clock::now();
    auto updatedAt = std::chrono::system_clock::now() + std::chrono::hours(1);
    
    Task task(id, desc, status, createdAt, updatedAt);
    EXPECT_EQ(task.GetId(), id);
    EXPECT_EQ(task.GetDescription(), desc);
    EXPECT_EQ(task.GetStatus(), status);
    EXPECT_EQ(task.GetCreatedAt(), createdAt);
    EXPECT_EQ(task.GetUpdatedAt(), updatedAt);
}

TEST_F(TaskTest, ConstructionWithNullUpdatedAt)
{
    int id = 1;
    std::string desc = "Test Task";
    auto status = Task::Status::DONE;
    auto createdAt = std::chrono::system_clock::now();
    
    Task task(id, desc, status, createdAt, std::nullopt);
    EXPECT_EQ(task.GetId(), id);
    EXPECT_EQ(task.GetDescription(), desc);
    EXPECT_EQ(task.GetStatus(), status);
    EXPECT_EQ(task.GetCreatedAt(), createdAt);
    EXPECT_EQ(task.GetUpdatedAt(), std::nullopt);
}

// Update Tests
TEST_F(TaskTest, UpdateTask) 
{
    Task task(1, "Original Description");
    std::string newDesc = "New Description";
    
    auto beforeUpdate = std::chrono::system_clock::now();
    bool result = task.UpdateTask(newDesc);
    auto afterUpdate = std::chrono::system_clock::now();
    
    EXPECT_TRUE(result);
    EXPECT_EQ(task.GetDescription(), newDesc);
    EXPECT_NE(task.GetUpdatedAt(), std::nullopt);
    
    // Check that updatedAt is set to a reasonable time
    auto updatedAt = task.GetUpdatedAt();
    EXPECT_GE(*updatedAt, beforeUpdate);
    EXPECT_LE(*updatedAt, afterUpdate);
}

TEST_F(TaskTest, UpdateTaskWithEmptyDescription)
{
    Task task(1, "Original Description");
    bool result = task.UpdateTask("");
    
    EXPECT_FALSE(result);
    EXPECT_EQ(task.GetDescription(), "Original Description");
    EXPECT_EQ(task.GetUpdatedAt(), std::nullopt);
}

TEST_F(TaskTest, UpdateTaskWithVeryLongDescription)
{
    Task task(1, "Original Description");
    std::string longDesc(1001, 'a'); // More than 1000 characters
    
    bool result = task.UpdateTask(longDesc);
    
    EXPECT_FALSE(result);
    EXPECT_EQ(task.GetDescription(), "Original Description");
    EXPECT_EQ(task.GetUpdatedAt(), std::nullopt);
}

// Status Tests
TEST_F(TaskTest, MarkTaskToInProgress)
{
    Task task(1, "Test Task");
    task.MarkTask(Task::Status::IN_PROGRESS);
    EXPECT_EQ(task.GetStatus(), Task::Status::IN_PROGRESS);
}

TEST_F(TaskTest, MarkTaskToDone)
{
    Task task(1, "Test Task");
    task.MarkTask(Task::Status::DONE);
    EXPECT_EQ(task.GetStatus(), Task::Status::DONE);
}

TEST_F(TaskTest, MarkTaskToTodo)
{
    auto now = std::chrono::system_clock::now();
    Task task(1, "Test Task", Task::Status::DONE, now, std::nullopt);
    task.MarkTask(Task::Status::TODO);
    EXPECT_EQ(task.GetStatus(), Task::Status::TODO);
}

// String Conversion Tests
TEST_F(TaskTest, GetCreatedAtString)
{
    auto now = std::chrono::system_clock::now();
    Task task(1, "Test Task", Task::Status::TODO, now, std::nullopt);
    
    std::string createdAtStr = task.GetCreatedAtString();
    EXPECT_FALSE(createdAtStr.empty());
    EXPECT_NE(createdAtStr, "null");
    
    // Should contain date format like "2025-08-02 23:30:00"
    EXPECT_NE(createdAtStr.find('-'), std::string::npos);
    EXPECT_NE(createdAtStr.find(':'), std::string::npos);
}

TEST_F(TaskTest, GetUpdatedAtStringWithValue)
{
    auto now = std::chrono::system_clock::now();
    auto updatedAt = now + std::chrono::hours(1);
    Task task(1, "Test Task", Task::Status::TODO, now, updatedAt);
    
    std::string updatedAtStr = task.GetUpdatedAtString();
    EXPECT_FALSE(updatedAtStr.empty());
    EXPECT_NE(updatedAtStr, "null");
}

TEST_F(TaskTest, GetUpdatedAtStringWithNull)
{
    Task task(1, "Test Task");
    std::string updatedAtStr = task.GetUpdatedAtString();
    EXPECT_EQ(updatedAtStr, "null");
}

// Print Tests
TEST_F(TaskTest, PrintTask)
{
    auto now = std::chrono::system_clock::now();
    Task task(42, "Test Task", Task::Status::IN_PROGRESS, now, std::nullopt);
    std::ostringstream oss;
    
    task.PrintTask(oss);
    std::string output = oss.str();
    
    EXPECT_NE(output.find("id: 42"), std::string::npos);
    EXPECT_NE(output.find("description: Test Task"), std::string::npos);
    EXPECT_NE(output.find("status: IN_PROGRESS"), std::string::npos);
    EXPECT_NE(output.find("createdAt:"), std::string::npos);
    EXPECT_NE(output.find("updatedAt: null"), std::string::npos);
}

TEST_F(TaskTest, PrintTaskWithUpdatedAt)
{
    auto now = std::chrono::system_clock::now();
    auto updatedAt = now + std::chrono::hours(1);
    Task task(1, "Test Task", Task::Status::DONE, now, updatedAt);
    
    std::ostringstream oss;
    task.PrintTask(oss);
    std::string output = oss.str();
    
    EXPECT_NE(output.find("status: DONE"), std::string::npos);
    EXPECT_NE(output.find("updatedAt:"), std::string::npos);
    EXPECT_EQ(output.find("updatedAt: null"), std::string::npos);
}

// JSON Tests
TEST_F(TaskTest, ToJson)
{
    auto now = std::chrono::system_clock::now();
    Task task(1, "Test Task", Task::Status::IN_PROGRESS, now, std::nullopt);
    std::ostringstream oss;
    
    task.ToJson(oss, 2);
    std::string json = oss.str();
    
    EXPECT_NE(json.find("\"id\": 1"), std::string::npos);
    EXPECT_NE(json.find("\"description\": \"Test Task\""), std::string::npos);
    EXPECT_NE(json.find("\"status\": \"IN_PROGRESS\""), std::string::npos);
    EXPECT_NE(json.find("\"createdAt\":"), std::string::npos);
    EXPECT_NE(json.find("\"updatedAt\": \"null\""), std::string::npos);
}

TEST_F(TaskTest, ToJsonWithIndent)
{
    Task task(1, "Test Task");
    std::ostringstream oss;
    
    task.ToJson(oss, 4);
    std::string json = oss.str();
    
    // Should start with 4 spaces
    EXPECT_EQ(json.substr(0, 4), "    ");
}

// Edge Cases
TEST_F(TaskTest, TaskWithSpecialCharacters)
{
    std::string specialDesc = "Task with \"quotes\", \n newlines, \t tabs, and unicode: 🚀";
    Task task(1, specialDesc);
    
    EXPECT_EQ(task.GetDescription(), specialDesc);
}

TEST_F(TaskTest, TaskWithMaximumLengthDescription)
{
    std::string maxDesc(1000, 'a');
    Task task(1, maxDesc);
    
    EXPECT_EQ(task.GetDescription(), maxDesc);
}

TEST_F(TaskTest, TaskWithZeroId)
{
    Task task(0, "Test Task");
    EXPECT_EQ(task.GetId(), 0);
}

TEST_F(TaskTest, TaskWithNegativeId)
{
    Task task(-1, "Test Task");
    EXPECT_EQ(task.GetId(), -1);
}
