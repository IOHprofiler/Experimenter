#pragma once

#include <cassert> // Include the assert header

#include "distorted_onemax_inherited.hpp"
#include "ioh/problem/single.hpp"
#include "ioh/problem/transformation.hpp"
#include "ioh/problem/utils.hpp"

namespace ioh::problem::distorted_onemax
{
    class DistortedOnemax final : public DistortedOnemaxInheritedProblem<DistortedOnemax>
    {
    protected:

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
        DistortedOnemax(const int instance, const int n_variables, const double distortion, const double distortion_probability)
        : DistortedOnemaxInheritedProblem(instance, n_variables, distortion, distortion_probability) {
            assert(distortion_probability_ >= 0.0 && distortion_probability_ <= 1.0 && "Distortion probability must be between 0 and 1");
            optimum_.x = std::vector<int>(n_variables,1);
            optimum_.y = evaluate(optimum_.x);
            optimum_.x = reset_transform_variables(optimum_.x);
            optimum_.y = transform_objectives(optimum_.y);
        }
    };
}
