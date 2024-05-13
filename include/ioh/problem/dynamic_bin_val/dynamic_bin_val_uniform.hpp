#pragma once
/**
 * @file dynamic_bin_val_uniform.hpp
 * @brief Contains the declaration of the DynamicBinValUniform class, which represents dynamic binary value
 *        problems in the context of IOH.
 */

#include "dynamic_bin_val.hpp"

namespace ioh::problem
{
    /**
     * @class DynamicBinValUniform
     * @brief This class serves to represent dynamic binary value problems within the context of Iterative
     *        Optimization Heuristics (IOH).
     *
     *        DynamicBinValUniform: takes a value between 0 and 1 for each component at each time_step
     *
     *        Inheriting functionalities from the IntegerSingleObjective, it also integrates functionalities
     *        for automatic registration of the problem type into various data structures. This facilitates
     *        easier management and retrieval of problem instances, while encapsulating characteristics and
     *        behaviours specific to dynamic binary value problems. It holds vital data members such as
     *        time_step and weights, which are crucial in depicting the dynamic aspects and unique features
     *        of these problem instances.
     */
    class DynamicBinValUniform final : public DynamicBinValProblem<DynamicBinValUniform>
    {
    protected:
        void update_weights() override
        {
            // Reinitialize the weights with random numbers between 0 and 1 after shuffling
            for (size_t i = 0; i < this->weights.size(); ++i)
            {
                this->weights[i] = std::generate_canonical<double, 10>(this->random_generator.gen);
            }
        }

    public:
        /**
         * @brief Constructs a new instance of DynamicBinValUniform.
         *
         * @param instance the random instance of the problem
         * @param n_variables The dimension of the problem, representing the size of the search space and
         *                    indicating the number of variables in the problem.
         */
        DynamicBinValUniform(const int instance, const int n_variables) :
            DynamicBinValProblem(10'001, instance, n_variables, "DynamicBinValUniform")
        {
            step();
        }
    };
} // namespace ioh::problem