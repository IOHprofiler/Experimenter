#pragma once

#include "ioh/problem/problem.hpp"
#include "ioh/problem/registry.hpp"

namespace ioh::problem
{
    class BBOBProblem : public RealProblem
    {
    protected:
        struct TransformationState
        {
            long seed;
            std::vector<double> exponents{};
            std::vector<double> conditions{};
            std::vector<std::vector<double>> transformation_matrix{};
            std::vector<double> transformation_base{};
            std::vector<std::vector<double>> second_transformation_matrix{};
            std::vector<std::vector<double>> first_rotation{};
            std::vector<std::vector<double>> second_rotation{};

            TransformationState(const long problem_id, const int instance, const int n_variables,
                                const double condition = sqrt(10.0)) :
                seed((problem_id == 4 || problem_id == 18 ? problem_id - 1 : problem_id) + 10000 * instance),
                exponents(n_variables),
                conditions(n_variables),
                transformation_matrix(n_variables, std::vector<double>(n_variables)),
                transformation_base(n_variables),
                second_transformation_matrix(n_variables, std::vector<double>(n_variables)),
                first_rotation(n_variables, std::vector<double>(n_variables)),
                second_rotation(n_variables, std::vector<double>(n_variables))
            {
                using namespace transformation::coco;

                for (auto i = 0; i < n_variables; ++i)
                    exponents[i] = static_cast<double>(i) / (static_cast<double>(n_variables) - 1);

                bbob2009_compute_rotation(first_rotation, seed + 1000000, n_variables);
                bbob2009_compute_rotation(second_rotation, seed, n_variables);
                bbob2009_copy_rotation_matrix(first_rotation, transformation_matrix, transformation_base, n_variables);

                for (auto i = 0; i < n_variables; ++i)
                    for (auto j = 0; j < n_variables; ++j)
                        for (auto k = 0; k < n_variables; ++k)
                            second_transformation_matrix[i][j] += first_rotation.at(i).at(k)
                                * pow(condition, exponents.at(k))
                                * second_rotation.at(k).at(j);
            }
        } transformation_state_;


        std::vector<double> transform_objectives(std::vector<double> y) override
        {
            transformation::coco::transform_obj_shift_evaluate_function(y, meta_data_.objective.y.at(0));
            return y;
        }

    public:
        BBOBProblem(const int problem_id, const int instance, const int n_variables, const std::string &name,
                    const double condition = sqrt(10.0)):
            RealProblem(MetaData<double>(problem_id, instance, name, n_variables, 1,
                                         common::OptimizationType::minimization),
                        Constraint<double>(n_variables, 5, -5)),
            transformation_state_(problem_id, instance, n_variables, condition)
        {
            meta_data_.objective = calculate_objective();
        }

        [[nodiscard]]
        Solution<double> calculate_objective() const
        {
            using namespace transformation::coco;
            std::vector<double> x(meta_data_.n_variables, 0);
            bbob2009_compute_xopt(x, transformation_state_.seed, meta_data_.n_variables);
            return Solution<double>{
                x, {bbob2009_compute_fopt(meta_data_.problem_id, meta_data_.instance)}
            };
        }
    };

    template <typename ProblemType>
    class BBOBBase : public BBOBProblem,
                 AutomaticFactoryRegistration<ProblemType, BBOBProblem>,
                 AutomaticFactoryRegistration<ProblemType, RealProblem>
    {
    public:
        using BBOBProblem::BBOBProblem;
    };
}