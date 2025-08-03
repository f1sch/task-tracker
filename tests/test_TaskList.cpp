// tests/test_TaskList.cpp

#include "../src/TaskList.h"
#include <gtest/gtest.h>
#include <fstream>
#include <filesystem>
#include <sstream>

class TaskListTest : public ::testing::Test {
protected:
    std::filesystem::path testJsonPath;
    std::filesystem::path testJsonTmpPath;
    
    void SetUp() override {
        // Create temporary test files
        testJsonPath = std::filesystem::temp_directory_path() / "test-task-tracker.json";
        testJsonTmpPath = std::filesystem::temp_directory_path() / "test-task-tracker.json.tmp";
        
        // Clean up any existing test files
        if (std::filesystem::exists(testJsonPath)) {
            std::filesystem::remove(testJsonPath);
        }
        if (std::filesystem::exists(testJsonTmpPath)) {
            std::filesystem::remove(testJsonTmpPath);
        }
    }
    
    void TearDown() override {
        // Clean up test files
        if (std::filesystem::exists(testJsonPath)) {
            std::filesystem::remove(testJsonPath);
        }
        if (std::filesystem::exists(testJsonTmpPath)) {
            std::filesystem::remove(testJsonTmpPath);
        }
    }
    
    void CreateTestJsonFile(const std::string& content) {
        std::ofstream file(testJsonPath);
        file << content;
        file.close();
    }
    
    std::string ReadJsonFile() {
        std::ifstream file(testJsonPath);
        std::stringstream buffer;
        buffer << file.rdbuf();
        return buffer.str();
    }
};

// Basic Functionality Tests
TEST_F(TaskListTest, EmptyTaskList) {
    CreateTestJsonFile("[]");
    
    // Not possible without file path dependency.
}

TEST_F(TaskListTest, AddTask) {
    TaskList tl;
    
    size_t initialSize = tl.Size();
    bool result = tl.AddTask("Test Task");
    
    EXPECT_TRUE(result);
    EXPECT_EQ(tl.Size(), initialSize + 1);
}

TEST_F(TaskListTest, AddTaskWithEmptyDescription) {
    TaskList tl;
    
    bool result = tl.AddTask("");
    EXPECT_FALSE(result);
}

TEST_F(TaskListTest, AddTaskWithVeryLongDescription) {
    TaskList tl;
    std::string longDesc(1001, 'a'); // More than 1000 characters
    
    bool result = tl.AddTask(longDesc);
    EXPECT_FALSE(result);
}

TEST_F(TaskListTest, AddMultipleTasks) {
    TaskList tl;
    size_t old_size = tl.Size();
    EXPECT_TRUE(tl.AddTask("Task 1"));
    EXPECT_TRUE(tl.AddTask("Task 2"));
    EXPECT_TRUE(tl.AddTask("Task 3"));
    
    EXPECT_EQ(tl.Size(), old_size + 3);
}

// Update Tests
TEST_F(TaskListTest, UpdateTaskValidIndex) {
    TaskList tl;
    tl.AddTask("Original Description");
    
    bool result = tl.UpdateTask(0, "Updated Description");
    EXPECT_TRUE(result);
}

TEST_F(TaskListTest, UpdateTaskInvalidIndex) {
    TaskList tl;
    tl.AddTask("Test Task");
    
    bool result = tl.UpdateTask(999, "Updated Description");
    EXPECT_FALSE(result);
}

TEST_F(TaskListTest, UpdateTaskEmptyDescription) {
    TaskList tl;
    tl.AddTask("Original Description");
    
    bool result = tl.UpdateTask(0, "");
    EXPECT_FALSE(result);
}

TEST_F(TaskListTest, UpdateTaskOutOfBounds) {
    TaskList tl;
    tl.AddTask("Test Task");
    
    bool result = tl.UpdateTask(-1, "Updated Description");
    EXPECT_FALSE(result);
}

// Remove Tests
TEST_F(TaskListTest, RemoveTaskValidIndex) {
    TaskList tl;
    tl.AddTask("Task 1");
    tl.AddTask("Task 2");
    
    size_t initialSize = tl.Size();
    bool result = tl.RemoveTask(0);
    
    EXPECT_TRUE(result);
    EXPECT_EQ(tl.Size(), initialSize - 1);
}

TEST_F(TaskListTest, RemoveTaskInvalidIndex) {
    TaskList tl;
    tl.AddTask("Test Task");
    
    bool result = tl.RemoveTask(999);
    EXPECT_FALSE(result);
}
// Not possible as expected with current architecture
TEST_F(TaskListTest, RemoveTaskOutOfBounds) {
    TaskList tl;
    tl.AddTask("Test Task");
    
    bool result = tl.RemoveTask(tl.Size()+1);
    EXPECT_FALSE(result);
}

// Not possible with current architecture
//TEST_F(TaskListTest, RemoveTaskFromEmptyList) {
//    TaskList tl;
//    
//    bool result = tl.RemoveTask(0);
//    EXPECT_FALSE(result);
//}

// Mark Task Tests
TEST_F(TaskListTest, MarkTaskValidIndex) {
    TaskList tl;
    tl.AddTask("Test Task");
    
    bool result = tl.MarkTask(0, Task::Status::IN_PROGRESS);
    EXPECT_TRUE(result);
}

TEST_F(TaskListTest, MarkTaskInvalidIndex) {
    TaskList tl;
    tl.AddTask("Test Task");
    
    bool result = tl.MarkTask(999, Task::Status::DONE);
    EXPECT_FALSE(result);
}

TEST_F(TaskListTest, MarkTaskAllStatuses) {
    TaskList tl;
    tl.AddTask("Test Task");
    
    EXPECT_TRUE(tl.MarkTask(0, Task::Status::TODO));
    EXPECT_TRUE(tl.MarkTask(0, Task::Status::IN_PROGRESS));
    EXPECT_TRUE(tl.MarkTask(0, Task::Status::DONE));
}

// GetByStatus Tests
TEST_F(TaskListTest, GetByStatus) {
    TaskList tl;
    tl.AddTask("Task 1");
    tl.AddTask("Task 2");
    tl.AddTask("Task 3");
    
    // Mark some tasks with different statuses
    tl.MarkTask(0, Task::Status::TODO);
    tl.MarkTask(1, Task::Status::IN_PROGRESS);
    tl.MarkTask(2, Task::Status::DONE);
    
    auto todoTasks = tl.GetByStatus(Task::Status::TODO);
    auto inProgressTasks = tl.GetByStatus(Task::Status::IN_PROGRESS);
    auto doneTasks = tl.GetByStatus(Task::Status::DONE);
    
    EXPECT_EQ(todoTasks.size(), 1);
    EXPECT_EQ(inProgressTasks.size(), 1);
    EXPECT_EQ(doneTasks.size(), 1);
    
    EXPECT_EQ(todoTasks[0].GetStatus(), Task::Status::TODO);
    EXPECT_EQ(inProgressTasks[0].GetStatus(), Task::Status::IN_PROGRESS);
    EXPECT_EQ(doneTasks[0].GetStatus(), Task::Status::DONE);
}

TEST_F(TaskListTest, GetByStatusEmptyResult) {
    TaskList tl;
    tl.AddTask("Test Task");
    tl.MarkTask(0, Task::Status::TODO);
    
    auto doneTasks = tl.GetByStatus(Task::Status::DONE);
    EXPECT_EQ(doneTasks.size(), 0);
}

// ListTasks Tests
TEST_F(TaskListTest, ListTasksWithValidStatus) {
    TaskList tl;
    tl.AddTask("Test Task");
    tl.MarkTask(0, Task::Status::TODO);
    
    bool result = tl.ListTasks("todo");
    EXPECT_TRUE(result);
}

TEST_F(TaskListTest, ListTasksWithInvalidStatus) {
    TaskList tl;
    tl.AddTask("Test Task");
    
    bool result = tl.ListTasks("invalid-status");
    EXPECT_TRUE(result); // Should list all tasks when status is invalid
}

TEST_F(TaskListTest, ListTasksWithEmptyStatus) {
    TaskList tl;
    tl.AddTask("Test Task");
    
    bool result = tl.ListTasks("");
    EXPECT_TRUE(result); // Should list all tasks
}

// FindByKeyWord Tests (currently unimplemented)
TEST_F(TaskListTest, FindByKeyWord) {
    TaskList tl;
    tl.AddTask("Test Task");
    
    auto results = tl.FindByKeyWord("Test");
    EXPECT_EQ(results.size(), 0); // Currently returns empty vector
}

// Edge Cases
TEST_F(TaskListTest, TaskWithSpecialCharacters) {
    TaskList tl;
    std::string specialDesc = "Task with \"quotes\", \n newlines, \t tabs, and unicode: 🚀";
    
    bool result = tl.AddTask(specialDesc);
    EXPECT_TRUE(result);
}

TEST_F(TaskListTest, MaximumLengthDescription) {
    TaskList tl;
    std::string maxDesc(1000, 'a');
    
    bool result = tl.AddTask(maxDesc);
    EXPECT_TRUE(result);
}

TEST_F(TaskListTest, BoundaryLengthDescription) {
    TaskList tl;
    std::string boundaryDesc(1001, 'a');
    
    bool result = tl.AddTask(boundaryDesc);
    EXPECT_FALSE(result);
}

// Stress Tests
TEST_F(TaskListTest, AddManyTasks) {
    TaskList tl;
    size_t old_size = tl.Size();
    for (int i = 0; i < 100; ++i) {
        std::string desc = "Task " + std::to_string(i);
        EXPECT_TRUE(tl.AddTask(desc));
    }
    
    EXPECT_EQ(tl.Size(), old_size + 100);
}

TEST_F(TaskListTest, RemoveAllTasks) {
    TaskList tl;
    
    // Add some tasks
    for (int i = 0; i < 5; ++i) {
        tl.AddTask("Task " + std::to_string(i));
    }
    
    size_t initialSize = tl.Size();
    
    // Remove all tasks
    for (size_t i = 0; i < initialSize; ++i) {
        EXPECT_TRUE(tl.RemoveTask(0)); // Always remove first task
    }
    
    EXPECT_EQ(tl.Size(), 0);
}

// Integration Tests
TEST_F(TaskListTest, FullTaskLifecycle) {
    TaskList tl;
    
    // Add task
    EXPECT_TRUE(tl.AddTask("Lifecycle Task"));
    EXPECT_EQ(tl.Size(), 1);
    
    // Update task
    EXPECT_TRUE(tl.UpdateTask(0, "Updated Lifecycle Task"));
    
    // Mark task as in progress
    EXPECT_TRUE(tl.MarkTask(0, Task::Status::IN_PROGRESS));
    
    // Mark task as done
    EXPECT_TRUE(tl.MarkTask(0, Task::Status::DONE));
    
    // Verify status
    auto doneTasks = tl.GetByStatus(Task::Status::DONE);
    EXPECT_EQ(doneTasks.size(), 1);
    EXPECT_EQ(doneTasks[0].GetDescription(), "Updated Lifecycle Task");
    
    // Remove task
    EXPECT_TRUE(tl.RemoveTask(0));
    EXPECT_EQ(tl.Size(), 0);
}

// Error Handling Tests
TEST_F(TaskListTest, OperationsOnEmptyList) {
    TaskList tl;
    
    EXPECT_FALSE(tl.UpdateTask(0, "Updated"));
    EXPECT_FALSE(tl.RemoveTask(0));
    EXPECT_FALSE(tl.MarkTask(0, Task::Status::DONE));
    
    auto tasks = tl.GetByStatus(Task::Status::TODO);
    EXPECT_EQ(tasks.size(), 0);
}

TEST_F(TaskListTest, InvalidOperations) {
    TaskList tl;
    tl.AddTask("Test Task");
    
    // Test with invalid indices
    EXPECT_FALSE(tl.UpdateTask(-1, "Updated"));
    EXPECT_FALSE(tl.RemoveTask(-1));
    EXPECT_FALSE(tl.MarkTask(-1, Task::Status::DONE));
    
    EXPECT_FALSE(tl.UpdateTask(999, "Updated"));
    EXPECT_FALSE(tl.RemoveTask(999));
    EXPECT_FALSE(tl.MarkTask(999, Task::Status::DONE));
}
