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
#include <string_view>

std::optional<size_t> ParseTaskIndex(char const* userInput, size_t maxTasks)
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

    if (userIdx == 0 || userIdx > maxTasks)
    {
        std::cerr << "Error: Task with index " << userIdx
            << " does not exist (valid: 1..." << maxTasks << ")\n";
        return std::nullopt;
    }

    return static_cast<size_t>(userIdx-1);
}

int main(int argc, char *argv[])
{
    // Create TaskList to get data
    auto tasks = TaskList();

    // Read arguments
    std::string arg = argv[1];
    std::transform(arg.begin(), arg.end(), arg.begin(),
    [](unsigned char c) { return std::tolower(c); });
    switch (argc) {
        case 0:
            std::cerr << "Error: no exe" << std::endl;
            return EXIT_FAILURE;
        
        case 1:
            // TODO: show usage of program
            std::cout << "Error: no argument" << std::endl;
            return EXIT_FAILURE;
    
        case 2:
            if (arg == "list")
            {
                tasks.PrintAllTasks();
                return EXIT_SUCCESS;
            }
            else
                return EXIT_FAILURE;
        
        case 3:
        {   
            if (arg == "add")
            {
                tasks.AddTask(argv[2]);
                return EXIT_SUCCESS;
            }
            if (arg == "delete")
            {
                auto maybeIdx = ParseTaskIndex(
                    argv[2], tasks.Size());

                if (!maybeIdx)
                    return EXIT_FAILURE;

                if (tasks.RemoveTask(*maybeIdx))
                    return EXIT_SUCCESS;
                else 
                {
                    std::cerr << "Error: Could not delete task"
                        << ( *maybeIdx + 1) << "\n";
                    return EXIT_FAILURE;
                }
            }
            if (arg == "mark-in-progress")
            {
                auto maybeIdx = ParseTaskIndex(
                    argv[2], tasks.Size());
                tasks.MarkTask(*maybeIdx, Task::Status::IN_PROGRESS);
                return EXIT_SUCCESS;
            }
            if (arg == "mark-done")
            {
                tasks.MarkTask(std::stoi(argv[2]), Task::Status::DONE);
                return EXIT_SUCCESS;
            }
            if (arg == "list")
            {
                auto arg = std::string(argv[2]);
                std::transform(
                arg.begin(), arg.end(), arg.begin(),
            [](unsigned char c) { return std::tolower(c); });
                tasks.ListTasks(arg);
            }
        } break;
        case 4:
        {
            if (arg == "update")
            {
                auto maybeIdx = ParseTaskIndex(
                    argv[2], tasks.Size());
                
                if (!maybeIdx)
                    return EXIT_FAILURE;

                return tasks.UpdateTask(*maybeIdx, argv[3])
                    ? EXIT_SUCCESS
                    : (std::cerr << "Error: Could not update task" 
                        << *maybeIdx << "\n", EXIT_FAILURE);
            }
        } break;
        default:
            std::cout << "Error: no valid argument" << std::endl;
            return EXIT_FAILURE;
    }
     
    return 0;
}
