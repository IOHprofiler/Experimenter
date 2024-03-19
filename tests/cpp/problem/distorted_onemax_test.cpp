#include "../utils.hpp"
#include "ioh/problem/distorted_onemax/distorted_onemax.hpp"
#include <iostream>
#include <vector>
#include <random>
#include <algorithm>
#include <sstream>
#include <fstream>
#include <chrono>
#include <iomanip>

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

std::string vec_to_string(const std::vector<int>& vec) {
  std::ostringstream oss;
  for (size_t i = 0; i < vec.size(); ++i) {
    if (i != 0)
      oss << ", ";
    oss << vec[i];
  }
  return oss.str();
}

void run_test_case(int instance, int n_variables, double distortion_probability, double distortion, const std::vector<int>& x) {
    ioh::problem::DistortedOnemax landscape(1, n_variables, distortion_probability, distortion);

    for (int i = 0; i < 5; ++i) {
        std::vector<int> test_vector = x;

        // Modify test_vector for evaluations 2, 4, and 5
        if (i == 1 || i == 3 || i == 4) {
            std::random_device rd;
            std::mt19937 g(rd());

            // Flip random bits
            for (size_t j = 0; j < test_vector.size(); ++j) {
                if (g() % 2 == 0) { // 50% chance to flip each bit
                    test_vector[j] = 1 - test_vector[j];
                }
            }
        }

        auto y = landscape(test_vector);
        LOG("Evaluation " << (i + 1) << ": Instance: " << instance << ", n_variables: " << n_variables
            << ", distortion_probability: " << distortion_probability
            << ", distortion: " << distortion);
        LOG("Input Vector x: [" << vec_to_string(test_vector) << "]");
        LOG("Output: " << y);
    }
}

int main() {
    // Varying instances, variable counts, distortion probabilities, distortion values, and input vectors
    // Instance values range from typical (1) to edge cases (0, 100)
    // Variable counts range from minimal (1) to larger (20)
    // Distortion probabilities and values vary to test different behaviors

    // Typical case
    run_test_case(1, 10, 0.5, 1000, {0, 0, 1, 1, 1, 0, 0, 1, 1, 0});

    // Edge case: Zero instance
    run_test_case(0, 10, 0.5, 1000, {1, 1, 1, 1, 1, 1, 1, 1, 1, 1});

    // Edge case: High instance number
    run_test_case(100, 10, 0.5, 1000, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0});

    // Small variable count
    run_test_case(1, 1, 0.5, 1000, {1});

    // Large variable count
    run_test_case(1, 20, 0.5, 1000, {0, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0});

    // Distortion probability edge cases
    run_test_case(1, 10, 0.0, 1000, {0, 1, 0, 1, 0, 1, 0, 1, 0, 1}); // Zero probability
    run_test_case(1, 10, 1.0, 1000, {1, 0, 1, 0, 1, 0, 1, 0, 1, 0}); // Full probability

    // Distortion value variations
    run_test_case(1, 10, 0.5, -1000, {1, 1, 0, 0, 0, 1, 1, 0, 0, 1});
    run_test_case(1, 10, 0.5, 0, {0, 1, 1, 0, 0, 1, 0, 1, 1, 0});
    run_test_case(1, 10, 0.5, 500, {1, 0, 1, 1, 0, 0, 1, 0, 1, 1});

    // Random and edge vectors
    run_test_case(1, 10, 0.5, 1000, {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}); // All ones
    run_test_case(1, 10, 0.5, 1000, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}); // All zeros

    // Repeat with varying instances and variable counts
    for (int i = 1; i <= 5; ++i) {
        for (int n = 5; n <= 15; n += 5) {
            std::vector<int> x(n, i % 2); // Alternating 0s and 1s
            run_test_case(i, n, 0.5, 1000, x);
        }
    }

    // Specific edge and corner cases
    run_test_case(50, 10, 0.5, 1000, {1, 0, 1, 0, 1, 0, 1, 0, 1, 0});
    run_test_case(51, 10, 0.5, 1000, {0, 1, 0, 1, 0, 1, 0, 1, 0, 1});
    run_test_case(1, 10, 0.25, 2000, {1, 1, 1, 1, 0, 0, 0, 0, 1, 1});
    run_test_case(2, 10, 0.75, 1500, {0, 0, 0, 0, 1, 1, 1, 1, 0, 0});
    // ... Add more specific test cases as needed

    return 0;
}
