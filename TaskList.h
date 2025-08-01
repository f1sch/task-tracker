#pragma once
#include "Task.h"
#include <vector>
#include <optional>
#include <string_view>
#include <filesystem>

class TaskList
{
public:
    TaskList();
    ~TaskList();

    // CRUD
    void AddTask(std::string_view desc);
    bool UpdateTask(size_t index, std::string desc);
    bool RemoveTask(size_t index);
    void MarkTask(size_t index, Task::Status);
    void ListTasks(std::string s);

    // Helper
    void PrintAllTasks();
    // Size
    size_t Size() const noexcept { return tasks_.size(); }
private:
    // Filter
    std::vector<Task> GetByStatus(Task::Status s) const;
    std::vector<Task> FindByKeyWord(std::string_view word) const;

    // Modify
    std::vector<std::string> SplitTasks(const std::string& json);
    std::string ExtractJsonValue(const std::string& obj, const std::string& key);
    std::optional<Task::Status> ParseStatus(std::string_view sv);
    
    // File management
    std::filesystem::path GetExecutablePath();
    void AtomicReplace(const std::filesystem::path& orig, const std::filesystem::path& tmp);
    void WriteVectorToFile(std::vector<Task>& tasks);
    bool LoadFromFile(std::string const& filename);
    bool SaveToFile(std::string const& filename) const;

private:
    std::vector<Task> tasks_;
    std::filesystem::path g_taskListPath;
    std::filesystem::path g_taskListPathTmp;
};