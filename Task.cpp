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

Task::Task(int id, std::string description)
    : m_id(id), m_description(std::move(description)), m_status(Status::TODO)
{
     m_createdAt = chrono::system_clock::now();
}

void Task::UpdateTask(std::string description)
{
    m_description = description;
    m_updatedAt = chrono::system_clock::now();
}

void Task::MarkTask(Status status)
{
    m_status = status;
}

void Task::PrintTask(std::ostream& stream)
{
    stream 
    << "[" << m_id << "] "
    << m_description
    << " (" << GetStatus() << ") "
    << "created @ " << m_createdAt << "\n";
    //<< "}\n";
    //<< m_updatedAt << "}\n";
}

void Task::ToJson(std::ostream& stream)
{
    stream
    << "{\n"
    << "    \"id\": " << m_id << ",\n"
    << "    \"description\": " << "\"" << m_description << "\",\n"
    << "    \"status\": " << "\"" << GetStatus() << "\",\n"
    << "    \"createdAt\": " << "\"" << m_createdAt << "\",\n"
    << "    \"updatedAt\": " << "\"" << "null" << "\"\n"
    << "}";
}

int Task::GetId()
{
    return m_id;
}

std::string_view Task::GetDescription()
{
    return m_description;
}

std::string_view Task::GetStatus()
{
    switch (m_status) 
    {
        case Status::TODO:
            return "TODO";
        case Status::IN_PROGRESS:
            return "IN PROGRESS";
        case Status::DONE:
            return "DONE";
        default:
            return "NO STATUS";
    }   
}

std::chrono::system_clock::time_point Task::GetCreatedAt()
{
    return m_createdAt;
}
std::optional<std::chrono::system_clock::time_point> Task::GetUpdatedAt()
{
    return m_updatedAt;
}

//Task CreateTaskFromString(const std::string& objectString)
//{
//    auto task = Task()
//}