/// \file f_N_queens.hpp
/// \brief cpp file for class f_N_queens.
///
/// A detailed file description.
///
/// \author Naama Horesh and Furong Ye
/// \date 2019-06-27
#pragma once
#include "pbo_base.hpp"


namespace ioh
{
	namespace problem
	{
		namespace pbo
		{
			class NQueens : public pbo_base
			{
			public:
				NQueens(int instance_id = DEFAULT_INSTANCE, int dimension = DEFAULT_DIMENSION)
					: pbo_base("NQueens", instance_id)
				{
					set_number_of_variables(dimension);
				}

				void customize_optimal() override
				{
					set_optimal(sqrt(static_cast<double>(get_number_of_variables())));
				}

				// TODO: why not use std::max?
				double _max(double element1, double element2)
				{
					if (element1 > element2)
					{
						return element1;
					}
					return element2;
				}
				
				double internal_evaluate(const std::vector<int>& x) override
				{
					double result;
					auto n = x.size();
					int index, j, i, k, l;
					auto N_queens = static_cast<int>(sqrt(static_cast<double>(n)) + 0.5);
					auto number_of_queens_on_board = 0;
					auto k_penalty = 0.0;
					auto l_penalty = 0.0;
					auto raws_penalty = 0.0;
					auto columns_penalty = 0.0;
					auto indx = 0;
					auto C = static_cast<float>(N_queens);

					if (floor(sqrt(static_cast<double>(n))) != sqrt(static_cast<double>(n)))
					{
						common::log::error("Number of parameters in the N Queen problem must be a square number");
					}

					for (index = 0; index < n; index++)
					{
						if (x[index] == 1)
						{
							number_of_queens_on_board += 1;
						}
					}

					for (j = 1; j <= N_queens; j++)
					{
						auto sum_column = 0.0;
						for (i = 1; i <= N_queens; i++)
						{
							indx = ((i - 1) * N_queens) + ((j - 1) % N_queens);
							sum_column += static_cast<double>(x[indx]);
						}
						columns_penalty += _max(0.0, (-1.0 + sum_column));
					}

					for (i = 1; i <= N_queens; i++)
					{
						auto sum_raw = 0.0;
						/*double sum_k = 0.0;
						double sum_l = 0.0;*/
						for (j = 1; j <= N_queens; j++)
						{
							indx = ((i - 1) * N_queens) + ((j - 1) % N_queens);
							sum_raw += static_cast<double>(x[indx]);
						}
						raws_penalty += _max(0.0, (-1.0 + sum_raw));
					}

					for (k = 2 - N_queens; k <= N_queens - 2; k++)
					{
						auto sum_k = 0.0;
						for (i = 1; i <= N_queens; i++)
						{
							if (k + i >= 1 && k + i <= N_queens)
							{
								indx = ((i - 1) * N_queens) + ((k + i - 1) % N_queens);
								sum_k += static_cast<double>(x[indx]);
							}
						}
						k_penalty += _max(0.0, (-1.0 + sum_k));
					}
					for (l = 3; l <= 2 * N_queens - 1; l++)
					{
						auto sum_l = 0.0;
						for (i = 1; i <= N_queens; i++)
						{
							if (l - i >= 1 && l - i <= N_queens)
							{
								indx = ((i - 1) * N_queens) + ((l - i - 1) % N_queens);
								sum_l += static_cast<double>(x[indx]);
							}
						}
						l_penalty += _max(0.0, (-1.0 + sum_l));
					}
					result = static_cast<double>(number_of_queens_on_board - (C * raws_penalty) - (C * columns_penalty)
						- (C * k_penalty) - (C *
							l_penalty));
					return static_cast<double>(result);
				}

				static NQueens* createInstance(int instance_id = DEFAULT_INSTANCE, int dimension = DEFAULT_DIMENSION)
				{
					return new NQueens(instance_id, dimension);
				}
			};
		};
	}
}