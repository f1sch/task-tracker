#pragma once
#include <chrono>
#include <string>
#include <ostream>
#include <optional>
#include <string_view>

class Task
{
public:
    enum class Status 
    {
        TODO, IN_PROGRESS, DONE
    };
    
    Task(int id, std::string description);
    ~Task() = default;

    // change task
    void UpdateTask(std::string description);
    void MarkTask(Status status);

    // helper
    void PrintTask(std::ostream& stream);
    void ToJson(std::ostream& stream, int ident = 4) const;
    Task CreateTaskFromString(const std::string& objectString);
    
    // Getter
    int GetId() const;
    std::string_view GetDescription() const;
    std::string_view GetStatus() const;
    std::chrono::system_clock::time_point GetCreatedAt() const;
    std::optional<std::chrono::system_clock::time_point> GetUpdatedAt() const;

    // Setter
    void SetId(int id);
    //void SetDescription(std::string newDesc);

private:
    int         m_id;
    std::string m_description;
    Status      m_status;
    std::chrono::system_clock::time_point m_createdAt;
    std::optional<std::chrono::system_clock::time_point> m_updatedAt;
};