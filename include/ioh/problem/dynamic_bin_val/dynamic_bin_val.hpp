/**
 * @file dynamic_bin_val.hpp
 * @brief Contains the declaration of the DynamicBinVal class, which represents dynamic binary value
 *        problems in the context of IOH.
 */

#pragma once

#include <algorithm>
#include <iostream>
#include <random>
#include <vector>

#include "ioh/problem/single.hpp"
#include "ioh/problem/transformation.hpp"

namespace ioh::problem
{
    /**
     * @class DynamicBinVal
     * @brief This class serves to represent dynamic binary value problems within the context of Iterative
     *        Optimization Heuristics (IOH).
     *
     *        Inheriting functionalities from the IntegerSingleObjective, it also integrates functionalities
     *        for automatic registration of the problem type into various data structures. This facilitates
     *        easier management and retrieval of problem instances, while encapsulating characteristics and
     *        behaviours specific to dynamic binary value problems. It holds vital data members such as
     *        timestep and weights, which are crucial in depicting the dynamic aspects and unique features
     *        of these problem instances.
     */
    class DynamicBinVal : public
        IntegerSingleObjective,
        AutomaticProblemRegistration<DynamicBinVal, DynamicBinVal>,
        AutomaticProblemRegistration<DynamicBinVal, IntegerSingleObjective>
    {
    public:

        int timestep; /**< The current timestep in the dynamic binary value problem scenario. */
        std::vector<double> weights; /**< A vector of weights used in the evaluation of the problem. */
        std::mt19937 random_generator;

        /**
         * @brief Constructs a new instance of DynamicBinVal.
         *
         * @param n_variables The dimension of the problem, representing the size of the search space and
         *                    indicating the number of variables in the problem.
         */
        DynamicBinVal(const int instance, const int n_variables) :
            IntegerSingleObjective
            (
                MetaData(10001, 1, "DynamicBinVal", n_variables, common::OptimizationType::MAX),
                Bounds<int>(n_variables, 0, 1)
            ),
            random_generator(instance)
        {
            if (n_variables == 1) { return; }

            this->timestep = 0;

            // Initialize the weights vector with random numbers between 0 and 1
            this->weights.resize(n_variables);
            for(int i = 0; i < n_variables; ++i)
            {
                this->weights[i] = std::generate_canonical<double, 10>(this->random_generator);
            }
        }

        int step()
        {
            this->timestep += 1;

            // Reinitialize the weights with random numbers between 0 and 1 after shuffling
            for(size_t i = 0; i < this->weights.size(); ++i)
            {
                this->weights[i] = std::generate_canonical<double, 10>(this->random_generator);
            }

            return this->timestep;
        }

    protected:

        /**
         * @brief Evaluates the problem instance using the given input vector.
         *
         * @param x The input vector which represents a potential solution to the problem.
         * @return The evaluation result as a double value.
         */
        double evaluate(const std::vector<int> &x) override
        {
            double value = 0;
            for(size_t i = 0; i < x.size(); ++i)
            {
                value += x[i] * this->weights[i];
            }

            return value;
        }
    };
} // namespace ioh::problem