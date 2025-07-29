#pragma once
#include <chrono>
#include <string>
#include <ostream>
#include <optional>
#include <string_view>

class Task
{
    enum class Status 
    {
        TODO, IN_PROGRESS, DONE
    };
public:
    Task(int id, std::string description);
    ~Task() = default;

    // change task
    void UpdateTask(std::string description);
    void MarkTask(Status status);

    // helper
    void PrintTask(std::ostream& stream);
    void ToJson(std::ostream& stream);
    Task CreateTaskFromString(const std::string& objectString);
    
    // Getter
    int GetId();
    std::string_view GetDescription();
    std::string_view GetStatus();
    std::chrono::system_clock::time_point GetCreatedAt();
    std::optional<std::chrono::system_clock::time_point> GetUpdatedAt();

private:
    int         m_id;
    std::string m_description;
    Status      m_status;
    std::chrono::system_clock::time_point m_createdAt;
    std::optional<std::chrono::system_clock::time_point> m_updatedAt;
};