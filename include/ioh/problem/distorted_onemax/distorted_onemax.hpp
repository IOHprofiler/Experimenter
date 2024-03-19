#pragma once

#include <cassert> // Include the assert header

#include "ioh/problem/single.hpp"
#include "ioh/problem/transformation.hpp"
#include "ioh/problem/utils.hpp"

namespace ioh::problem
{
    class DistortedOnemax : public IntegerSingleObjective
    {
    protected:

        double distortion_;
        double distortion_probability_;
        std::mt19937 rng_;  // Random number generator
        std::bernoulli_distribution distortion_distribution_;  // Bernoulli distribution for distortion

        struct VectorHash {
            size_t operator()(const std::vector<int>& v) const {
                std::hash<int> hasher;
                size_t seed = 0;
                for (int i : v) {
                    seed ^= hasher(i) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
                }
                return seed;
            }
        };

        std::unordered_map<std::vector<int>, bool, VectorHash> distortion_map_;

        std::vector<int> transform_variables(std::vector<int> x) override
        {
            if (meta_data_.instance > 1 && meta_data_.instance <= 50)
            {
                transformation::variables::random_flip(x, meta_data_.instance);
            }
            else if (meta_data_.instance > 50 && meta_data_.instance <= 100)
            {
                transformation::variables::random_reorder(x, meta_data_.instance);
            }
            return x;
        }

        double transform_objectives(const double y) override
        {
            using namespace transformation::objective;
            if (meta_data_.instance > 1)
            {
                return uniform(shift, uniform(scale, y, meta_data_.instance, 0.2, 5.0), meta_data_.instance, -1e3, 1e3);
            }
            return y;
        }

        double evaluate(const std::vector<int> &x) override {
            bool should_distort;

            // Check if this vector has been seen before
            auto it = distortion_map_.find(x);
            if (it != distortion_map_.end()) {
                // Use the stored value
                should_distort = it->second;
            } else {
                // Compute and store the new value
                should_distort = distortion_distribution_(rng_);
                distortion_map_[x] = should_distort;
            }

            double measurement = std::accumulate(x.begin(), x.end(), 0.0);
            if (should_distort) {
                measurement += distortion_;
            }

            return measurement;
        }

    public:
        DistortedOnemax(const int instance, const int n_variables, const double distortion_probability, const double distortion)
        : IntegerSingleObjective(
            MetaData(999'999, instance, "DistortedOnemax", n_variables, common::OptimizationType::MAX),
            Bounds<int>(n_variables, 0, 1)
          ),
          distortion_(distortion), distortion_probability_(distortion_probability),
          distortion_distribution_(distortion_probability) {

            assert(distortion_probability >= 0.0 && distortion_probability <= 1.0 && "Distortion probability must be between 0 and 1");

            std::random_device rd;  // Initialize a random device
            rng_ = std::mt19937(rd()); // Seed the random number generator
        }
    };
}
