#include "../src/TaskList.h"
#include <gtest/gtest.h>
#include <fstream>
#include <filesystem>

class JsonParsingTest : public ::testing::Test {
protected:
    std::filesystem::path testJsonPath;
    std::filesystem::path testJsonTmpPath;
    
    void SetUp() override {
        testJsonPath = std::filesystem::temp_directory_path() / "test-task-tracker.json";
        testJsonTmpPath = std::filesystem::temp_directory_path() / "test-task-tracker.json.tmp";
        
        if (std::filesystem::exists(testJsonPath)) {
            std::filesystem::remove(testJsonPath);
        }
        if (std::filesystem::exists(testJsonTmpPath)) {
            std::filesystem::remove(testJsonTmpPath);
        }
    }
    
    void TearDown() override {
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
};

// Note: These tests would require modifying the TaskList class to accept a custom file path
// for testing purposes. The current design makes it difficult to test file operations
// without affecting the actual task-tracker.json file.

TEST_F(JsonParsingTest, ValidJsonFormat) {
    std::string validJson = R"([
    {
        "id": 1,
        "description": "Test Task",
        "status": "TODO",
        "createdAt": "2025-08-02 23:30:00",
        "updatedAt": "null"
    }
])";
    
    CreateTestJsonFile(validJson);
    // Would test TaskList loading here if we had dependency injection
}

TEST_F(JsonParsingTest, MultipleTasksJson) {
    std::string multipleTasksJson = R"([
    {
        "id": 1,
        "description": "Task 1",
        "status": "TODO",
        "createdAt": "2025-08-02 23:30:00",
        "updatedAt": "null"
    },
    {
        "id": 2,
        "description": "Task 2",
        "status": "IN_PROGRESS",
        "createdAt": "2025-08-02 23:31:00",
        "updatedAt": "2025-08-02 23:35:00"
    },
    {
        "id": 3,
        "description": "Task 3",
        "status": "DONE",
        "createdAt": "2025-08-02 23:32:00",
        "updatedAt": "null"
    }
])";
    
    CreateTestJsonFile(multipleTasksJson);
    // Would test TaskList loading here
}

TEST_F(JsonParsingTest, EmptyJsonArray) {
    std::string emptyJson = "[]";
    CreateTestJsonFile(emptyJson);
    // Would test empty TaskList loading here
}

TEST_F(JsonParsingTest, InvalidJsonFormat) {
    std::string invalidJson = R"([
    {
        "id": 1,
        "description": "Test Task",
        "status": "TODO",
        "createdAt": "2025-08-02 23:30:00",
        "updatedAt": "null"
    }
    // Missing closing bracket
)";
    
    CreateTestJsonFile(invalidJson);
    // Would test error handling here
}

TEST_F(JsonParsingTest, MissingRequiredFields) {
    std::string incompleteJson = R"([
    {
        "id": 1,
        "description": "Test Task"
        // Missing status, createdAt, updatedAt
    }
])";
    
    CreateTestJsonFile(incompleteJson);
    // Would test error handling here
}

TEST_F(JsonParsingTest, InvalidStatusValue) {
    std::string invalidStatusJson = R"([
    {
        "id": 1,
        "description": "Test Task",
        "status": "INVALID_STATUS",
        "createdAt": "2025-08-02 23:30:00",
        "updatedAt": "null"
    }
])";
    
    CreateTestJsonFile(invalidStatusJson);
    // Would test error handling here
}

TEST_F(JsonParsingTest, InvalidDateFormat) {
    std::string invalidDateJson = R"([
    {
        "id": 1,
        "description": "Test Task",
        "status": "TODO",
        "createdAt": "invalid-date-format",
        "updatedAt": "null"
    }
])";
    
    CreateTestJsonFile(invalidDateJson);
    // Would test date parsing error handling here
}

TEST_F(JsonParsingTest, FutureDateHandling) {
    std::string futureDateJson = R"([
    {
        "id": 1,
        "description": "Test Task",
        "status": "TODO",
        "createdAt": "2030-01-01 00:00:00",
        "updatedAt": "null"
    }
])";
    
    CreateTestJsonFile(futureDateJson);
    // Would test future date handling here
}

TEST_F(JsonParsingTest, SpecialCharactersInDescription) {
    std::string specialCharsJson = R"([
    {
        "id": 1,
        "description": "Task with \"quotes\", \n newlines, \t tabs, and unicode: 🚀",
        "status": "TODO",
        "createdAt": "2025-08-02 23:30:00",
        "updatedAt": "null"
    }
])";
    
    CreateTestJsonFile(specialCharsJson);
    // Would test special character handling here
}

TEST_F(JsonParsingTest, VeryLongDescription) {
    std::string longDesc(1000, 'a');
    std::string longDescJson = R"([
    {
        "id": 1,
        "description": ")" + longDesc + R"(",
        "status": "TODO",
        "createdAt": "2025-08-02 23:30:00",
        "updatedAt": "null"
    }
])";
    
    CreateTestJsonFile(longDescJson);
    // Would test long description handling here
} 