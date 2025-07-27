#pragma once
#include <chrono>
#include <string>
#include <ostream>
#include <optional>
//#include <string_view>

class Task
{
    enum class Status 
    {
        TODO, IN_PROGRESS, DONE
    };
public:
    Task(int id, std::string description);
    ~Task() = default;

    //void AddTask(std::string description);
    void UpdateTask(std::string description);
    //void DeleteTask(int id);

    void MarkTask(Status status);

    void PrintTask(std::ostream& stream);

private:
    int         m_id;
    std::string m_description;
    Status      m_status;
    std::chrono::system_clock::time_point m_createdAt;
    std::optional<std::chrono::system_clock::time_point> m_updatedAt;
};