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
    
    Task(int id, std::string_view description);
    Task(int id, std::string_view description, Status status, 
        std::chrono::system_clock::time_point createdAt, 
        std::optional<std::chrono::system_clock::time_point> updatedAt);

    ~Task() = default;

    // change task
    bool UpdateTask(std::string_view description);
    void MarkTask(Status status);

    // helper
    void PrintTask(std::ostream& stream) const noexcept;
    void ToJson(std::ostream& stream, int ident = 4) const;
    static constexpr std::string_view toString(Status s) noexcept
    {
        switch(s)
        {
            case Status::TODO: return "TODO";
            case Status::IN_PROGRESS: return "IN_PROGRESS";
            case Status::DONE: return "DONE";
        }

        return {};
    }
    
    // Getter
    int GetId() const noexcept { return m_id; };
    std::string_view GetDescription() const { return m_description; };
    constexpr Status GetStatus() const noexcept { return m_status; };
    std::chrono::system_clock::time_point GetCreatedAt() const { return m_createdAt; };
    std::optional<std::chrono::system_clock::time_point> GetUpdatedAt() const { return m_updatedAt; };

    // Helper methods for time formatting
    std::string GetCreatedAtString() const;
    std::string GetUpdatedAtString() const;

    // Setter
    void SetId(int id) noexcept { m_id = id; };
    //void SetDescription(std::string newDesc);

private:
    int         m_id;
    std::string m_description;
    Status      m_status;
    std::chrono::system_clock::time_point m_createdAt;
    std::optional<std::chrono::system_clock::time_point> m_updatedAt;
};