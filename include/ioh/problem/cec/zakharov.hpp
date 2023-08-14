#pragma once

#include "cec_problem.hpp"

namespace ioh::problem::cec
{
    class Zakharov final : public CECProblem<Zakharov>
    {
    protected:

        double evaluate(const std::vector<double> &x)
        {
            double sum1 = 0.0, sum2 = 0.0;
            int i = 0;

            for (const auto &xi : x)
            {
                sum1 += xi * xi;
                sum2 += 0.5 * (i+1) * xi;
                i++;
            }

            double&& result = sum1 + pow(sum2, 2) + pow(sum2, 4);
            std::cout << "result: " << result << std::endl;

            return result;
        }

        std::vector<double> transform_variables(std::vector<double> x) override
        {
            auto&& transformed_variables = this->basic_transform(x, 1, 0);
            return transformed_variables;
        }

    public:

        Zakharov(const int instance, const int n_variables) :
            CECProblem (1, instance, n_variables, "CEC_Zakharov")
        {
        }
    };
}