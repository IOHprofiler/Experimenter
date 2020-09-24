/// \file observer.h
/// \brief Header file for the class observer.
///
/// \author Furong Ye
#pragma once

#include "ioh/common.hpp"
#include "ioh/problem.hpp"


namespace ioh
{
	namespace logger
	{
		/// \brief A class of methods of setting triggers recording evaluations.
		///
		/// Four methods is introduced here:
		///   1. Recording evaluations by a static interval.
		///   2. Recording complete evaluations.
		///   3. Recording evaluations as the best found solution is updated.
		///   4. Recording evaluations at pre-defined points or/and with a static number for each exponential bucket.
		class observer
		{
		public:
			observer() :
				observer_interval(0),
				observer_complete_flag(false),
				observer_update_flag(true),
				observer_time_points({0}),
				evaluations_value1(1),
				time_points_index(0),
				time_points_expi(0),
				observer_time_points_exp_base1(10),
				evaluations_value2(1),
				evaluations_expi(0),
				observer_time_points_exp_base2(10)
			{
			}

			virtual ~observer()
			{
			};

			observer(const observer&) = delete;
			observer& operator =(const observer&) = delete;

			void set_complete_flag(bool complete_flag);

			bool complete_status() const;

			bool complete_trigger() const;

			void set_interval(int interval);

			bool interval_status() const;

			bool interval_trigger(size_t evaluations) const;

			void set_update_flag(bool update_flag);

			bool update_trigger(double fitness, common::optimization_type);

			bool update_status() const;

			void set_time_points(const std::vector<int>& time_points, int number_of_evaluations,
			                     int time_points_exp_base1 = 10, int time_points_exp_base2 = 10);

			bool time_points_status() const;

			bool time_points_trigger(size_t evaluations);

			void reset_observer(common::optimization_type optimization_type);

			virtual void do_log(const std::vector<double>& log_info)
			{
			}

			virtual void track_problem(const problem::base<int>& problem)
			{
			}

			virtual void track_problem(const problem::base<double>& problem)
			{
			}

			/// \todo Adding virtual functions for more IuputType IOHprofiler_problem.

		private:
			int observer_interval; /// < variable for recording by a static interval.
			bool observer_complete_flag; /// < variable for recording complete optimization process. 
			bool observer_update_flag; /// < variable for recording when a better solution is found.

			std::vector<int> observer_time_points; /// < variables for recording at pre-defined points.
			size_t evaluations_value1; /// < intermediate variables for calculating points with 'observer_time_points'.
			size_t time_points_index; /// < intermediate variables for calculating points with 'observer_time_points'.
			int time_points_expi; /// < intermediate variables for calculating points with 'observer_time_points'.
			int observer_time_points_exp_base1;

			int observer_number_of_evaluations;
			/// < variables for recording with a pre-defined times in each exponential boxplot.
			size_t evaluations_value2;
			/// < intermediate variables for calculating points with 'observer_number_of_evaluations'.
			int evaluations_expi;
			/// < intermediate variables for calculating points with 'observer_number_of_evaluations'.
			int observer_time_points_exp_base2;

			/// \todo Currently this is only for single objective optimization.
			double current_best_fitness;
		};
	}
}