#include "Task.h"
#include <chrono>
#include <ctime>
#include <iomanip>
#include <ostream>

namespace chrono = std::chrono;

std::ostream& operator<<(std::ostream& os, chrono::system_clock::time_point tp)
{
    std::time_t t = chrono::system_clock::to_time_t(tp);
    std::tm tm;
    #ifdef _WIN32
        localtime_s(&tm, &t);
    #else
        localtime_r(&t, &tm);
    #endif
    os << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
    return os;
}

Task::Task(int id, std::string_view description)
    : m_id(id), m_description(description), m_status(Status::TODO)
{
     m_createdAt = chrono::system_clock::now();
}

Task::Task(int id, std::string_view description, Status status, 
        std::chrono::system_clock::time_point createdAt, 
        std::optional<std::chrono::system_clock::time_point> updatedAt)
    : m_id(id), m_description(description), m_status(status), m_createdAt(createdAt), m_updatedAt(updatedAt)
{

}

bool Task::UpdateTask(std::string_view description)
{
    m_description = description;
    m_updatedAt = chrono::system_clock::now();
    return true;
}

void Task::MarkTask(Status status)
{
    m_status = status;
}

void Task::PrintTask(std::ostream& stream) const noexcept
{
    stream << "id: " << GetId() << "\n";
    stream << "description: " << GetDescription() << "\n";
    stream << "status: " << toString(GetStatus()) << "\n";
    std::chrono::system_clock::time_point tp = GetCreatedAt();
    stream << "createdAt: " << std::format("{:%F %T}", tp) << "\n";
    std::optional<std::chrono::system_clock::time_point> opt_tp = GetUpdatedAt();
    
    opt_tp ? 
    stream << "updatedAt: " << std::format("{:%F %T}", *opt_tp) << "\n"
    : stream << "updatedAt: null" << "\n";
    
}

void Task::ToJson(std::ostream& stream, int indent) const
{
    const std::string ind(indent, ' ');
    stream  << ind << "{\n"
            << ind << "    \"id\": " << m_id << ",\n"
            << ind << "    \"description\": " << "\"" << m_description << "\",\n"
            << ind << "    \"status\": " << "\"" << toString(GetStatus()) << "\",\n"
            << ind << "    \"createdAt\": " << "\"" << m_createdAt << "\",\n"
            << ind << "    \"updatedAt\": " << "\"" << "null" << "\"\n"
            << ind << "}";
}
