#include "src/TaskList.h"

#include <algorithm>
#include <cctype>
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <exception>
#include <iostream>
#include <optional>
#include <stdexcept>
#include <string>

// TODO:
// - <delete> irgendwas geht bei den indices sehr schief -> behalten unique id
// - tests updaten und mehr schreiben

// Structs
struct Command 
{
    enum class Type 
    {
        LIST, ADD, DELETE, MARK_IN_PROGRESS, MARK_DONE, UPDATE, INVALID
    };
    Type type;
    std::string description;
    std::optional<size_t> taskIndex;
    std::string filter;
};

// Forward declarations
std::optional<Command> ParseArguments(int argc, char* argv[]);
bool ExecuteCommand(const Command& cmd, TaskList& tasks);
std::optional<size_t> ParseTaskIndex(char const* userInput);
void PrintUsage(const char* progName);


std::optional<Command> ParseArguments(int argc, char* argv[])
{
    // Input Validation
    if (argc <= 0 || argv == nullptr || argv[1] == nullptr)
    {
        std::cerr << "Error: invalid arguments" << std::endl;
        return std::nullopt;
    }
    
    if (argc == 1)
    {
        std::cerr << "Error: no arguments" << std::endl;
        return std::nullopt;
    }

    // Command Recognition && Argument Count Validation
    std::string arg1 = argv[1];
    std::transform(arg1.begin(), arg1.end(), arg1.begin(),
    [](unsigned char c) { return std::tolower(c); });
    Command command = {};
    
    if (arg1 == "list")
    {
        if (argc != 2 && argc != 3)
        {
            std::cerr << "Error: wrong number of arguments" << std::endl;
            return std::nullopt;
        }
        command.type = Command::Type::LIST;
        if (argc == 2)
            return command;
        else
        {
            std::string filter = argv[2];
            std::transform(
                filter.begin(), filter.end(), filter.begin(),
                [](unsigned char c) { return std::tolower(c); }
            );
            command.filter = filter;
            return command;
        }
    }
    else if (arg1 == "add")
    {   
        if (argc != 3)
        {
            std::cerr << "Error: wrong number of arguments" << std::endl;
            return std::nullopt;
        }
        command.type = Command::Type::ADD;
        std::string description = argv[2];
        if (!description.empty())
        {
            command.description = description;
            return command;
        }
        else 
        {
            std::cerr << "Error: description is empty" << std::endl;
            return std::nullopt;
        }
    }
    else if (arg1 == "delete")
    {
        if (argc != 3)
        {
            std::cerr << "Error: wrong number of arguments" << std::endl;
            return std::nullopt;
        }
        command.type = Command::Type::DELETE;
        std::optional<size_t> userIdx = ParseTaskIndex(argv[2]);
        
        if (userIdx)
        {
            command.taskIndex = userIdx;
            return command;
        }
        else 
        {
            return std::nullopt;
        }
    }
    else if (arg1 == "update")
    {
        if (argc != 4)
        {
            std::cerr << "Error: wrong number of arguments" << std::endl;
            return std::nullopt;
        }
        command.type = Command::Type::UPDATE;

        std::optional<size_t> userIdx = ParseTaskIndex(argv[2]);
        if (!userIdx)
        {
            std::cerr << "Error: index is invalid index format" << std::endl;
            return std::nullopt;
        } 
        command.taskIndex = userIdx;
        std::string description = argv[3];
        if (!description.empty())
        {
            command.description = description;
            return command;
        }
        else 
        {
            std::cerr << "Error: description is empty" << std::endl;
            return std::nullopt;
        }
    }
    else if (arg1 == "mark-in-progress")
    {
        if (argc != 3)
        {
            std::cerr << "Error: wrong number of arguments" << std::endl;
            return std::nullopt;
        }
        command.type = Command::Type::MARK_IN_PROGRESS;
        std::optional<size_t> userIdx = ParseTaskIndex(argv[2]);
        if (userIdx)
        {
            command.taskIndex = userIdx;
            return command;
        }
        else 
        {
            return std::nullopt;
        }
    }
    else if (arg1 == "mark-done")
    {
        if (argc != 3)
        {
            std::cerr << "Error: wrong number of arguments" << std::endl;
            return std::nullopt;
        }
        command.type = Command::Type::MARK_DONE;
        std::optional<size_t> userIdx = ParseTaskIndex(argv[2]);
        if (userIdx)
        {
            command.taskIndex = userIdx;
            return command;
        }
        else 
        {
            return std::nullopt;
        }
    }
    else
    {
        command.type = Command::Type::INVALID;
        std::cerr << "Error: unknown or invalid command" << std::endl;
        return std::nullopt;
    }
}

bool ExecuteCommand(const Command& cmd, TaskList& tasks)
{
    switch (cmd.type) {
        case Command::Type::LIST:
            if (cmd.filter.empty())
            {
                tasks.PrintAllTasks();
                return true;
            }
            else 
            {
                if (!tasks.ListTasks(cmd.filter)) 
                {
                    std::cerr << "Error: Could not list tasks with filter '" 
                        << cmd.filter << "'" << std::endl;
                    return false;
                }
                return true;
            }
            
        case Command::Type::ADD:
            if (!tasks.AddTask(cmd.description)) 
            {
                std::cerr << "Error: Could not add task '" << cmd.description 
                    << "'" << std::endl;
                return false;
            }
            return true;
            
        case Command::Type::UPDATE:
            if (*cmd.taskIndex >= tasks.Size()) 
            {
                std::cerr << "Error: Task index " << (*cmd.taskIndex + 1) 
                    << " out of range (valid: 1..." << tasks.Size() << ")" 
                    << std::endl;
                return false;
            }
            if (!tasks.UpdateTask(*cmd.taskIndex, cmd.description)) 
            {
                std::cerr << "Error: Could not update task " 
                    << (*cmd.taskIndex + 1) << std::endl;
                return false;
            }
            return true;
            
        case Command::Type::DELETE:
            if (*cmd.taskIndex >= tasks.Size()) 
            {
                std::cerr << "Error: Task index " << (*cmd.taskIndex + 1) 
                    << " out of range (valid: 1..." << tasks.Size() << ")" 
                    << std::endl;
                return false;
            }
            if (!tasks.RemoveTask(*cmd.taskIndex)) 
            {
                std::cerr << "Error: Could not delete task " 
                    << (*cmd.taskIndex + 1) << std::endl;
                return false;
            }
            return true;
            
        case Command::Type::MARK_DONE:
            if (*cmd.taskIndex >= tasks.Size()) 
            {
                std::cerr << "Error: Task index " << (*cmd.taskIndex + 1) 
                    << " out of range (valid: 1..." << tasks.Size() << ")" 
                    << std::endl;
                return false;
            }
            if (!tasks.MarkTask(*cmd.taskIndex, Task::Status::DONE)) 
            {
                std::cerr << "Error: Could not mark task " 
                    << (*cmd.taskIndex + 1) << " as done" << std::endl;
                return false;
            }
            return true;
            
        case Command::Type::MARK_IN_PROGRESS:
            if (*cmd.taskIndex >= tasks.Size()) 
            {
                std::cerr << "Error: Task index " << (*cmd.taskIndex + 1) 
                    << " out of range (valid: 1..." << tasks.Size() << ")" 
                    << std::endl;
                return false;
            }
            if (!tasks.MarkTask(*cmd.taskIndex, Task::Status::IN_PROGRESS)) 
            {
                std::cerr << "Error: Could not mark task " 
                    << (*cmd.taskIndex + 1) << " as in-progress" << std::endl;
                return false;
            }
            return true;
            
        case Command::Type::INVALID:
            std::cerr << "Error: Invalid command type" << std::endl;
            return false;
            
        default:
            std::cerr << "Error: Unknown command type" << std::endl;
            return false;
    }
}

std::optional<size_t> ParseTaskIndex(char const* userInput)
{
    unsigned long userIdx = 0;
    try {
        size_t pos = 0;
        userIdx = std::stoul(userInput, &pos);
        if (pos != std::strlen(userInput))
        {
            throw std::invalid_argument{"Extra characters"};
        }
    } 
    catch (const std::exception&)
    {
        std::cerr << "Error: " << userInput
            << " is not a valid positive integer\n";
            return std::nullopt;
    }

    if (userIdx == 0)
    {
        std::cerr << "Error: Task with index 0 does not exist" << std::endl; 
        return std::nullopt;
    }

    return static_cast<size_t>(userIdx-1);
}
void PrintUsage(const char* progName)
{
    std::cout 
    << "Usage: " << progName << " <command> [options]\n"
    << "Commands:\n"
    << "  add \"<task description>\"            Add a new Task\n"
    << "  update <id> \"<new description>\"     Update an existing task\n"
    << "  delete <id>                           Delete a task\n"
    << "  mark-in-progress <id>                 Mark task as in-progress\n"
    << "  mark-done <id>                        Mark task as done\n"
    << "  list [status]                         List tasks (optional status: "
    << "todo, in-progress, done)\n\n";
}

int main(int argc, char *argv[])
{
    auto command = ParseArguments(argc, argv);
    if (!command)
    {
        PrintUsage(argv[0]);
        return EXIT_FAILURE;
    }
    
    // Create TaskList to get data
    auto tasks = TaskList("task-tracker.json");
    return ExecuteCommand(*command, tasks)
        ? EXIT_SUCCESS : EXIT_FAILURE;
}
