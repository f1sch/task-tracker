#pragma once
#include "Task.h"
#include <vector>
#include <optional>
#include <string_view>
#include <filesystem>
#include <chrono>

class TaskList
{
public:
    TaskList(const std::filesystem::path& jsonPath = "task-tracker.json");
    ~TaskList();

    // CRUD
    bool AddTask(std::string_view desc);
    bool UpdateTask(size_t index, std::string desc);
    bool RemoveTask(size_t index);
    bool MarkTask(size_t index, Task::Status);
    bool ListTasks(std::string s);

    // Helper
    void PrintAllTasks() const;
    // Size
    size_t Size() const noexcept { return tasks_.size(); }
    
    // Filter
    std::vector<Task> GetByStatus(Task::Status s) const;
    std::vector<Task> FindByKeyWord(std::string_view word) const;

private:
    // Modify
    std::vector<std::string> SplitTasks(const std::string& json);
    std::string ExtractJsonValue(const std::string& obj, const std::string& key);
    std::optional<Task::Status> ParseStatus(std::string_view sv);
    std::chrono::system_clock::time_point ParseDateTimeString(const std::string& dateStr);
    
    // File management
    std::filesystem::path GetExecutablePath();
    void AtomicReplace(const std::filesystem::path& orig, const std::filesystem::path& tmp);
    void WriteVectorToFile(std::vector<Task>& tasks);
    bool LoadFromFile(const std::filesystem::path& jsonPath);
    //bool SaveToFile(std::string const& filename) const;

private:
    std::vector<Task> tasks_;
    std::filesystem::path g_taskListPath;
    std::filesystem::path g_taskListPathTmp;
};