#include "TaskList.h"
#include "Task.h"

#include <algorithm>
#include <cctype>
#include <chrono>
#include <ctime>
#include <direct.h>
#include <fstream>
#include <iostream>
#include <iterator>
#include <limits.h>
#include <ostream>
#include <sstream>
#include <string>
#include <string_view>
#include <system_error>
#include <vector>

#ifdef _WIN32
    #define WIN32_LEAN_AND_MEAN
    #include <Windows.h>
#else
    #include <unistd.h>
    #include <limits.h>
#endif

TaskList::TaskList()
{
    auto exeDir = GetExecutablePath();
    g_taskListPath = exeDir / "../task-tracker.json";
    g_taskListPathTmp = exeDir / "../task-tracker.json.tmp";

    // Open JSON file
    // Create reading stream to json file
    std::ifstream read_stream{g_taskListPath};
    if (!read_stream)
    {
        std::cerr << g_taskListPath << " Could not be opened for reading\n";
        return;
    }

    // Read JSON file
    // Stream whole file into as one string
    std::string wholeJsonFile;
    {
        std::ostringstream oss;
        oss << read_stream.rdbuf();
        wholeJsonFile = oss.str();
    }
    if (wholeJsonFile.empty() || wholeJsonFile == "")
    {
        std::cerr << "Error: jsonfilestring is empty\n";
        return;
    }

    // Get every task object as a string
    auto tasksJson = SplitTasks(wholeJsonFile);
    if (tasksJson.empty())
    {
        std::cerr << "Error: tasksJson is empty\n";
        return;
    }
    // Save data in tasks_
    // Convert task object strings to Task object
    for (auto const& s : tasksJson)
    {
        // Get string inside {}
        auto start = s.find('{');
        auto end = s.find('}');
        std::string inner = s.substr(start, end - start -1);

        // Get values
        // TODO: needs checking
        int id = std::stoi(ExtractJsonValue(inner, "id"));
        std::string desc = ExtractJsonValue(inner, "description");
        auto status = 
            ParseStatus(ExtractJsonValue(inner, "status"));
        if (!status)
        {
            std::cerr << "Error: Invalid status value in JSON\n";
            return;
        }
        std::string createdAt = ExtractJsonValue(inner, "createdAt");
        std::string updatedAt = ExtractJsonValue(inner, "updatedAt");
        
        // Parse date strings to time_point objects
        auto createdAtTp = ParseDateTimeString(createdAt);
        std::optional<std::chrono::system_clock::time_point> updatedAtTp;
        if (updatedAt != "null") {
            updatedAtTp = ParseDateTimeString(updatedAt);
        }
        
        tasks_.push_back(
            Task(id, desc, *status, 
                createdAtTp, updatedAtTp));
    }
    if(read_stream.is_open()) read_stream.close();
}

TaskList::~TaskList()
{
    // Only write to file if there are tasks to save
    if (!tasks_.empty())
    {
        WriteVectorToFile(tasks_);
        AtomicReplace(g_taskListPath, g_taskListPathTmp);
    }
}

bool TaskList::AddTask(std::string_view desc)
{
    // Validate input
    if (desc.empty()) 
    {
        return false;
    }
    
    // Check for reasonable length
    if (desc.length() > 1000) 
    {
        return false;
    }
    
    // Perform operation
    Task task = Task(tasks_.size() + 1, desc);
    tasks_.push_back(std::move(task));
    
    return true;
}

bool TaskList::UpdateTask(size_t index, std::string desc)
{
    // Validate bounds
    if (index >= tasks_.size()) 
    {
        return false;
    }
    
    // Validate input
    if (desc.empty()) 
    {
        return false;
    }
    
    // Delegate to Task class
    return tasks_[index].UpdateTask(desc);
}

bool TaskList::RemoveTask(size_t index)
{
    // Validate bounds
    if (index >= tasks_.size()) 
    {
        return false;
    }
    
    // Check if vector is not empty
    if (tasks_.empty()) 
    {
        return false;
    }
    
    tasks_.erase(tasks_.begin() + index);
    return true;
}

bool TaskList::MarkTask(size_t index, Task::Status status)
{
    // Validate bounds
    if (index >= tasks_.size()) 
    {
        return false;
    }
    
    // Validate status 
    if (status != Task::Status::TODO 
        && status != Task::Status::IN_PROGRESS 
        && status != Task::Status::DONE) 
    {
        return false;
    }
    
    tasks_[index].MarkTask(status);
    return true;
}

bool TaskList::ListTasks(std::string s)
{
    std::optional<Task::Status> status = ParseStatus(s);
    if (status)
    {
        auto tasks = GetByStatus(status.value());
        if (tasks.empty()) 
        {
            std::cout << "No tasks found with status: " << s << std::endl;
            return true;  // Not an error, just no results
        }
        
        for (auto task : tasks)
        {
            task.PrintTask(std::cout);
        }
    }
    else 
    {
        PrintAllTasks();
    }
    
    return true;
}

void TaskList::PrintAllTasks() const
{
    for (auto const& task : tasks_)
    {
        task.PrintTask(std::cout);
    }
}

std::vector<Task> TaskList::GetByStatus(Task::Status s) const
{
    std::vector<Task> out;
    for (auto const& task : tasks_)
    {
        if (task.GetStatus() == s)
            out.push_back(task);
    }
    return out;
}

std::vector<Task> TaskList::FindByKeyWord(std::string_view word) const
{
    std::vector<Task> out;
    // TODO:
    
    return out;
}

std::filesystem::path TaskList::GetExecutablePath()
{
    #ifdef _WIN32
        wchar_t buf[MAX_PATH];
        DWORD len = GetModuleFileNameW(NULL, buf, MAX_PATH);
        if (len == 0)
        {
            DWORD err = GetLastError();
            throw std::system_error(err, std::system_category(),
            "GetModuleFileNameW failed");
        }
        if (len == MAX_PATH)
        {
            throw std::runtime_error("Executable path too long");
        }
        std::filesystem::path exePath{std::wstring(buf, buf + len)};
        return exePath.remove_filename();
    #else
        char buf[PATH_MAX];
        ssize_t len = readlink("/proc/self/exe", buf, sizeof(buf)-1);
        buf[(len > 0 && len < (ssize_t)sizeof(buf)) ? len : 0] = '\0';
        return std::filesystem:path{buf}.remove_filename();
    #endif
}

std::vector<std::string> TaskList::SplitTasks(const std::string& json)
{
    // Get indices for inner array of tasks
    auto start = json.find('[');
    auto end = json.rfind(']');
    if (start == std::string::npos || end == std::string::npos || end <= start)
        return {};

    // Get inner array as string
    std::string inner = json.substr(start + 1, end - start - 1);

    // iterate over string and count {}
    std::vector<std::string> result;
    int depth = 0;
    size_t objStart = std::string::npos;

    for (size_t i = 0; i < inner.size(); ++i)
    {
        char c = inner[i];
        if (c == '{')
        {
            if (depth == 0)
            {
                objStart = i;
            }
            ++depth;
        }
        else if (c == '}')
        {
            --depth;
            if (depth == 0 && objStart != std::string::npos)
            {
                // end of object reached
                result.push_back(inner.substr(objStart, i - objStart + 1));
                objStart = std::string::npos;
            }
        }
        // ignore everything else
    }

    return result;
}

std::string TaskList::ExtractJsonValue(const std::string& obj, const std::string& key)
{
    // look for key
    std::string pat = "\"" + key + "\"";
    size_t p = obj.find(pat);
    if (p == std::string::npos)
        return {}; //not found

    // look for ':' after key
    p = obj.find(':', p + pat.size());
    if (p == std::string::npos)
        return {};

    // skip whitespace
    ++p;
    while (p < obj.size() && std::isspace((unsigned char)obj[p]))
        ++p;
    
    // is value inside "" ?
    if (p < obj.size() && obj[p] == '"')
    {
        // string value
        size_t start = ++p;
        size_t end = obj.find('"', p);
        return (end != std::string::npos)
            ? obj.substr(start, end - start) // get inside of "..."
            : std::string{};
    }
    else 
    {
        // no string value
        size_t start = p;

        // read till comma, curly bracked or whitespace
        size_t end = obj.find(",}\n\r", start);
        if (end == std::string::npos)
            end = obj.size();

        // trim trailing spaces
        while (end > start && std::isspace((unsigned char) obj[end-1]))
            --end;
        return obj.substr(start, end - start);
    }
}

void TaskList::WriteVectorToFile(std::vector<Task>& tasks)
{
    std::ofstream write_stream{g_taskListPathTmp, std::ios::trunc};
    if (!write_stream)
    {
        std::cerr << g_taskListPathTmp << " Could not be opened for writing\n";
        return ;
    }

    write_stream << "[\n";
    for (size_t i = 0; i < tasks.size(); ++i)
    {
        tasks[i].ToJson(write_stream, 4);
        if (i + 1 < tasks.size()) write_stream << ",";
        write_stream << "\n";
    }
    write_stream << "]\n";
    write_stream.close();
}

// TODO:
// This function will be deleted in the future.
// The goal is to edit the json file in place and not to replace the whole file
void TaskList::AtomicReplace(const std::filesystem::path& orig, const std::filesystem::path& tmp)
{
    namespace fs = std::filesystem;
    std::error_code ec;

    // Delete original file if exists
    if (fs::exists(orig, ec))
    {
        if (!fs::remove(orig, ec))
        {
            std::cerr << "Error while deleting " << orig << ": " << ec.message()
            << "\n";
            return;
        }
    }

    // Rename tmp file
    fs::rename(tmp, orig, ec);
    if (ec)
    {
        std::cerr << "Error while renaming " << tmp << ": " << ec.message()
        << "\n";
        return;
    }
}

std::optional<Task::Status> TaskList::ParseStatus(std::string_view sv)
{
    static constexpr std::pair<std::string_view, Task::Status> table[] = 
    {
        {"todo", Task::Status::TODO},
        {"in-progress", Task::Status::IN_PROGRESS},
        {"done", Task::Status::DONE},
        {"TODO", Task::Status::TODO},
        {"IN_PROGRESS", Task::Status::IN_PROGRESS},
        {"DONE", Task::Status::DONE}
    };

    auto it = std::find_if(std::begin(table), std::end(table),
        [&]( auto const& entry) {return entry.first == sv; });

    if (it != std::end(table))
        return it->second;
    
    return std::nullopt;
}

std::chrono::system_clock::time_point TaskList::ParseDateTimeString(const std::string& dateStr)
{
    // Expected format: "2025-08-02 23:56:24"
    std::tm tm = {};
    std::istringstream ss(dateStr);
    ss >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S");
    
    if (ss.fail()) {
        // If parsing fails, return current time as fallback
        return std::chrono::system_clock::now();
    }
    
    // Set timezone to local time
    tm.tm_isdst = -1; // Let mktime determine DST
    
    // Convert tm to time_t, then to time_point
    std::time_t time = std::mktime(&tm);
    
    // Check if the parsed time is in the future (more than 1 day ahead)
    auto now = std::chrono::system_clock::now();
    auto parsed_time = std::chrono::system_clock::from_time_t(time);
    auto one_day = std::chrono::hours(24);
    
    if (parsed_time > now + one_day) {
        // If the parsed time is more than 1 day in the future, 
        // it's likely a parsing error, so use current time
        return now;
    }
    
    return parsed_time;
}
