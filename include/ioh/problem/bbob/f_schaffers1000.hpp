/// \file f_Schaffers1000.hpp
/// \brief cpp file for class Schaffers1000.
///
/// A detailed file description.
/// Refer "https://github.com/numbbo/coco/blob/master/code-experiments/src/f_schaffers.c"
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
			// TODO: Create single base class for Schaffers, only conditioning is different
			class Schaffers1000 : public bbob_base
			{
				const double conditioning_ = 1000.0;
				const double penalty_factor_ = 10.0;
				std::vector<std::vector<double>> M1_;
				std::vector<double> b1_;
				std::vector<double> raw_x_;
			public:
				Schaffers1000(int instance_id = DEFAULT_INSTANCE, int dimension = DEFAULT_DIMENSION)
					: bbob_base(18, "Schaffers1000", instance_id),
					M1_(dimension, std::vector<double>(dimension)),
					b1_(dimension),
					raw_x_(dimension)
				{
					set_number_of_variables(dimension);
				}

				void prepare_bbob_problem(std::vector<double>& xopt, std::vector<std::vector<double>>& M,
				                          std::vector<double>& b, std::vector<std::vector<double>>& rot1,
				                          std::vector<std::vector<double>>& rot2,
				                          const long rseed, const long n
				) override
				{
					transformation::coco::bbob2009_compute_xopt(xopt, rseed, n);
					transformation::coco::bbob2009_compute_rotation(rot1, rseed + 1000000, n);
					transformation::coco::bbob2009_compute_rotation(rot2, rseed, n);
					transformation::coco::bbob2009_copy_rotation_matrix(rot1, M1_, b1_, n);
					for (auto i = 0; i < n; ++i)
					{
						b[i] = 0.0;
						for (auto j = 0; j < n; ++j)
						{
							auto exponent = 1.0 * static_cast<int>(i) / (static_cast<double>(static_cast<long>(n)) -
								1.0);
							M[i][j] = rot2[i][j] * pow(sqrt(conditioning_), exponent);
						}
					}
				}

				void objectives_transformation(const std::vector<double>& x, std::vector<double>& y,
					const int transformation_id, const int instance_id) override
				{
					transformation::coco::transform_obj_shift_evaluate_function(y, fopt_);
					transformation::coco::transform_obj_penalize_evaluate(raw_x_, lower_bound_, upper_bound_,	penalty_factor_, y);
				}

				void variables_transformation(std::vector<double>& x, const int transformation_id,
					const int instance_id) override
				{
					raw_x_ = x;
					transformation::coco::transform_vars_shift_evaluate_function(x, xopt_);
					transformation::coco::transform_vars_affine_evaluate_function(x, M1_, b1_);
					transformation::coco::transform_vars_asymmetric_evaluate_function(x, 0.5);
					transformation::coco::transform_vars_affine_evaluate_function(x, M_, b_);
				}


				double internal_evaluate(const std::vector<double>& x) override
				{
					auto n = x.size();
					/* Computation core */
					auto result = 0.0;
					for (size_t i = 0; i < n - 1; ++i)
					{
						const auto tmp = x[i] * x[i] + x[i + 1] * x[i + 1];
						if (std::isinf(tmp) && std::isnan(sin(50.0 * pow(tmp, 0.1)))) /* sin(inf) -> nan */
							/* the second condition is necessary to pass the integration tests under Windows and Linux */
							return tmp;
						result += pow(tmp, 0.25) * (1.0 + pow(sin(50.0 * pow(tmp, 0.1)), 2.0));
					}
					return pow(result / (static_cast<double>(static_cast<long>(n)) - 1.0), 2.0);
				}

				static Schaffers1000* createInstance(int instance_id = DEFAULT_INSTANCE,
				                                     int dimension = DEFAULT_DIMENSION)
				{
					return new Schaffers1000(instance_id, dimension);
				}
			};
		}
	}
}