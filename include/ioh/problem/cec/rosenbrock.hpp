#pragma once

#include "cec_problem.hpp"
#include "functions.hpp"

namespace ioh::problem::cec
{
    class Rosenbrock final : public CECProblem<Rosenbrock>
    {
    protected:

        double evaluate(const std::vector<double>& x) override
        {
            double&& f = rosenbrock_func(x);
            return f;
        }

        std::vector<double> transform_variables(std::vector<double> x) override
        {
            std::vector<double> y(x.size()), z(x.size());

            scale_and_rotate(x, z, y, this->variables_shift_, this->linear_transformation_, 2.048 / 100.0, 1, 1);

            return z;
        }

    public:

        inline static const int meta_problem_id = 2;
        inline static const std::string meta_name = "CEC_Rosenbrock";

        Rosenbrock(const int instance, const int n_variables) :
            CECProblem(meta_problem_id, instance, n_variables, meta_name)
        {
            this->set_optimum();
        }
    };
}
