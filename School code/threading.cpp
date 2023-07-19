#include <iostream>
#include <thread>
#include <vector>

void GenerateRandomNum(int _id, int _cmdLineNum)
{
        int randomNum = 0;

        while(randomNum != _cmdLineNum)
        {
                randomNum = rand() % 10000;
        }

        std::cout << "Thread " << _id << " completed." << std::endl;
}

int main(int argc, char** argv)
{
        std::vector<std::thread> threads;

        for(int i = 0; i < 10; i++)
        {
                threads.emplace_back(GenerateRandomNum, i, argc);
        }

        for(auto &thr : threads)
        {
                thr.join();
        }

        threads.clear();

        std::cout << "All threads have finished finding numbers." << std::endl;

        return 0;
}

// g++ -std=c++11 -pthread -o a.out threading.cpp
// nice -n 19 ./a.out 1414