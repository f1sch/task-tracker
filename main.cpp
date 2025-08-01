#include "TaskList.h"

#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <iostream>
#include <string>
#include <string_view>

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
                size_t idx = std::stoi(argv[2]);
                if (idx > tasks.Size() || idx < 0)
                {
                    std::cout << "Error: Task with index " << idx << " does not"
                    << "exist" << std::endl;
                    return EXIT_FAILURE;
                }
                else 
                {
                    if (idx > 0) 
                        idx--;
                    if (tasks.RemoveTask(idx))
                        return EXIT_SUCCESS;
                    else
                        return EXIT_FAILURE;
                }
            }
            if (arg == "mark-in-progress")
            {
                tasks.MarkTask(std::stoi(argv[2]), Task::Status::IN_PROGRESS);
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

                //arg = argv[2];
                //
                //if (arg == "done")
                //{
                //    tasks.ListTasks(Task::Status::DONE);
                //    return EXIT_SUCCESS;
                //}
                //if (arg == "todo")
                //{
                //    tasks.ListTasks(Task::Status::TODO);
                //    return EXIT_SUCCESS;
                //}
                //if (arg == "in-progress")
                //{
                //    tasks.ListTasks(Task::Status::IN_PROGRESS);
                //    return EXIT_SUCCESS;
                //}
            }
        } break;
        case 4:
        {
            if (arg == "update")
            {
                int idx = std::stoi(argv[2]);
                if (idx > 0) 
                    idx--;
                if (idx < 0)
                {
                    std::cout << "Error: Task with index " << idx 
                        << "does not exist" << std::endl;
                    return EXIT_FAILURE;
                }
                else 
                {
                    tasks.UpdateTask(idx, argv[3]);
                    return EXIT_SUCCESS;
                }
                std::cout << "Error: something went wrong" << std::endl;
                return EXIT_FAILURE;
            }
        } break;
        default:
            std::cout << "Error: no valid argument" << std::endl;
            return EXIT_FAILURE;
    }
     
    return 0;
}
