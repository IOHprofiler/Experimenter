/// \file f_katsuura.hpp
/// \brief cpp file for class Katsuura.
///
/// A detailed file description.
/// Refer "https://github.com/numbbo/coco/blob/master/code-experiments/src/f_katsuura.c"
///
/// \author Furong Ye
/// \date 2019-09-12
#pragma once
#include "bbob_base.hpp"

namespace ioh
{
	namespace problem
	{
		namespace bbob
		{
			class Katsuura : public bbob_base
			{
				const double penalty_factor_ = 1.0;
				std::vector<double> raw_x_;
				
			public:
				Katsuura(int instance_id = DEFAULT_INSTANCE, int dimension = DEFAULT_DIMENSION)
					: bbob_base(23, "Katsuura", instance_id), raw_x_(dimension)
				{
					set_number_of_variables(dimension);
				}


				void prepare_bbob_problem(std::vector<double>& xopt, std::vector<std::vector<double>>& M,
				                          std::vector<double>& b, std::vector<std::vector<double>>& rot1,
				                          std::vector<std::vector<double>>& rot2,
				                          const long rseed, const long n
				) override
				{
					using namespace transformation::coco;
		
					bbob2009_compute_xopt(xopt, rseed, n);
					bbob2009_compute_rotation(rot1, rseed + 1000000, n);
					bbob2009_compute_rotation(rot2, rseed, n);
					for (auto i = 0; i < n; ++i)
					{
						b[i] = 0.0;
						for (auto j = 0; j < n; ++j)
						{
							M[i][j] = 0.0;
							for (auto k = 0; k < n; ++k)
							{
								auto exponent = 1.0 * static_cast<int>(k) / (static_cast<double>(static_cast<long>(n))
									- 1.0);
								M[i][j] += rot1[i][k] * pow(sqrt(100.0), exponent) * rot2[k][j];
							}
						}
					}
				}

				double internal_evaluate(const std::vector<double>& x) override
				{
					auto n = x.size();
					size_t i, j;
					double tmp, tmp2;

					/* Computation core */
					auto result = 1.0;
					for (i = 0; i < n; ++i)
					{
						tmp = 0;
						for (j = 1; j < 33; ++j)
						{
							tmp2 = pow(2., static_cast<double>(j));
							tmp += fabs(tmp2 * x[i] - floor(tmp2 * x[i] + 0.5)) / tmp2;
						}
						tmp = 1.0 + (static_cast<double>(static_cast<long>(i)) + 1) * tmp;
						/*result *= tmp;*/ /* Wassim TODO: delete once consistency check passed*/
						result *= pow(tmp, 10. / pow(static_cast<double>(n), 1.2));
					}
					result = 10. / static_cast<double>(n) / static_cast<double>(n) * (-1. + result);
					return result;
				
				}

				void objectives_transformation(const std::vector<double>& x, std::vector<double>& y,
					const int transformation_id, const int instance_id) override
				{
					transformation::coco::transform_obj_shift_evaluate_function(y, fopt_);
					transformation::coco::transform_obj_penalize_evaluate(raw_x_, lower_bound_, upper_bound_,penalty_factor_, y);
				}

				void variables_transformation(std::vector<double>& x, const int transformation_id,
					const int instance_id) override
				{
					raw_x_ = x;
					transformation::coco::transform_vars_shift_evaluate_function(x, xopt_);
					transformation::coco::transform_vars_affine_evaluate_function(x, M_, b_);
				}
				
				static Katsuura* createInstance(int instance_id = DEFAULT_INSTANCE, int dimension = DEFAULT_DIMENSION)
				{
					return new Katsuura(instance_id, dimension);
				}
			};
		}
	}
}