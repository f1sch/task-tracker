#include "Task.h"
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>

const std::string g_taskListPath = "../task-tracker.json";

int main()
{
    // TESTING:
    //auto task1 = Task(0, "Example Task");
    //std::ostringstream oss;
    //task1.PrintTask(oss);
    //std::cout << oss.str();

    // ******************** READ JSON ********************
    // Create json file if it doesn't exist
    std::ofstream outf{g_taskListPath};
    if (!outf)
    {
        std::cerr << g_taskListPath << " Could not be opened for writing\n";
        return 1;
    }
    
    auto task1 = Task(1, "Buy some milk");
    std::ostringstream oss;
    task1.ToJson(oss);
    outf << oss.str();
    outf.close();

    std::ifstream inf{g_taskListPath};
    if (!inf)
    {
        std::cerr << g_taskListPath << " Could not be opened for reading\n";
        return 1;
    }

    std::string input{};
    while (std::getline(inf, input))
    {
        std::cout << input << '\n';
    }
    // json lesen, alles Tasks in vector speichern
    std::vector<Task> tasks;

    // **************** READ USER INPUT ****************


    
    inf.close();
    return 0;
}