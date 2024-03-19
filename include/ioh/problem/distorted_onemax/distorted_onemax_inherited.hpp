#pragma once

#include "ioh/problem/single.hpp"
#include "ioh/problem/transformation.hpp"

namespace ioh::problem::distorted_onemax
{
    //! DistortedOnemaxInherited base class
    class DistortedOnemaxInherited : public IntegerSingleObjective
    {
    protected:

        double distortion_;
        double distortion_probability_;
        std::bernoulli_distribution distortion_distribution_;  // Bernoulli distribution for distortion
        std::mt19937 rng_;  // Random number generator

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

        //! Optimum before transformation
        std::vector<int> reset_transform_variables(std::vector<int> x)
        {
            if (meta_data_.instance > 1 && meta_data_.instance <= 50)
                transformation::variables::random_flip(x, meta_data_.instance);
            else if (meta_data_.instance > 50 && meta_data_.instance <= 100)
                x = transformation::variables::random_reorder_reset(x, meta_data_.instance);
            return x;
        }

    public:
        /**
         * @brief Construct a new DistortedOnemaxInherited object
         * 
         * @param problem_id The id of the problem
         * @param instance The instance of the problem
         * @param n_variables the dimension of the problem
         * @param name the name of the problem
         */
        DistortedOnemaxInherited(const int instance, const int n_variables, const double distortion, const double distortion_probability) :
            IntegerSingleObjective(
                MetaData(999'999, instance, "DistortedOnemax", n_variables, common::OptimizationType::MAX),
                Bounds<int>(n_variables, 0, 1)
            ),
            distortion_(distortion),
            distortion_probability_(distortion_probability),
            distortion_distribution_(0.5)
        {
            std::random_device rd;
            rng_ = std::mt19937(rd());
        }
    };

    /**
     * @brief CRTP class for DistortedOnemaxInherited problems. Inherit from this class when defining new DistortedOnemaxInherited problems
     * 
     * @tparam ProblemType The New DistortedOnemaxInherited problem class
     */
    template <typename ProblemType>
    class DistortedOnemaxInheritedProblem : public DistortedOnemaxInherited,
                       AutomaticProblemRegistration<ProblemType, DistortedOnemaxInherited>,
                       AutomaticProblemRegistration<ProblemType, IntegerSingleObjective>
    {
    public:
        using DistortedOnemaxInherited::DistortedOnemaxInherited;
    };
}
