#include "Task.h"
#include <iostream>
#include <sstream>

int main()
{
    auto task1 = Task(0, "Example Task");
    std::ostringstream oss;
    task1.PrintTask(oss);
    std::cout << oss.str();

    return 0;
}