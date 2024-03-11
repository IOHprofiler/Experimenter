#include "../utils.hpp"

#include "ioh/problem/problem.hpp"


#include <iostream>
#include <vector>
#include <random>
#include <algorithm>

#define LOG_FILE_NAME "test.log"

#define LOG(message)                                                           \
  do {                                                                         \
    std::ofstream debug_log(LOG_FILE_NAME, std::ios::app);                     \
    auto now = std::chrono::system_clock::now();                               \
    std::time_t now_time = std::chrono::system_clock::to_time_t(now);          \
    debug_log << "["                                                           \
              << std::put_time(std::localtime(&now_time), "%Y-%m-%d %H:%M:%S") \
              << "] " << message << std::endl;                                 \
    debug_log.close();                                                         \
  } while (0)


int main()
{
    const auto& problem_factory = ioh::problem::ProblemRegistry<ioh::problem::DistortedOnemax>::instance();

    auto landscape = problem_factory.create(1, 10, 0.5, 1000);
    std::vector<int> x = {0, 0, 1, 1, 1, 0, 0, 1, 1, 0};
    auto y = landscape(x);
    LOG("x[0]:" << x[0]);
    LOG("x[1]:" << x[1]);
    LOG("x[2]:" << x[2]);
    LOG("x[3]:" << x[3]);
    LOG("x[4]:" << x[4]);
    LOG("Output:" << y);

    return 0;
}
