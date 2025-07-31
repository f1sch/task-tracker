#include "Task.h"

#include <cctype>
#include <chrono>
#include <cstddef>
#include <cstdio>
#include <ctime>
#include <fstream>
#include <filesystem>
#include <iostream>
#include <ostream>
#include <string>
#include <sstream>
#include <system_error>
#include <vector>

// Get working dir
#include <direct.h>
#include <limits.h>

#ifdef _WIN32
    #define WIN32_LEAN_AND_MEAN
    #include <Windows.h>
#else
    #include <unistd.h>
    #include <limits.h>
#endif

// TODO:
// die kommandos funktionieren einzeln, aber dadurch dass 
// es jedes mal einen full rewrite gibt, werden die daten nur bedingt
// gespeichert:
// default wert von tasks ist TODO -> die status werte werden nur vom
// letzten task gespeichert, da alle tasks jedes mal neu erstellt werden.
// gleiches Problem bei den Zeitdaten

std::filesystem::path g_taskListPath;
std::filesystem::path g_taskListPathTmp;

std::filesystem::path GetExecutablePath()
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

std::vector<std::string> SplitTasks(const std::string& json)
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

std::string ExtractJsonValue(const std::string& obj, const std::string& key)
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

void WriteVectorToFile(std::vector<Task>& tasks)
{
    std::ofstream write_stream{g_taskListPathTmp, std::ios::app};
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

void AtomicReplace(const std::filesystem::path& orig, const std::filesystem::path& tmp)
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

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        std::cout << "Error: no argument\n";
        return 1;
    }
    if (argc > 3)
    {
        std::cout << "Ignoring everything after \"" << argv[2] << "\"\n";
    }
    auto exeDir = GetExecutablePath();
    g_taskListPath = exeDir / "../task-tracker.json";
    g_taskListPathTmp = exeDir / "../task-tracker.json.tmp";
    //g_taskListPathTmp = g_taskListPath.concat(".tmp");

    // ******************** READ JSON ********************
    
    
    // Create reading stream to json file
    std::ifstream read_stream{g_taskListPath};
    if (!read_stream)
    {
        std::cerr << g_taskListPath << " Could not be opened for reading\n";
        return 1;
    }

    // stream whole file into as one string
    std::string wholeJsonFile;
    {
        std::ostringstream oss;
        oss << read_stream.rdbuf();
        wholeJsonFile = oss.str();
    }
    if (wholeJsonFile.empty() || wholeJsonFile == "")
    {
        std::cerr << "Error: jsonfilestring is empty\n";
        return 1;
    }

    // get every task object as a string
    auto tasksJson = SplitTasks(wholeJsonFile);
    if (tasksJson.empty())
    {
        std::cerr << "Error: tasksJson is empty\n";
        return 1;
    }

    // convert task object strings to Task object
    std::vector<Task> tasks;
    for (auto const& s : tasksJson)
    {
        // Get string inside {}
        auto start = s.find('{');
        auto end = s.find('}');
        std::string inner = s.substr(start, end - start -1);

        // get values
        //int id = stoi(ExtractJsonValue(inner, "id"));
        std::string desc = ExtractJsonValue(inner, "description");
        tasks.push_back(Task(tasks.size()+1, desc));
    }

    // Print every task
    //for (auto task : tasks)
    //{
    //    task.PrintTask(std::cout);
    //}

    // **************** READ USER INPUT ****************
    
    // Check arguments
    std::string command = argv[1];
    if (command == "list")
    {
        if (argc == 2)
        {
            // Print every line of json file to console
            std::string input{};
            while (std::getline(read_stream, input))
            {
                std::cout << input << '\n';
            }
            read_stream.close();
            return 0;
        }
        else if (argc == 3)
        {
            std::string status = argv[2];
            if (status == "done")
            {

            }
            if (status == "todo")
            {

            }
            if (status == "in-progress")
            {

            }
        }
        else
        {
            std::cout << "list command: list [option]" << std::endl;
        }
    }
    else
    {
        // Create json file if it doesn't exist
        std::ofstream write_stream{g_taskListPath, std::ios::app};
        if (!write_stream)
        {
            std::cerr << g_taskListPath << " Could not be opened for writing\n";
            return 1;
        }
        if (command == "add")
        {
            size_t idx = tasks.size();
            tasks.push_back(Task(idx+1, argv[2]));
            
            std::cout << "Task added successfully (ID: " << tasks[idx].GetId() << ")" 
            << std::endl;
        }
        if (command == "update")
        {
            int id = std::stoi(argv[2]) - 1;
            std::string newDesc = argv[3];

            tasks[id].UpdateTask(newDesc);
        }
        if (command == "delete")
        {
            int id = std::stoi(argv[2]) - 1;
            tasks.erase(tasks.begin() + id);

            // update ids to be continous
            // if this is not wanted and ids
            // are expected to be unique -> skip this step
            for (size_t i = 0; i < tasks.size(); ++i)
            {
                tasks[i].SetId(static_cast<int>(i) + 1);
            }
        }
        if (command == "mark-in-progress")
        {
            int id = std::stoi(argv[2]) - 1;
            tasks[id].MarkTask(Task::Status::IN_PROGRESS);
        }
        if (command == "mark-done")
        {
            int id = std::stoi(argv[2]) - 1;
            tasks[id].MarkTask(Task::Status::DONE);
        }

        WriteVectorToFile(tasks);
        if(read_stream.is_open()) read_stream.close();
        if(write_stream.is_open()) write_stream.close();
        AtomicReplace(g_taskListPath, g_taskListPathTmp);
    }    
    return 0;
}
