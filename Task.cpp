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
    std::string s = "";
    switch (m_status) {
        case Status::TODO:
            s = "TODO";
            break;
        case Status::IN_PROGRESS:
            s = "IN PROGRESS";
            break;
        case Status::DONE:
            s = "DONE";
            break;
    }
    stream 
    << "[" << m_id << "] "
    << m_description
    << " (" << s << " ) "
    << "@ { " << m_createdAt << " , "
    << "}\n";
    //<< m_updatedAt << "}\n";
}