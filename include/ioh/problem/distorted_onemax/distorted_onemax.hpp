#pragma once

#include "ioh/problem/single.hpp"
#include "ioh/problem/utils.hpp"
#include "ioh/problem/transformation.hpp"

namespace ioh::problem
{
    //! Base class for DistortedOnemax problems
    class DistortedOnemax : public IntegerSingleObjective
    {
    protected:

        double distortion_;
        double distortion_probability_;

        // hashmap with all the x's
        // std::set<std::vector<int>> seen_x;

        std::vector<int> transform_variables(std::vector<int> x) override
        {
            if (meta_data_.instance > 1 && meta_data_.instance <= 50)
                transformation::variables::random_flip(x, meta_data_.instance);
            else if (meta_data_.instance > 50 && meta_data_.instance <= 100)
                transformation::variables::random_reorder(x, meta_data_.instance);
            return x;
        }

        double transform_objectives(const double y) override
        {
            using namespace transformation::objective;
            if (meta_data_.instance > 1)
                return uniform(shift,
                            uniform(
                                scale, y, meta_data_.instance, 0.2, 5.0
                            ),
                        meta_data_.instance, -1e3, 1e3
                    );
            return y;
        }

        double evaluate(const std::vector<int> &x) override
        {
          bool should_distort = /* draw according to p */ false;
          double onemax_value = std::accumulate(x.begin(), x.end(), 0.0);

          if (should_distort)
          {
            return onemax_value;
          }

          return onemax_value + this->distortion_;
        }

    public:
        /**
         * @brief Construct a new DistortedOnemax object
         * 
         * @param instance instance id
         * @param n_variables the dimension of the problem
         */
        DistortedOnemax(const int instance, const int n_variables, const double distortion_probability, const double distortion) :
          IntegerSingleObjective(
            MetaData(problem_id, instance, name, n_variables, common::OptimizationType::MAX),
            Bounds<int>(n_variables, 0, 1)
          ),
        {
          this->distortion_ = distortion;
          this->distortion_probability_ = distortion_probability;

        }
    };
}
